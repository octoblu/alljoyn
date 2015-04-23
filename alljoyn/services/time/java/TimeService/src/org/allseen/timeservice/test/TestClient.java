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

package org.allseen.timeservice.test;

import java.util.EnumSet;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.UUID;

import org.alljoyn.about.AboutKeys;
import org.alljoyn.bus.AboutListener;
import org.alljoyn.bus.AboutObjectDescription;
import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.BusException;
import org.alljoyn.bus.ProxyBusObject;
import org.alljoyn.bus.Status;
import org.alljoyn.bus.Variant;
import org.alljoyn.services.common.utils.TransportUtil;
import org.allseen.timeservice.AuthorityType;
import org.allseen.timeservice.Date;
import org.allseen.timeservice.DateTime;
import org.allseen.timeservice.Period;
import org.allseen.timeservice.Schedule;
import org.allseen.timeservice.Schedule.WeekDay;
import org.allseen.timeservice.TimeServiceConst;
import org.allseen.timeservice.TimeServiceException;
import org.allseen.timeservice.client.Alarm;
import org.allseen.timeservice.client.Alarm.AlarmHandler;
import org.allseen.timeservice.client.AlarmFactory;
import org.allseen.timeservice.client.Clock;
import org.allseen.timeservice.client.Clock.TimeAuthorityHandler;
import org.allseen.timeservice.client.SessionListenerHandler;
import org.allseen.timeservice.client.TimeServiceClient;
import org.allseen.timeservice.client.Timer;
import org.allseen.timeservice.client.Timer.TimerHandler;
import org.allseen.timeservice.client.TimerFactory;
import org.allseen.timeservice.server.TimeServiceServer;
import org.allseen.timeservice.test.TestServerCustomAlarm.AlarmCoolIface;

import android.text.format.Time;
import android.util.Log;

/**
 * Test application client side
 */
public class TestClient implements AboutListener, SessionListenerHandler {

    private static final String TAG = "ajtsapp" + TestClient.class.getSimpleName();

    /**
     * Test application
     */
    private final TimeServiceTestApp app;

    /**
     * TimeService clients
     */
    private final Map<String, TimeServiceClient> timeClients;

    /**
     * {@link Alarm}s created by the {@link AlarmFactory}
     */
    private final Map<String, Alarm> createdAlarms;

    /**
     * {@link Timer}s created by the {@link TimerFactory}
     */
    private final Map<String, Timer> createdTimers;

    /**
     * Constructor
     *
     * @param app
     *            Test application
     * @throws Exception
     */
    public TestClient(TimeServiceTestApp app) {

        this.app = app;
        timeClients = new HashMap<String, TimeServiceClient>();
        createdAlarms = new HashMap<String, Alarm>();
        createdTimers = new HashMap<String, Timer>();
    }

    /**
     * Init TestClient service
     */
    public void init() throws Exception {

        app.getBusAttachment().registerAboutListener(this);
        app.getBusAttachment().whoImplements(new String[] { TimeServiceConst.IFNAME_PREFIX + "*" });
    }

    /**
     * Shutdown
     */
    public void shutdown() {

        for (TimeServiceClient tsc : timeClients.values()) {

            tsc.release();
        }

        timeClients.clear();

        releaseCreatedAlarms();
        releaseCreatedTimers();

        app.getBusAttachment().cancelWhoImplements(new String[] { TimeServiceConst.IFNAME_PREFIX + "*" });
        app.getBusAttachment().unregisterAboutListener(this);

    }

