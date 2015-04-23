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

import java.util.HashMap;
import java.util.Map;

import org.alljoyn.bus.AboutObj;
import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.Mutable.ShortValue;
import org.alljoyn.bus.SessionOpts;
import org.alljoyn.bus.SessionPortListener;
import org.alljoyn.bus.Status;
import org.alljoyn.services.android.storage.PropertyStoreImpl;
import org.allseen.timeservice.AuthorityType;
import org.allseen.timeservice.TimeServiceException;
import org.allseen.timeservice.server.Alarm;
import org.allseen.timeservice.server.AlarmFactory;
import org.allseen.timeservice.server.Clock;
import org.allseen.timeservice.server.TimeAuthorityClock;
import org.allseen.timeservice.server.TimeServiceServer;
import org.allseen.timeservice.server.Timer;
import org.allseen.timeservice.server.TimerFactory;
import org.allseen.timeservice.test.TestServerCustomAlarm.TestServerCustomAlarmBusObj;

import android.util.Log;

/**
 * Test application server side
 */
public class TestServer {
    private static final String TAG = "ajtsapp" + TestServer.class.getSimpleName();

    /**
     * Test application
     */
    private final TimeServiceTestApp app;

    /**
     * Clocks objPath => Clock
     */
    private final Map<String, Clock> clocks;

    /**
     * Alarms objPath => Alarm
     */
    private final Map<String, Alarm> alarms;

    /**
     * Alarm Factories objPath => Alarm Factory
     */
    private final Map<String, AlarmFactory> alarmFactories;

    /**
     * Timer objPath => Timer
     */
    private final Map<String, Timer> timers;

    /**
     * Timer Factories objPath => Timer Factory
     */
    private final Map<String, TimerFactory> timerFactories;

    /**
     * About Object which is used to send Announcements
     */
    private AboutObj aboutObj;

    /**
     * Port number which is sent in the Announcement signal
     */
    public static final short ANNOUNCED_PORT = 1080;

    /**
     * Constructor
     *
     * @param app
     *            Test application throws {@link Exception}
     */
    public TestServer(TimeServiceTestApp app) {

        this.app       = app;
        clocks         = new HashMap<String, Clock>();
        alarms         = new HashMap<String, Alarm>();
        alarmFactories = new HashMap<String, AlarmFactory>();
        timers         = new HashMap<String, Timer>();
        timerFactories = new HashMap<String, TimerFactory>();
    }

    /**
     * Init {@link TimeServiceServer}
     *
     * @throws Exception
     */
    public void init() throws Exception {

        BusAttachment bus = app.getBusAttachment();

        SessionOpts sessionOpts  = new SessionOpts();
        sessionOpts.traffic      = SessionOpts.TRAFFIC_MESSAGES;
        sessionOpts.isMultipoint = false;
        sessionOpts.proximity    = SessionOpts.PROXIMITY_ANY;
        sessionOpts.transports   = SessionOpts.TRANSPORT_ANY;

        Status status = bus.bindSessionPort(new ShortValue(ANNOUNCED_PORT), sessionOpts, new SessionPortListener() {

            @Override
            public boolean acceptSessionJoiner(short sessionPort, String joiner, SessionOpts opts) {

                return sessionPort == ANNOUNCED_PORT;
            }
        });

        if ( status != Status.OK ) {

            throw new Exception("Failed to bind ANNOUNCED_PORT, Status: '" + status + "'");
        }

        aboutObj = new AboutObj(bus);
        TimeServiceServer.getInstance().init(bus);
    }

    /**
     * Shutdown
     */
    public void shutdown() {

        for (Clock clock : clocks.values()) {

            clock.release();
        }
        clocks.clear();

        for (Alarm alarm : alarms.values()) {

            alarm.release();
        }
        alarms.clear();

        for (AlarmFactory alarmFactory : alarmFactories.values()) {

            alarmFactory.release();
        }
        alarmFactories.clear();

        for (Timer timer : timers.values()) {

            timer.release();
        }
        timers.clear();

        for (TimerFactory timerFactory : timerFactories.values()) {

            timerFactory.release();
        }
        timerFactories.clear();

        TimeServiceServer.getInstance().shutdown();

        aboutObj.unannounce();
        app.getBusAttachment().unbindSessionPort(ANNOUNCED_PORT);
    }

