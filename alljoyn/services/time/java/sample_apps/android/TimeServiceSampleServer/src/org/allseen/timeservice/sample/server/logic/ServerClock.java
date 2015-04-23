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

import org.allseen.timeservice.Date;
import org.allseen.timeservice.DateTime;
import org.allseen.timeservice.sample.server.ui.TimeSampleServer;
import org.allseen.timeservice.server.Clock;

import android.content.Context;
import android.text.format.Time;
import android.util.Log;

/**
 * Represents an ServerClock {@link  org.allseen.timeservice.server.Clock}.
 *
 */
public class ServerClock extends Clock {

    /**
     * Log Tag
     */
    private static final String TAG = "ServerClock";

    /**
     *  Stores context
     */
    protected Context context;

    /**
     *  Stores {@link DateTime}
     */
    private DateTime dt;

    /**
     *  Stores if clock can be set.
     */
    private boolean isSet;


    private boolean isSetable;

    /**
     * Constructor
     */
    public ServerClock(Context context) {

        super();
        this.context=context;
        Time t = new Time(Time.getCurrentTimezone());
        t.setToNow();
        byte seconds = (byte) ((t.second > 59) ? 59 :  t.second);
        org.allseen.timeservice.Time ts = new org.allseen.timeservice.Time((byte)t.hour, (byte) t.minute,seconds, (short)0);
        org.allseen.timeservice.Date ds = new Date((short)t.year, (byte)(t.month + 1), (byte)t.monthDay);
        dt = new DateTime(ds, ts, (short)0);
        TimeSampleServer.sendMessage(context, TimeSampleServer.CLOCK_EVENT_ACTION, getObjectPath(), "ServerClock created");
    }

    @Override
    public DateTime getDateTime() {
        Calendar currentCalendar=TimeOffsetManager.getInstance().getCurrentServerTime();
        TimeSampleServer.sendMessage(context, TimeSampleServer.CLOCK_EVENT_ACTION, getObjectPath(), "ServerClock getDateTime request");
        dt=TimeOffsetManager.getInstance().convertCalendarToDateTime(currentCalendar);
        return dt;
    }


    /**
     *  Change the time and date of the application.
     *  Update the {@link TimeChangeListener} about the change.
     *  In case isSetable is false will throw RuntimeException.
     */
    @Override
    public void setDateTime(DateTime dateTime) {
    	Log.d(TAG,"Got the following data date="+ dt.getDate() +" Time="+ dt.getTime());
    	if (!isSetable){
    	    throw new RuntimeException("Clock can't be set");
    	}
        this.dt = dateTime;
        Calendar newTime=TimeOffsetManager.getInstance().convertDateTimeToCalendar(dateTime);
        String formattedDate = TimeOffsetManager.getInstance().getDateAndTimeDisplayFormat().format(newTime.getTime());
        long diff=Calendar.getInstance().getTimeInMillis()-newTime.getTimeInMillis();
        TimeOffsetManager.getInstance().setTimeDifference(diff);
        TimeSampleServer.sendMessage(context, TimeSampleServer.CLOCK_EVENT_ACTION, getObjectPath(), "ServerClock setDateTime request to "+formattedDate );
        isSet=true;
    }

    @Override
    public boolean getIsSet() {
        TimeSampleServer.sendMessage(context, TimeSampleServer.CLOCK_EVENT_ACTION, getObjectPath(), "ServerClock getIsSet request");
        return isSet;
    }

    /**
     * This is an application related flag.(not related to TimeServiceApi)
     * This flag indicates whether the clock can be set remotely.
     * @param isSetable
     */
    public void setIsSetable(boolean  isSetable){
     this.isSetable=isSetable;
    }
}