    /**
     * @see org.alljoyn.services.common.AnnouncementHandler#onAnnouncement(java.lang.String,
     *      short, org.alljoyn.services.common.BusObjectDescription[],
     *      java.util.Map)
     */
    @Override
    public void announced(String busName, int version, short port, AboutObjectDescription[] objDescs, Map<String, Variant> aboutData) {

        BusAttachment bus = app.getBusAttachment();
        bus.enableConcurrentCallbacks();

        Variant deviceIdVar = aboutData.get(AboutKeys.ABOUT_DEVICE_ID);
        Variant appIdVar    = aboutData.get(AboutKeys.ABOUT_APP_ID);

        if (deviceIdVar == null || appIdVar == null) {

            Log.e(TAG, "A bad announcement received from '" + busName + "', deviceId, or appId are undefined");
            return;
        }

        String deviceId;
        UUID appId;
        try {

            deviceId         = deviceIdVar.getObject(String.class);
            byte[] appIdByte = appIdVar.getObject(byte[].class);
            appId            = TransportUtil.byteArrayToUUID(appIdByte);
        } catch (BusException be) {

            Log.e(TAG, "A bad announcement received from '" + busName + "', failed to unmarshal data", be);
            return;
        }

        if (appId == null) {

            Log.e(TAG, "A bad announcement received from '" + busName + "', failed to create UUID object");
            return;
        }

        String key = getKey(deviceId, appId);

        Log.i(TAG, "Received Announcement from bus: '" + busName + "', key: '" + key + "'");

        TimeServiceClient timeClient = timeClients.get(key);
        if (timeClient != null) {

            Log.d(TAG, "The TimeClient for the key: '" + key + "', already exists, releasing its resources");
            timeClient.release();
        }

        timeClient = new TimeServiceClient(bus, busName, deviceId, appId, objDescs);
        timeClients.put(key, timeClient);
    }// onAnnouncement

    /**
     * @see org.allseen.timeservice.client.SessionListenerHandler#sessionLost(int,
     *      org.allseen.timeservice.client.TimeServiceClient)
     */
    @Override
    public void sessionLost(int reason, TimeServiceClient timeServiceClient) {

        Log.d(TAG, "Session Lost busName: '" + timeServiceClient.getServerBusName() + "'");
    }

    /**
     * @see org.allseen.timeservice.client.SessionListenerHandler#sessionJoined(org.allseen.timeservice.client.TimeServiceClient,
     *      org.alljoyn.bus.Status)
     */
    @Override
    public void sessionJoined(TimeServiceClient timeServiceClient, Status status) {

        Log.d(TAG, "Session Joined busName: '" + timeServiceClient.getServerBusName() + "', Status: '" + status + "'");
    }

    /**
     * Connect {@link TimeServiceClient} identified by the given key to the
     * {@link TimeServiceServer}
     */
    public void connect(String key) {

        TimeServiceClient tsc = timeClients.get(key);
        if (tsc == null) {

            Log.w(TAG, "Not found client with the key: '" + key + "'");
            return;
        }

        tsc.joinSessionAsync(this);
    }

    /**
     * Disconnect {@link TimeServiceClient} identified by the given key from the
     * {@link TimeServiceServer}
     *
     * @param key
     */
    public void disconnect(String key) {

        TimeServiceClient tsc = timeClients.get(key);
        if (tsc == null) {

            Log.w(TAG, "Not found client with the key: '" + key + "'");
            return;
        }
        tsc.leaveSession();
    }

    /**
     * Print all the device keys that the client received announcements from
     */
    public void printDevices() {

        for (String key : timeClients.keySet()) {

            Log.d(TAG, key);
        }
    }

    /**
     * Print out all the Clock info
     */
    public void printClock(String key, String objectPath) {

        Clock clock = findClock(key, objectPath);
        if (clock == null) {

            Log.w(TAG, "Clock not found");
            return;
        }

        try {

            AuthorityType authorityType = null;

            if (clock.isAuthority()) {

                authorityType = clock.retrieveAuthorityType();
            }

            Log.d(TAG, String.format("Clock: Authority: '%s', AuthType: '%s', DateTime: '%s', IsSet: '%s'", clock.isAuthority(), authorityType, clock.retrieveDateTime(), clock.retrieveIsSet()));
        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to retrieve Clock", tse);
        }
    }