    /**
     * Send Announcement signal
     */
    public void announceServer() {

        Log.d(TAG, "Sending Announcement");
        aboutObj.announce(ANNOUNCED_PORT, new PropertyStoreImpl(app));
    }

    /**
     * Create a {@link Clock}
     */
    public void createClock() {

        Clock clock = new TestServerClock();
        try {

            TimeServiceServer.getInstance().createClock(clock);
            Log.d(TAG, "Created a Clock, objPath: '" + clock.getObjectPath() + "'");

            clocks.put(clock.getObjectPath(), clock);
        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to create a clock", tse);
        }
    }

    /**
     * Create a {@link TimeAuthorityClock}
     */
    public void createTimeAuthorityClock(AuthorityType type) {

        TestServerAuthorityClock clock = new TestServerAuthorityClock(new TestServerClock());
        try {

            TimeServiceServer.getInstance().createTimeAuthorityClock(clock, type);
            Log.d(TAG, "Created a Time Authority Clock, objPath: '" + clock.getObjectPath() + "'");

            clocks.put(clock.getObjectPath(), clock);
        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to create a Time Authority clock", tse);
        }
    }

    /**
     * Sends {@link TimeAuthorityClock#timeSync()} signal if the given object is
     * a {@link TimeAuthorityClock}
     *
     * @param objPath
     *            {@link TimeAuthorityClock} object
     */
    public void sendTimeSync(String objPath) {

        Clock clock = clocks.get(objPath);
        if (!(clock instanceof TestServerAuthorityClock)) {

            Log.w(TAG, "The objectPath: '" + objPath + "' isn't a TimeAuthorityClock");
            return;
        }

        try {

            ((TestServerAuthorityClock) clock).timeSync();
        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to send TimeSync signal", tse);
        }
    }

    /**
     * Show all created clocks object paths
     */
    public void showClocks() {

        for (Clock clock : clocks.values()) {

            Log.v(TAG, "Clock objPath: '" + clock.getObjectPath() + "'");
        }
    }

    // ==============================================//

    /**
     * Creates basic Alarm
     */
    public void createAlarm(String desc) {

        Alarm alarm = new TestServerAlarm();
        try {

            if (desc == null) {
                TimeServiceServer.getInstance().createAlarm(alarm);
            } else {
                TimeServiceServer.getInstance().createAlarm(alarm, desc, "en", null);
            }

            Log.d(TAG, "Created Alarm, objPath: '" + alarm.getObjectPath() + "', desc: '" + desc + "'");

            alarms.put(alarm.getObjectPath(), alarm);
        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to create an Alarm", tse);
        }
    }

    /**
     * Create Custom Alarm
     */
    public void createCustomAlarm(String desc) {

        Alarm alarm = new TestServerCustomAlarm();
        try {

            if (desc == null) {

                TimeServiceServer.getInstance().registerCustomAlarm(new TestServerCustomAlarmBusObj(), alarm, null);
            } else {
                TimeServiceServer.getInstance().registerCustomAlarm(new TestServerCustomAlarmBusObj(), alarm, null, desc, "en", null);
            }

            Log.d(TAG, "Created a Custom Alarm, objPath: '" + alarm.getObjectPath() + "', desc: '" + desc + "'");

            alarms.put(alarm.getObjectPath(), alarm);
        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to create Custom Alarm", tse);
        }
    }

    /**
     * Show all created alarm object paths
     */
    public void showAlarms() {

        for (Alarm alarm : alarms.values()) {

            Log.v(TAG, "Alarm objPath: '" + alarm.getObjectPath() + "'");
        }
    }

