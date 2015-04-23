/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

package org.allseen.timeservice.sample.server.logic;

import java.util.Calendar;
import java.util.Timer;
import java.util.TimerTask;

import org.alljoyn.bus.ErrorReplyBusException;
import org.allseen.timeservice.Schedule;
import org.allseen.timeservice.Schedule.WeekDay;
import org.allseen.timeservice.TimeServiceException;
import org.allseen.timeservice.sample.server.ui.TimeSampleServer;
import org.allseen.timeservice.server.Alarm;

import android.content.Context;
import android.text.format.Time;
import android.util.Log;

/**
 *  Represents an Alarm {@link  org.allseen.timeservice.server.Alarm}.
 *  Sets an Alarm to expire. Upon designated time reached, a relevant signal is emitted.
 *  Calculates the alarm's designated time relatively to the system time.
 *  In case the time of the clock changes, the time of the alarm is updated accordingly.
 */
public class ServerAlarm extends Alarm {

    /**
     * Log Tag
     */
    private static final String TAG = "ServerAlarm";

    /**
     * Milliseconds in a day
     */
    private static final long DAY_MSEC = 24 * 60 * 60 * 1000;

    /**
     * Timer to activate TimeTask
     */
    private Timer timer = new Timer();

    /**
     * Stores context
     */
    private final Context context;

    /**
     * {@link Schedule}
     */
    private Schedule schedule;

    /**
     * Alarm title
     */
    private String title;

    /**
     * Is Alarm enabled
     */
    private boolean isEnabled;

    /**
     * Constructor
     */
    public ServerAlarm(Context context) {
        super();
        this.context = context;
        Time t = new Time(Time.getCurrentTimezone());
        t.setToNow();
        org.allseen.timeservice.Time ts = new org.allseen.timeservice.Time((byte) 0, (byte) 0, (byte) 0, (short) 0);
        schedule = new Schedule(ts);
        title = "";
        // in case the clock has changed we need to update the alarm with regards to the new time
        TimeOffsetManager.getInstance().registerTimeChangeListener(new TimeChangeListener() {
            @Override
            public void timeHasChanged(long newTime) {
                if (isEnabled) {
                    Log.d(TAG, "Clock has changed updating Alarm [" + getObjectPath() + "]");
                    startNewTimer();
                }
            }
        });
        TimeSampleServer.sendMessage(context, TimeSampleServer.ALARM_EVENT_ACTION, getObjectPath(), "Alarm has been created");

    }

    @Override
    public Schedule getSchedule() {
        TimeSampleServer.sendMessage(context, TimeSampleServer.ALARM_EVENT_ACTION, getObjectPath(), "Alarm getSchedule request");
        return schedule;
    }

    @Override
    public void setSchedule(Schedule schedule) throws ErrorReplyBusException {
        TimeSampleServer.sendMessage(context, TimeSampleServer.ALARM_EVENT_ACTION, getObjectPath(), "Alarm setSchedule request");
        this.schedule = schedule;
        if (isEnabled) {
            startNewTimer();
        }
    }

    @Override
    public String getTitle() {
        TimeSampleServer.sendMessage(context, TimeSampleServer.ALARM_EVENT_ACTION, getObjectPath(), "Alarm getTitle request");
        return title;
    }

    @Override
    public void setTitle(String title) {
        TimeSampleServer.sendMessage(context, TimeSampleServer.ALARM_EVENT_ACTION, getObjectPath(), "Alarm setTitle request");
        this.title = title;
    }

    @Override
    public boolean isEnabled() {
        TimeSampleServer.sendMessage(context, TimeSampleServer.ALARM_EVENT_ACTION, getObjectPath(), "Alarm isEnabled request");
        return isEnabled;
    }

    @Override
    public void setEnabled(boolean enabled) {

        TimeSampleServer.sendMessage(context, TimeSampleServer.ALARM_EVENT_ACTION, getObjectPath(), "Alarm setEnabled request");

        this.isEnabled = enabled;
        if (isEnabled) {
            startNewTimer();
        } else {
            timer.cancel();
            timer.purge();
            timer = new Timer();
        }

    }

    @Override
    public void alarmReached() throws TimeServiceException {
        Log.d(TAG, "alarmReached for [" + getObjectPath() + "]");
        TimeSampleServer.sendMessage(context, TimeSampleServer.ALARM_EVENT_ACTION, getObjectPath(), "Alarm alarmReached Event");
        super.alarmReached();
    }

    /**
     * Starts a new timer. Checks if it's a one time Alarm ,in this case use the {@link Timer#schedule(TimerTask, long)} else use {@link Timer#scheduleAtFixedRate(TimerTask, long, long)}
     */
    private void startNewTimer() {
        timer.cancel();
        timer.purge();
        timer = new Timer();
        final ServerAlarm alarm = this;
        // use the server clock date and fill with the alarm schedule time .
        Calendar rawAlarmTime = TimeOffsetManager.getInstance().getCurrentServerTime();
        rawAlarmTime.set(Calendar.HOUR_OF_DAY, schedule.getTime().getHour());
        rawAlarmTime.set(Calendar.MINUTE, schedule.getTime().getMinute());
        rawAlarmTime.set(Calendar.SECOND, schedule.getTime().getSeconds());
        rawAlarmTime.set(Calendar.MILLISECOND, schedule.getTime().getMilliseconds());
        Log.d(TAG, "Original  Alarm received for [" + getObjectPath() + "] is  " + TimeOffsetManager.getInstance().getDateAndTimeDisplayFormat().format(rawAlarmTime.getTime()));

        Calendar alarmInServerTime = Calendar.getInstance();
        alarmInServerTime.setTimeInMillis(rawAlarmTime.getTimeInMillis() + TimeOffsetManager.getInstance().getTimeDifference());
        Log.d(TAG, "Calculated  for [" + getObjectPath() + "] is  " + TimeOffsetManager.getInstance().getDateAndTimeDisplayFormat().format(alarmInServerTime.getTime()));

        if (schedule.getWeekDays().size() == 0) {
            // In this case the alarm is a one time alarm.
            timer.schedule(new TimerTask() {
                @Override
                public void run() {

                    try {
                        alarm.alarmReached();
                        Log.d(TAG, "Alarms expired for [" + getObjectPath() + "]");
                    } catch (TimeServiceException e) {
                        Log.e(TAG, "Unable to send alarmReached signal", e);
                    }
                }
            }, alarmInServerTime.getTime());
        } else {
            // In this case the alarm is a recurring alarm.
            timer.schedule(new TimerTask() {
                @Override
                public void run() {
                    // Calendar.DAY_OF_WEEK gives us the day in range of 1-7
                    boolean alarmExpired = false;
                    for (WeekDay weekDay : schedule.getWeekDays()) {
                        // WeekDay ordinal is 0-6
                        if (weekDay.ordinal() == TimeOffsetManager.getInstance().getCurrentServerTime().get(Calendar.DAY_OF_WEEK) - 1) {
                            alarmExpired = true;
                            break;
                        }
                    }
                    if (alarmExpired) {
                        try {
                            alarm.alarmReached();
                            Log.d(TAG, "Alarms expired for [" + getObjectPath() + "]");
                        } catch (TimeServiceException e) {
                            Log.e(TAG, "Unable to send alarmReached signal", e);
                        }
                    }
                }
            }, alarmInServerTime.getTime(), DAY_MSEC);
        }
    }
}