    /**
     * Set the clock
     *
     * @param key
     * @param objectPath
     */
    public void setClock(String key, String objectPath) {

        Clock clock = findClock(key, objectPath);
        if (clock == null) {

            Log.w(TAG, "Clock not found");
            return;
        }

        Time t = new Time(Time.getCurrentTimezone());
        t.setToNow();

        org.allseen.timeservice.Time ts = getTime();
        org.allseen.timeservice.Date ds = new Date((short) t.year, (byte) (t.month + 1), (byte) t.monthDay);

        try {

            clock.setDateTime(new DateTime(ds, ts, (short) 120));
        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to set the clock", tse);
        }
    }

    /**
     * Register TimeSync signals listener
     *
     * @param key
     * @param objectPath
     */
    public void regTimeSyncHandler(String key, String objectPath) {

        Clock clock = findClock(key, objectPath);
        if (clock == null) {

            Log.w(TAG, "Clock not found");
            return;
        }

        clock.registerTimeAuthorityHandler(new TimeAuthorityHandler() {
            @Override
            public void handleTimeSync(Clock clock) {

                Log.i(TAG, "Received 'TimeSync' signal from a Clock: '" + clock.getObjectPath() + "'");
            }
        });
    }

    /**
     * Register TimeSync signals listener
     *
     * @param key
     * @param objectPath
     */
    public void unRegTimeSyncHandler(String key, String objectPath) {

        Clock clock = findClock(key, objectPath);
        if (clock == null) {

            Log.w(TAG, "Clock not found");
            return;
        }

        clock.unregisterTimeAuthorityHandler();
    }

    /**
     * Find clock
     *
     * @param key
     * @param objectPath
     * @return {@link Clock} or NULL
     */
    public Clock findClock(String key, String objectPath) {

        TimeServiceClient tsc = timeClients.get(key);
        if (tsc == null) {

            return null;
        }

        List<Clock> clocks = tsc.getAnnouncedClockList();
        for (Clock clock : clocks) {

            if (clock.getObjectPath().equals(objectPath)) {

                return clock;
            }
        }

        return null;
    }

    /**
     * Print out all the Alarm info
     */
    public void printAlarm(String key, String objectPath) {

        Alarm alarm = findAlarm(key, objectPath);
        if (alarm == null) {

            Log.w(TAG, "Alarm not found");
            return;
        }

        try {

            String[] descLangs = alarm.retrieveDescriptionLanguages();
            String lang = "";

            if (descLangs != null) {

                lang = descLangs.length > 0 ? descLangs[0] : "";
            }

            Log.d(TAG, String.format("Alarm: Enabled: '%s', Schedule: '%s', Title: '%s', Description: '%s'", alarm.retrieveIsEnabled(), alarm.retrieveSchedule(), alarm.retrieveTitle(),
                    lang.length() > 0 ? alarm.retrieveObjectDescription(lang) : ""));

        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to retrieve Alarm", tse);
        }
    }

    /**
     * Print out Custom Alarm info
     *
     * @param objectPath
     */
    public void printCsAlarm(String key, String objectPath) {

        TimeServiceClient tsClient = timeClients.get(key);
        if (tsClient == null) {

            Log.w(TAG, "Alarm not found");
            return;
        }

        Integer sid = tsClient.getSessionId();
        if (sid == null) {

            Log.w(TAG, "No connection with the Alarm");
            return;
        }

        ProxyBusObject proxy = app.getBusAttachment().getProxyBusObject(tsClient.getServerBusName(), objectPath, sid, new Class<?>[] { AlarmCoolIface.class });

        try {

            String title = proxy.getInterface(AlarmCoolIface.class).getCoolTitle();
            Log.d(TAG, "Custom Alarm cool title: '" + title + "', object: '" + objectPath + "'");
        } catch (BusException be) {

            Log.e(TAG, "Failed to call a method of CustomAlarm", be);
        }
    }