    /**
     * Show all created alarm object paths
     */
    public void alarmReached(String objectPath) {

        Alarm alarm = findAlarm(objectPath);
        if (alarm == null) {

            Log.e(TAG, "The objectPath: '" + objectPath + "' is not an Alarm");
            return;
        }

        try {
            alarm.alarmReached();
        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to send 'AlarmReached' signal", tse);
        }
    }

    /**
     * Search for the {@link Alarm} in the alarms map or in the
     * {@link AlarmFactory}
     *
     * @return {@link Alarm}
     */
    private Alarm findAlarm(String objectPath) {

        if (objectPath.startsWith("/AlarmFactory")) { // AlarmFactory

            // Extract factory object path from the given object path.
            // Assumption that Alarm object path created from the factory has a
            // structure of:
            // [/FACTORY_OP][/ALARM_OP]
            String factoryOp = objectPath.substring(0, objectPath.indexOf('/', 1));
            TestServerAlarmFactory factory = (TestServerAlarmFactory) alarmFactories.get(factoryOp);

            return factory.findAlarm(objectPath);
        }

        return alarms.get(objectPath);
    }

    // ==============================================//

    /**
     * Create Alarm Factory
     */
    public void createAlarmFactory(String desc) {

        AlarmFactory alarmFactory = new TestServerAlarmFactory();
        try {

            if (desc == null) {

                TimeServiceServer.getInstance().createAlarmFactory(alarmFactory);
            } else {

                TimeServiceServer.getInstance().createAlarmFactory(alarmFactory, desc, "en", null);
            }

            Log.d(TAG, "Created AlarmFactory, objPath: '" + alarmFactory.getObjectPath() + "' with desc: '" + desc + "'");

            alarmFactories.put(alarmFactory.getObjectPath(), alarmFactory);

        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to create an AlarmFactory", tse);
        }
    }

    /**
     * Show all created Alarm Factories
     */
    public void showAlarmFactories() {

        for (AlarmFactory alarmFactory : alarmFactories.values()) {

            Log.v(TAG, "AlarmFactory objPath: '" + alarmFactory.getObjectPath() + "'");
        }
    }

    // ==============================================//

    /**
     * Create Timer Factory
     */
    public void createTimerFactory(String desc) {

        TimerFactory timerFactory = new TestServerTimerFactory();
        try {

            if (desc == null) {

                TimeServiceServer.getInstance().createTimerFactory(timerFactory);
            } else {

                TimeServiceServer.getInstance().createTimerFactory(timerFactory, desc, "en", null);
            }

            Log.d(TAG, "Created TimerFactory, objPath: '" + timerFactory.getObjectPath() + "', desc: '" + desc + "'");

            timerFactories.put(timerFactory.getObjectPath(), timerFactory);

        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to create the TimerFactory", tse);
        }
    }

    /**
     * Show all created Timer Factories
     */
    public void showTimerFactories() {

        for (TimerFactory timerFactory : timerFactories.values()) {

            Log.v(TAG, "TimerFactory objPath: '" + timerFactory.getObjectPath() + "'");
        }
    }

    // ==============================================//

    /**
     * Create {@link Timer}
     */
    public void createTimer(String desc) {

        Timer timer = new TestServerTimer();
        try {

            if (desc == null) {

                TimeServiceServer.getInstance().createTimer(timer);
            } else {
                TimeServiceServer.getInstance().createTimer(timer, desc, "en", null);
            }

            Log.d(TAG, "Created Timer, objPath: '" + timer.getObjectPath() + "', desc: '" + desc + "'");

            timers.put(timer.getObjectPath(), timer);
        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to created the Timer", tse);
        }
    }

    /**
     * Show all created timers object paths
     */
    public void showTimers() {

        for (Timer timer : timers.values()) {

            Log.v(TAG, "Timer objPath: '" + timer.getObjectPath() + "'");
        }
    }
}