    /**
     * Set Alarm
     *
     * @param days
     *            Comma separated string of i.e.: sun,mon,tue,wen,thu,fri,sat
     * @param title
     * @param enable
     */
    public void setAlarm(String key, String obj, String days, String title, String enable) {

        Alarm alarm = findAlarm(key, obj);

        if (alarm == null) {

            Log.d(TAG, "Alarm not found");
            return;
        }

        if (days != null) {

            Map<String, WeekDay> wd = new HashMap<String, WeekDay>();
            wd.put("sun", WeekDay.SUNDAY);
            wd.put("mon", WeekDay.MONDAY);
            wd.put("tue", WeekDay.TUESDAY);
            wd.put("wen", WeekDay.WEDNESDAY);
            wd.put("thu", WeekDay.THURSDAY);
            wd.put("fri", WeekDay.FRIDAY);
            wd.put("sat", WeekDay.SATURDAY);

            Set<WeekDay> weekDays = EnumSet.noneOf(WeekDay.class);
            for (String day : days.split(",")) {

                WeekDay enWd = wd.get(day.trim());
                if (enWd != null) {

                    weekDays.add(enWd);
                }
            }

            org.allseen.timeservice.Time ts = getTime();

            Schedule schedule;

            if (weekDays.size() > 0) {

                schedule = new Schedule(ts, weekDays);
            } else {
                schedule = new Schedule(ts);
            }

            try {
                alarm.setSchedule(schedule);
            } catch (TimeServiceException tse) {

                Log.e(TAG, "Failed to call setSchedule", tse);
            }
        }// if :: days

        if (title != null) {

            try {
                alarm.setTitle(title);
            } catch (TimeServiceException tse) {

                Log.e(TAG, "Failed to call setTitle", tse);
            }
        }

        if (enable != null) {

            try {
                if ("t".equals(enable)) {

                    alarm.setEnabled(true);
                } else {
                    alarm.setEnabled(false);
                }
            } catch (TimeServiceException tse) {

                Log.d(TAG, "Failed to call setEnabled", tse);
            }
        }
    }

    /**
     * Register AlarmHandler
     *
     * @param key
     * @param obj
     */
    public void regAlarmHandler(String key, String obj) {

        Alarm alarm = findAlarm(key, obj);
        if (alarm == null) {

            Log.w(TAG, "Alarm not found");
            return;
        }

        alarm.registerAlarmHandler(new AlarmHandler() {
            @Override
            public void handleAlarmReached(Alarm alarm) {
                Log.i(TAG, "Received 'AlarmReached' signal from an Alarm: '" + alarm.getObjectPath() + "'");
            }
        });
    }

    /**
     * Unregister Alarm Handler
     *
     * @param key
     * @param obj
     */
    public void unRegAlarmHandler(String key, String obj) {

        Alarm alarm = findAlarm(key, obj);
        if (alarm == null) {

            Log.w(TAG, "Alarm not found");
            return;
        }

        alarm.unregisterAlarmHandler();
    }

    /**
     * Find Alarm
     *
     * @param key
     * @param objectPath
     * @return {@link Alarm}
     */
    public Alarm findAlarm(String key, String objectPath) {

        // First search among the Alarms created by the AlarmFactory
        Alarm alarm = createdAlarms.get(objectPath);

        if (alarm != null) {

            return alarm;
        }

        // Not found the Alarm among the created Alarms, search it among the
        // Announced alarms
        TimeServiceClient tsc = timeClients.get(key);
        if (tsc == null) {

            return null;
        }

        List<Alarm> alarms = tsc.getAnnouncedAlarmList();
        for (Alarm annAlarm : alarms) {

            if (annAlarm.getObjectPath().equals(objectPath)) {

                return annAlarm;
            }
        }

        return null;
    }

    /**
     * Find Alarm Factory
     *
     * @param key
     * @param objectPath
     * @return {@link Alarm}
     */
    public AlarmFactory findAlarmFactory(String key, String objectPath) {

        TimeServiceClient tsc = timeClients.get(key);
        if (tsc == null) {

            return null;
        }

        List<AlarmFactory> alarmFactories = tsc.getAnnouncedAlarmFactoryList();
        for (AlarmFactory factory : alarmFactories) {

            if (factory.getObjectPath().equals(objectPath)) {

                return factory;
            }
        }

        return null;
    }

    /**
     * Create new {@link Alarm} with the {@link AlarmFactory}
     *
     * @param key
     *            device
     * @param objectPath
     *            {@link AlarmFactory} object path
     */
    public void newAlarm(String key, String objectPath) {

        AlarmFactory factory = findAlarmFactory(key, objectPath);

        if (factory == null) {

            Log.w(TAG, "Alarm Factory not found");
            return;
        }

        try {

            Alarm alarm = factory.newAlarm();
            createdAlarms.put(alarm.getObjectPath(), alarm);
        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to create new Alarm", tse);
        }
    }

    /**
     * Delete {@link Alarm} of the {@link AlarmFactory}
     *
     * @param key
     *            device
     * @param objectPath
     *            {@link AlarmFactory} object path
     * @param alarmToDeleteObjPath
     *            Object path of the {@link Alarm} to be deleted
     */
    public void deleteAlarm(String key, String objectPath, String alarmToDeleteObjPath) {

        AlarmFactory factory = findAlarmFactory(key, objectPath);

        if (factory == null) {

            Log.w(TAG, "Alarm Factory not found");
            return;
        }

        Alarm deletedAlarm = createdAlarms.get(alarmToDeleteObjPath);
        if (deletedAlarm == null) {

            Log.w(TAG, "Alarm to be deleted is not found, objPath: '" + alarmToDeleteObjPath + "'");
            return;
        }

        try {

            factory.deleteAlarm(alarmToDeleteObjPath);
            deletedAlarm.release();
        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to delete Alarm", tse);
        }
    }

    /**
     * Retrieve {@link Alarm} objects of the given {@link AlarmFactory}
     *
     * @param key
     *            device
     * @param objectPath
     *            {@link AlarmFactory}
     */
    public void getAlarms(String key, String objectPath) {

        AlarmFactory factory = findAlarmFactory(key, objectPath);

        if (factory == null) {

            Log.w(TAG, "Alarm Factory not found");
            return;
        }

        List<Alarm> alarms;
        try {

            alarms = factory.retrieveAlarmList();
        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to retrieve List Of Alarms", tse);
            return;
        }

        createdAlarms.clear();
        for (Alarm alarm : alarms) {

            createdAlarms.put(alarm.getObjectPath(), alarm);
        }
    }

    /**
     * Find Timer
     *
     * @param key
     * @param objectPath
     * @return {@link Timer}
     */
    public Timer findTimer(String key, String objectPath) {

        // First search among the Timers created by the TimerFactory
        Timer timer = createdTimers.get(objectPath);

        if (timer != null) {

            return timer;
        }

        TimeServiceClient tsc = timeClients.get(key);
        if (tsc == null) {

            return null;
        }

        List<Timer> timers = tsc.getAnnouncedTimerList();
        for (Timer annTimer : timers) {

            if (annTimer.getObjectPath().equals(objectPath)) {

                return annTimer;
            }
        }

        return null;
    }

    /**
     * Print out all the Timer info
     */
    public void printTimer(String key, String objectPath) {

        Timer timer = findTimer(key, objectPath);
        if (timer == null) {

            Log.w(TAG, "Timer not found");
            return;
        }

        try {

            String[] descLangs = timer.retrieveDescriptionLanguages();
            String lang = "";

            if (descLangs != null) {

                lang = descLangs.length > 0 ? descLangs[0] : "";
            }

            Log.d(TAG, String.format("Timer: Interval: '%s', IsRunning: '%s', Repeat: '%s', TimeLeft: '%s', " + " Title: '%s', Description: '%s'", timer.retrieveInterval(), timer.retrieveIsRunning(),
                    timer.retrieveRepeat(), timer.retrieveTimeLeft(), timer.retrieveTitle(), lang.length() > 0 ? timer.retrieveObjectDescription(lang) : ""));

        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to retrieve Timer", tse);
        }
    }

    /**
     * Set timer
     *
     * @param key
     * @param objectPath
     * @param interval
     * @param repeat
     * @param title
     */
    public void setTimer(String key, String objectPath, String interval, String repeat, String title) {

        Timer timer = findTimer(key, objectPath);
        if (timer == null) {

            Log.w(TAG, "Timer not found");
            return;
        }

        if (interval != null) {

            String[] parts = interval.split(":");
            Period period = new Period(Integer.parseInt(parts[0]), Byte.parseByte(parts[1]), Byte.parseByte(parts[2]), (short) 0);

            try {

                timer.setInterval(period);
            } catch (TimeServiceException tse) {

                Log.e(TAG, "Failed to call 'setInterval'", tse);
            }
        }

        if (repeat != null) {

            try {
                short repeatSh = Short.parseShort(repeat);
                repeatSh = repeatSh == -1 ? org.allseen.timeservice.ajinterfaces.Timer.REPEAT_FOREVER : repeatSh;

                timer.setRepeat(repeatSh);
            } catch (Exception e) {

                Log.e(TAG, "Failed to call 'setRepeat'", e);
            }
        }

        if (title != null) {

            try {

                timer.setTitle(title);
            } catch (TimeServiceException tse) {

                Log.e(TAG, "Failed to call 'setTitle'", tse);
            }
        }
    }

    /**
     * Register Timer signal handler
     *
     * @param key
     * @param objectPath
     */
    public void regTimerHandler(String key, String objectPath) {

        Timer timer = findTimer(key, objectPath);
        if (timer == null) {

            Log.w(TAG, "Timer not found");
            return;
        }

        timer.registerTimerHandler(new TimerHandler() {
            @Override
            public void handleTimerEvent(Timer timer) {

                Log.i(TAG, "Received 'TimerEvent' signal from the timer: '" + timer.getObjectPath() + "'");
            }

            @Override
            public void handleRunStateChanged(Timer timer, boolean isRunning) {

                Log.i(TAG, "Received 'RunStateChanged' signal from the timer: '" + timer.getObjectPath() + "', IsRunning: '" + isRunning + "'");
            }
        });
    }

    /**
     * Unregister Timer signal handler
     *
     * @param key
     * @param objectPath
     */
    public void unRegTimerHandler(String key, String objectPath) {

        Timer timer = findTimer(key, objectPath);
        if (timer == null) {

            Log.w(TAG, "Timer not found");
            return;
        }

        timer.unregisterTimerHandler();
    }

    /**
     * Start the Timer
     *
     * @param key
     * @param objectPath
     */
    public void timerStart(String key, String objectPath) {

        Timer timer = findTimer(key, objectPath);
        if (timer == null) {

            Log.w(TAG, "Timer not found");
            return;
        }

        try {

            timer.start();
        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to call Timer start", tse);
        }
    }

    /**
     * Pause the Timer
     *
     * @param key
     * @param objectPath
     */
    public void timerPause(String key, String objectPath) {

        Timer timer = findTimer(key, objectPath);
        if (timer == null) {

            Log.w(TAG, "Timer not found");
            return;
        }

        try {

            timer.pause();
        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to call Timer pause", tse);
        }
    }

    /**
     * Reset the Timer
     *
     * @param key
     * @param objectPath
     */
    public void timerReset(String key, String objectPath) {

        Timer timer = findTimer(key, objectPath);
        if (timer == null) {

            Log.w(TAG, "Timer not found");
            return;
        }

        try {

            timer.reset();
        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to call Timer reset", tse);
        }
    }

    /**
     * Find Timer Factory
     *
     * @param key
     * @param objectPath
     * @return {@link Alarm}
     */
    public TimerFactory findTimerFactory(String key, String objectPath) {

        TimeServiceClient tsc = timeClients.get(key);
        if (tsc == null) {

            return null;
        }

        List<TimerFactory> timerFactories = tsc.getAnnouncedTimerFactoryList();
        for (TimerFactory factory : timerFactories) {

            if (factory.getObjectPath().equals(objectPath)) {

                return factory;
            }
        }

        return null;
    }

    /**
     * Create new {@link Timer} with the {@link TimerFactory}
     *
     * @param key
     *            device
     * @param objectPath
     *            {@link TimerFactory} object path
     */
    public void newTimer(String key, String objectPath) {

        TimerFactory factory = findTimerFactory(key, objectPath);

        if (factory == null) {

            Log.w(TAG, "Timer Factory not found");
            return;
        }

        try {

            Timer timer = factory.newTimer();
            createdTimers.put(timer.getObjectPath(), timer);
        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to create new Timer", tse);
        }
    }

    /**
     * Delete {@link Timer} of the {@link TimerFactory}
     *
     * @param key
     *            device
     * @param objectPath
     *            {@link TimerFactory} object path
     * @param timerToDeleteObjPath
     *            Object path of the {@link Alarm} to be deleted
     */
    public void deleteTimer(String key, String objectPath, String timerToDeleteObjPath) {

        TimerFactory factory = findTimerFactory(key, objectPath);

        Timer deletedTimer = createdTimers.get(timerToDeleteObjPath);
        if (deletedTimer == null) {

            Log.w(TAG, "Timer to be deleted is not found, objPath: '" + timerToDeleteObjPath + "'");
            return;
        }

        if (factory == null) {

            Log.w(TAG, "Timer Factory not found");
            return;
        }

        try {

            factory.deleteTimer(timerToDeleteObjPath);
            deletedTimer.release();
        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to delete Timer", tse);
        }
    }

    /**
     * Retrieve {@link Timer} objects of the given {@link TimerFactory}
     *
     * @param key
     *            device
     * @param objectPath
     *            {@link TimerFactory}
     */
    public void getTimers(String key, String objectPath) {

        TimerFactory factory = findTimerFactory(key, objectPath);

        if (factory == null) {

            Log.w(TAG, "Timer Factory not found");
            return;
        }

        List<Timer> timers;
        try {

            timers = factory.retrieveTimerList();
        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to retrieve List Of Timers", tse);
            return;
        }

        createdTimers.clear();
        for (Timer timer : timers) {

            createdTimers.put(timer.getObjectPath(), timer);
        }
    }

    /**
     * @return Current Time object
     */
    private org.allseen.timeservice.Time getTime() {

        Time t = new Time(Time.getCurrentTimezone());
        t.setToNow();

        byte seconds = (byte) ((t.second > 59) ? 59 : t.second);

        return new org.allseen.timeservice.Time((byte) t.hour, (byte) t.minute, seconds, (short) 0);
    }

    /**
     * Release {@link Alarm}s created by an {@link AlarmFactory}
     */
    private void releaseCreatedAlarms() {

        for (Alarm alarm : createdAlarms.values()) {

            alarm.release();
        }
        createdAlarms.clear();
    }

    /**
     * Release {@link Timer}s created by an {@link TimerFactory}
     */
    private void releaseCreatedTimers() {

        for (Timer timer : createdTimers.values()) {

            timer.release();
        }
        createdTimers.clear();
    }

    /**
     * Returns
     */
    private String getKey(String deviceId, UUID appId) {

        return deviceId + "_" + appId;
    }
}
