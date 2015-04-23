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

package org.allseen.timeservice.server;

import java.util.Collections;
import java.util.HashSet;
import java.util.Set;

import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.BusObject;
import org.alljoyn.bus.Mutable.ShortValue;
import org.alljoyn.bus.SessionListener;
import org.alljoyn.bus.SessionOpts;
import org.alljoyn.bus.SessionPortListener;
import org.alljoyn.bus.Status;
import org.alljoyn.bus.Translator;
import org.alljoyn.bus.annotation.BusInterface;
import org.alljoyn.bus.ifaces.AllSeenIntrospectable;
import org.allseen.timeservice.AuthorityType;
import org.allseen.timeservice.TimeServiceConst;
import org.allseen.timeservice.TimeServiceException;
import org.allseen.timeservice.client.TimeServiceClient;

import android.util.Log;

/**
 * The main class to initialize the Time Service in the server mode. Initialize
 * the class by calling the {@link TimeServiceServer#init(BusAttachment)}
 * method.
 */
public class TimeServiceServer {

    private static final String TAG = "ajts" + TimeServiceServer.class.getSimpleName();

    /**
     * Listens for the Session Related events
     */
    private static class SessionListenerServer extends SessionListener {

        /**
         * @see org.alljoyn.bus.SessionListener#sessionLost(int, int)
         */
        @Override
        public void sessionLost(int sessionId, int reason) {

            TimeServiceServer tsServer = TimeServiceServer.getInstance();
            Integer sid = tsServer.getSessionId();

            BusAttachment bus = tsServer.getBusAttachment();
            bus.enableConcurrentCallbacks();

            Log.d(TAG, "Received sessionLost, sessionId: '" + sessionId + "', reason: '" + reason + "'");

            if (sessionId != sid) {

                Log.w(TAG, "Received sessionLost for the unknown session, TimeServiceServer's sessionId is: '" + sid + "'");
                return;
            }

            Log.d(TAG, "Resetting session id in the TimeServiceServer and removing SessionListener");
            bus.setSessionListener(sessionId, null);

            tsServer.setSessionId(null);
        }
    }

    // ==========================================================//

    /**
     * Listens for the Session Join events
     */
    private static class SessionPortListenerServer extends SessionPortListener {

        /**
         * @see org.alljoyn.bus.SessionPortListener#acceptSessionJoiner(short,
         *      java.lang.String, org.alljoyn.bus.SessionOpts)
         */
        @Override
        public boolean acceptSessionJoiner(short sessionPort, String joiner, SessionOpts opts) {

            TimeServiceServer.getInstance().getBusAttachment().enableConcurrentCallbacks();

            if (sessionPort != TimeServiceConst.PORT_NUM) {

                Log.w(TAG, "Received JoinSession request for an unknown port: '" + sessionPort + "', from : '" + joiner + "'");
                return false;
            }

            Log.d(TAG, "Received JoinSession request, from : '" + joiner + "', returning TRUE");
            return true;
        }

        /**
         * @see org.alljoyn.bus.SessionPortListener#sessionJoined(short, int,
         *      java.lang.String)
         */
        @Override
        public void sessionJoined(short sessionPort, int id, String joiner) {

            TimeServiceServer tsServer = TimeServiceServer.getInstance();
            BusAttachment bus = tsServer.getBusAttachment();
            bus.enableConcurrentCallbacks();

            Log.d(TAG, "The joiner: '" + joiner + "' has joined the session: '" + id + "'");

            // Set session listener to received SessionLost event
            Status status = bus.setSessionListener(id, new SessionListenerServer());

            if (status != Status.OK) {

                Log.e(TAG, "Failed to set SessionListener for the session, sid: '" + id + "', Status: '" + status + "'");
            }

            tsServer.setSessionId(id);
        }
    }

    // ==========================================================//

    /**
     * Self reference for the {@link TimeServiceServer} singleton
     */
    private static final TimeServiceServer SELF = new TimeServiceServer();

    /**
     * {@link BusAttachment} that is used by the {@link TimeServiceServer}
     */
    private BusAttachment bus;

    /**
     * Sessions connected to the {@link TimeServiceServer}
     */
    private Integer sid;

    /**
     * Constructor
     */
    private TimeServiceServer() {
    }

    /**
     * Return {@link TimeServiceServer}
     * 
     * @return {@link TimeServiceServer}
     */
    public static TimeServiceServer getInstance() {

        return SELF;
    }

    /**
     * Initialize {@link TimeServiceServer}.
     * 
     * @param busAttachment
     *            {@link BusAttachment} to be used by the
     *            {@link TimeServiceServer}.
     * @throws TimeServiceException
     *             Is thrown for the following reasons: <br>
     *             - Service has been initialized previously <br>
     *             - The given {@link BusAttachment} is not initialized or not
     *             connected to the Router <br>
     *             - {@link AboutService} hasn't started <br>
     *             - Failed to bind session port to the {@link BusAttachment}
     * @see BusAttachment#bindSessionPort(org.alljoyn.bus.Mutable.ShortValue,
     *      SessionOpts, SessionPortListener)
     * @see AboutService#startAboutServer(short,
     *      org.alljoyn.services.common.PropertyStore, BusAttachment)
     */
    public void init(BusAttachment busAttachment) throws TimeServiceException {

        if (bus != null) {

            throw new TimeServiceException("TimeServiceService has been initialized previously");
        }

        checkBusValidity(busAttachment);
        bus = busAttachment;

        Status status = bus.bindSessionPort(new ShortValue(TimeServiceConst.PORT_NUM), getSessionOpts(), new SessionPortListenerServer());

        if (status != Status.OK) {

            throw new TimeServiceException("Failed to bind session port, Status: '" + status + "'");
        }

        // This is used to create object paths for the service objects,
        // so needs to be initialized
        GlobalStringSequencer.init();
    }

    /**
     * Stop the {@link TimeServiceServer}.
     */
    public void shutdown() {

        if (bus == null) {

            Log.w(TAG, "Can't shutdown the server, it hasn't been initialized");
            return;
        }

        Log.i(TAG, "Shutting down TimeServiceServer");

        Log.d(TAG, "Unbinding session port");
        Status status = bus.unbindSessionPort(TimeServiceConst.PORT_NUM);
        if (status != Status.OK) {

            Log.e(TAG, "Failed to unbind SessionPort, status: '" + status + "'");
        }

        if (sid != null) {

            Log.d(TAG, "Removing SessionListener and closing the session: '" + sid + "'");
            bus.setSessionListener(sid, null);

            status = bus.leaveSession(sid);
            if (status == Status.OK) {

                sid = null;
            } else {

                Log.e(TAG, "Failed to leave the session, status: '" + status + "'");
            }
        }

        bus = null;
    }

    /**
     * {@link BusAttachment} that is used by the {@link TimeServiceServer}
     * 
     * @return {@link BusAttachment}
     */
    public BusAttachment getBusAttachment() {

        return bus;
    }

    /**
     * {@link TimeServiceServer} is started if the method
     * {@link TimeServiceServer#init(BusAttachment)} has been invoked
     * 
     * @return TRUE if the {@link TimeServiceServer} has been started
     */
    public boolean isStarted() {

        return (bus != null);
    }

    /**
     * Create Clock.
     * 
     * @param clock
     *            Methods of this {@link Clock} are invoked as a response to
     *            calls of the remote {@link TimeServiceClient}s.
     * @throws TimeServiceException
     *             Is thrown if failed to create the {@link Clock}
     */
    public void createClock(Clock clock) throws TimeServiceException {

        if (clock == null) {

            throw new TimeServiceException("Clock is undefined");
        }

        ClockBusObj clockBusObj = new ClockBusObj(clock);
        clock.setClockBusObj(clockBusObj);
    }

    /**
     * Create Time Authority Clock. <br>
     * The clock allows to send {@link TimeAuthorityClock#timeSync()} signal to
     * suggest to synchronize time with it.
     * 
     * @param clock
     *            Methods of this {@link TimeAuthorityClock} are invoked as a
     *            response to calls of the remote {@link TimeServiceClient}s.
     * @param type
     *            Options where this {@link TimeAuthorityClock} synchronizes its
     *            time
     * @throws TimeServiceException
     *             Is thrown if failed to create the {@link TimeAuthorityClock}
     */
    public void createTimeAuthorityClock(TimeAuthorityClock clock, AuthorityType type) throws TimeServiceException {

        if (clock == null) {

            throw new TimeServiceException("Clock is undefined");
        }

        if (type == null) {

            throw new TimeServiceException("AuthorityType is undefined");
        }

        TimeAuthorityClockBusObj clockBusObj = new TimeAuthorityClockBusObj(type, clock);
        clock.setClockBusObj(clockBusObj);
    }

    /**
     * Create Alarm.
     * 
     * @param alarm
     *            Methods of this {@link Alarm} are invoked as a response to
     *            calls of the remote {@link TimeServiceClient}s
     * @throws TimeServiceException
     *             Is throws if failed to create the {@link Alarm}
     */
    public void createAlarm(Alarm alarm) throws TimeServiceException {

        if (alarm == null) {

            throw new TimeServiceException("Alarm is undefined");
        }

        AlarmBusObj alarmBusObj = new AlarmBusObj();
        alarmBusObj.init(alarm, "", true, null, null, null, null);
        alarm.setAlarmBusObj(alarmBusObj);
    }

    /**
     * Create Alarm with description.
     * 
     * @param alarm
     *            Methods of this {@link Alarm} are invoked as a response to
     *            calls of the remote {@link TimeServiceClient}s
     * @param description
     *            Description to be added to the object when registering
     *            {@link Alarm}. Since description is provided,
     *            {@link AllSeenIntrospectable} interface is added to the
     *            announced interfaces to support Events & Actions feature.
     * @param language
     *            The language of the description
     * @param translator
     *            If not NULL, {@link Alarm} is registered with
     *            {@link Translator} to support the feature of
     *            {@link AllSeenIntrospectable#GetDescriptionLanguages()}
     * @throws TimeServiceException
     *             Is throws if failed to create the {@link Alarm}
     */
    public void createAlarm(Alarm alarm, String description, String language, Translator translator) throws TimeServiceException {

        if (alarm == null) {

            throw new TimeServiceException("Alarm is undefined");
        }

        if (description == null || description.length() == 0 || language == null || language.length() == 0) {

            throw new TimeServiceException("description or language are undefined");
        }

        AlarmBusObj alarmBusObj = new AlarmBusObj();
        alarmBusObj.init(alarm, "", true, null, description, language, translator);
        alarm.setAlarmBusObj(alarmBusObj);
    }

    /**
     * Register user defined custom Alarm. <br>
     * Custom Alarm should extend {@link BaseAlarmBusObj} and implement
     * {@link org.allseen.timeservice.ajinterfaces.Alarm} interface. <br>
     * This method registers the given alarmBusObj and searches for the
     * interfaces to be added to {@link AboutService} and later announced. The
     * announced interfaces are searched among the interfaces directly
     * implemented by the alarmBusObj. To exclude interfaces from being
     * announced, notAnnouncedInterfaces array should be provided.
     * 
     * @param alarmBusObj
     *            Custom Alarm to be registered
     * @param alarm
     *            Methods of this {@link Alarm} are invoked as a response to
     *            calls of the remote {@link TimeServiceClient}s
     * @param notAnnouncedInterfaces
     *            AllJoyn name of the interfaces that should NOT be announced.
     *            If this array is NULL, all the found interfaces will be added
     *            to the {@link AboutService} to be announced.
     * @throws TimeServiceException
     *             Is throws if failed to create the {@link Alarm}
     */
    public void registerCustomAlarm(BaseAlarmBusObj alarmBusObj, Alarm alarm, String[] notAnnouncedInterfaces) throws TimeServiceException {

        if (alarmBusObj == null) {

            throw new TimeServiceException("alarmBusObj is undefined");
        }

        if (!(alarmBusObj instanceof org.allseen.timeservice.ajinterfaces.Alarm)) {

            throw new TimeServiceException("Provided AlarmBusObject does not implement AllJoyn Alarm interface");
        }

        if (alarm == null) {

            throw new TimeServiceException("alarm is undefined");
        }

        alarmBusObj.init(alarm, "", true, notAnnouncedInterfaces, null, null, null);
        alarm.setAlarmBusObj(alarmBusObj);
    }

    /**
     * Register user defined custom Alarm with description.<br>
     * Custom Alarm should extend {@link BaseAlarmBusObj} and implement
     * {@link org.allseen.timeservice.ajinterfaces.Alarm} interface. <br>
     * This method registers the given alarmBusObj and searches for the
     * interfaces to be added to {@link AboutService} and later announced. The
     * announced interfaces are searched among the interfaces directly
     * implemented by the alarmBusObj. To exclude interfaces from being
     * announced, notAnnouncedInterfaces array should be provided.
     * 
     * @param alarmBusObj
     *            Custom Alarm to be registered
     * @param alarm
     *            Methods of this {@link Alarm} are invoked as a response to
     *            calls of the remote {@link TimeServiceClient}s
     * @param notAnnouncedInterfaces
     *            AllJoyn name of the interfaces that should NOT be announced.
     *            If this array is NULL, all the found interfaces will be added
     *            to the {@link AboutService} to be announced.
     * @param description
     *            Description to be added to the object when registering
     *            {@link BaseAlarmBusObj}. Since description is provided,
     *            {@link AllSeenIntrospectable} interface is added to the
     *            announced interfaces to support Events & Actions feature.
     * @param language
     *            The language of the description
     * @param translator
     *            If not NULL, {@link BaseAlarmBusObj} is registered with
     *            {@link Translator} to support the feature of
     *            {@link AllSeenIntrospectable#GetDescriptionLanguages()}
     * @throws TimeServiceException
     *             Is throws if failed to create the {@link Alarm}
     */
    public void registerCustomAlarm(BaseAlarmBusObj alarmBusObj, Alarm alarm, String[] notAnnouncedInterfaces, String description, String language, Translator translator) throws TimeServiceException {

        if (alarmBusObj == null) {

            throw new TimeServiceException("alarmBusObj is undefined");
        }

        if (!(alarmBusObj instanceof org.allseen.timeservice.ajinterfaces.Alarm)) {

            throw new TimeServiceException("Provided AlarmBusObject does not implement AllJoyn Alarm interface");
        }

        if (alarm == null) {

            throw new TimeServiceException("alarm is undefined");
        }

        if (description == null || description.length() == 0 || language == null || language.length() == 0) {

            throw new TimeServiceException("description or language are undefined");
        }

        alarmBusObj.init(alarm, "", true, notAnnouncedInterfaces, description, language, translator);
        alarm.setAlarmBusObj(alarmBusObj);
    }

    /**
     * Create Alarm Factory.<br>
     * Alarm Factory allows creating and deleting {@link Alarm} objects.
     * 
     * @param alarmFactory
     *            Methods of this {@link AlarmFactory} are invoked as a response
     *            to calls of the remote {@link TimeServiceClient}s.
     * @throws TimeServiceException
     *             Is thrown if failed to create the {@link AlarmFactory}
     */
    public void createAlarmFactory(AlarmFactory alarmFactory) throws TimeServiceException {

        if (alarmFactory == null) {

            throw new TimeServiceException("AlarmFactory is undefined");
        }

        AlarmFactoryBusObj alarmFactoryBusObj = new AlarmFactoryBusObj(alarmFactory);
        alarmFactory.setAlarmBusObj(alarmFactoryBusObj);
    }

    /**
     * Create Alarm Factory with description.<br>
     * Alarm Factory allows creating and deleting {@link Alarm} objects.
     * 
     * @param alarmFactory
     *            Methods of this {@link AlarmFactory} are invoked as a response
     *            to calls of the remote {@link TimeServiceClient}s.
     * @param description
     *            Description to be added to the created {@link Alarm}. Since
     *            description is provided, {@link AllSeenIntrospectable}
     *            interface is added to the announced interfaces to support
     *            Events & Actions feature.
     * @param language
     *            The language of the description
     * @param translator
     *            If not NULL, {@link Alarm} is registered with
     *            {@link Translator} to support the feature of
     *            {@link AllSeenIntrospectable#GetDescriptionLanguages()}
     * @throws TimeServiceException
     *             Is throws if failed to create the {@link AlarmFactory}
     */
    public void createAlarmFactory(AlarmFactory alarmFactory, String description, String language, Translator translator) throws TimeServiceException {

        if (alarmFactory == null) {

            throw new TimeServiceException("AlarmFactory is undefined");
        }

        if (description == null || description.length() == 0 || language == null || language.length() == 0) {

            throw new TimeServiceException("description or language are undefined");
        }

        AlarmFactoryBusObj alarmFactoryBusObj = new AlarmFactoryBusObj(alarmFactory, description, language, translator);
        alarmFactory.setAlarmBusObj(alarmFactoryBusObj);
    }

    /**
     * Create Timer.
     * 
     * @param timer
     *            Methods of this {@link Timer} are invoked as a response to
     *            calls of the remote {@link TimeServiceClient}s
     * @throws TimeServiceException
     *             Is throws if failed to create the {@link Timer}
     */
    public void createTimer(Timer timer) throws TimeServiceException {

        if (timer == null) {

            throw new TimeServiceException("Timer is undefined");
        }

        TimerBusObj timerBusObj = new TimerBusObj();
        timerBusObj.init(timer, "", true, null, null, null, null);
        timer.setTimerBusObj(timerBusObj);
    }

    /**
     * Create Timer with description.
     * 
     * @param timer
     *            Methods of this {@link Timer} are invoked as a response to
     *            calls of the remote {@link TimeServiceClient}s
     * @param description
     *            Description to be added to the object when registering
     *            {@link Timer}. Since description is provided,
     *            {@link AllSeenIntrospectable} interface is added to the
     *            announced interfaces to support Events & Actions feature.
     * @param language
     *            The language of the description
     * @param translator
     *            If not NULL, {@link Timer} is registered with
     *            {@link Translator} to support the feature of
     *            {@link AllSeenIntrospectable#GetDescriptionLanguages()}
     * @throws TimeServiceException
     *             Is throws if failed to create the {@link Timer}
     */
    public void createTimer(Timer timer, String description, String language, Translator translator) throws TimeServiceException {

        if (timer == null) {

            throw new TimeServiceException("Timer is undefined");
        }

        if (description == null || description.length() == 0 || language == null || language.length() == 0) {

            throw new TimeServiceException("description or language are undefined");
        }

        TimerBusObj timerBusObj = new TimerBusObj();
        timerBusObj.init(timer, "", true, null, description, language, translator);
        timer.setTimerBusObj(timerBusObj);
    }

    /**
     * Register user defined custom Timer. <br>
     * Custom Timer should extend {@link BaseTimerBusObj} and implement
     * {@link org.allseen.timeservice.ajinterfaces.Timer} interface. <br>
     * This method registers the given timerBusObj and searches for the
     * interfaces to be added to {@link AboutService} and later announced. The
     * announced interfaces are searched among the interfaces directly
     * implemented by the timerBusObj. To exclude interfaces from being
     * announced, notAnnouncedInterfaces array should be provided.
     * 
     * @param timerBusObj
     *            Custom Timer to be registered
     * @param timer
     *            Methods of this {@link Timer} are invoked as a response to
     *            calls of the remote {@link TimeServiceClient}s
     * @param notAnnouncedInterfaces
     *            AllJoyn name of the interfaces that should NOT be announced.
     *            If this array is NULL, all the found interfaces will be added
     *            to the {@link AboutService} to be announced.
     * @throws TimeServiceException
     *             Is throws if failed to create the {@link Timer}
     */
    public void registerCustomTimer(BaseTimerBusObj timerBusObj, Timer timer, String[] notAnnouncedInterfaces) throws TimeServiceException {

        if (timerBusObj == null) {

            throw new TimeServiceException("timerBusObj is undefined");
        }

        if (!(timerBusObj instanceof org.allseen.timeservice.ajinterfaces.Timer)) {

            throw new TimeServiceException("Provided TimerBusObject does not implement AllJoyn Timer interface");
        }

        if (timer == null) {

            throw new TimeServiceException("alarm is undefined");
        }

        timerBusObj.init(timer, "", true, notAnnouncedInterfaces, null, null, null);
        timer.setTimerBusObj(timerBusObj);
    }

    /**
     * Register user defined custom Timer with description.<br>
     * Custom Timer should extend {@link BaseTimerBusObj} and implement
     * {@link org.allseen.timeservice.ajinterfaces.Timer} interface. <br>
     * This method registers the given timerBusObj and searches for the
     * interfaces to be added to {@link AboutService} and later announced. The
     * announced interfaces are searched among the interfaces directly
     * implemented by the timerBusObj. To exclude interfaces from being
     * announced, notAnnouncedInterfaces array should be provided.
     * 
     * @param timerBusObj
     *            Custom Timer to be registered
     * @param timer
     *            Methods of this {@link Timer} are invoked as a response to
     *            calls of the remote {@link TimeServiceClient}s
     * @param notAnnouncedInterfaces
     *            AllJoyn name of the interfaces that should NOT be announced.
     *            If this array is NULL, all the found interfaces will be added
     *            to the {@link AboutService} to be announced.
     * @param description
     *            Description to be added to the object when registering
     *            {@link BaseTimerBusObj}. Since description is provided,
     *            {@link AllSeenIntrospectable} interface is added to the
     *            announced interfaces to support Events & Actions feature.
     * @param language
     *            The language of the description
     * @param translator
     *            If not NULL, {@link BaseTimerBusObj} is registered with
     *            {@link Translator} to support the feature of
     *            {@link AllSeenIntrospectable#GetDescriptionLanguages()}
     * @throws TimeServiceException
     *             Is throws if failed to create the {@link Timer}
     */
    public void registerCustomTimer(BaseTimerBusObj timerBusObj, Timer timer, String[] notAnnouncedInterfaces, String description, String language, Translator translator) throws TimeServiceException {

        if (timerBusObj == null) {

            throw new TimeServiceException("timerBusObj is undefined");
        }

        if (!(timerBusObj instanceof org.allseen.timeservice.ajinterfaces.Timer)) {

            throw new TimeServiceException("Provided TimerBusObject does not implement AllJoyn Timer interface");
        }

        if (timer == null) {

            throw new TimeServiceException("timer is undefined");
        }

        if (description == null || description.length() == 0 || language == null || language.length() == 0) {

            throw new TimeServiceException("description or language are undefined");
        }

        timerBusObj.init(timer, "", true, notAnnouncedInterfaces, description, language, translator);
        timer.setTimerBusObj(timerBusObj);
    }

    /**
     * Create Timer Factory.<br>
     * Timer Factory allows creating and deleting {@link Timer} objects.
     * 
     * @param timerFactory
     *            Methods of this {@link TimerFactory} are invoked as a response
     *            to calls of the remote {@link TimeServiceClient}s.
     * @throws TimeServiceException
     *             Is thrown if failed to create the {@link TimerFactory}
     */
    public void createTimerFactory(TimerFactory timerFactory) throws TimeServiceException {

        if (timerFactory == null) {

            throw new TimeServiceException("TimerFactory is undefined");
        }

        TimerFactoryBusObj timerFactoryBusObj = new TimerFactoryBusObj(timerFactory);
        timerFactory.setTimerBusObj(timerFactoryBusObj);
    }

    /**
     * Create Timer Factory with description.<br>
     * Timer Factory allows creating and deleting {@link Timer} objects.
     * 
     * @param timerFactory
     *            Methods of this {@link TimerFactory} are invoked as a response
     *            to calls of the remote {@link TimeServiceClient}s.
     * @param description
     *            Description to be added to the created {@link Timer}. Since
     *            description is provided, {@link AllSeenIntrospectable}
     *            interface is added to the announced interfaces to support
     *            Events & Actions feature.
     * @param language
     *            The language of the description
     * @param translator
     *            If not NULL, {@link Timer} will be created with
     *            {@link Translator} to support the feature of
     *            {@link AllSeenIntrospectable#GetDescriptionLanguages()}
     * @throws TimeServiceException
     *             Is thrown if failed to create the {@link TimerFactory}
     */
    public void createTimerFactory(TimerFactory timerFactory, String description, String language, Translator translator) throws TimeServiceException {

        if (timerFactory == null) {

            throw new TimeServiceException("TimerFactory is undefined");
        }

        if (description == null || description.length() == 0 || language == null || language.length() == 0) {

            throw new TimeServiceException("description or language are undefined");
        }

        TimerFactoryBusObj timerFactoryBusObj = new TimerFactoryBusObj(timerFactory, description, language, translator);
        timerFactory.setTimerBusObj(timerFactoryBusObj);
    }

    /**
     * Set received sid to the object
     * 
     * @param sid
     */
    synchronized void setSessionId(Integer sid) {

        this.sid = sid;
    }

    /**
     * The currently connected session. If the returned session id is NULL it
     * means no session is connected to the {@link TimeServiceServer}
     * 
     * @return session id
     */
    synchronized Integer getSessionId() {

        return sid;
    }

    /**
     * Register {@link BusObject}
     * 
     * @param toReg
     *            The {@link BusObject} to register
     * @param objectPath
     *            {@link BusObject} object path
     * @param description
     *            Events & Actions description
     * @param language
     *            Events & Actions language
     * @param translator
     *            {@link Translator}
     * @throws TimeServiceException
     *             Is thrown if failed to register {@link BusObject}
     */
    void registerBusObject(BusObject toReg, String objectPath, String description, String language, Translator translator) throws TimeServiceException {

        Status status;

        checkBusValidity(bus);

        if (description != null) {

            if (translator == null) {

                status = bus.registerBusObject(toReg, objectPath, false, language, description);

            } else {

                status = bus.registerBusObject(toReg, objectPath, false, language, description, translator);
            }
        } else {

            status = bus.registerBusObject(toReg, objectPath);
        }

        if (status != Status.OK) {

            throw new TimeServiceException("Failed to register BusObject: '" + objectPath + "', Status: '" + status + "'");
        }

        Log.i(TAG, "BusObject: '" + objectPath + "' registered successfully");
    }

    /**
     * Looks for the interfaces to be announced from the interfaces implemented
     * by the given object
     * 
     * @param obj
     *            Object to search for the announced interfaces
     * @param notAnnouncedArr
     *            The interfaces that should not be announced
     * @param addIntrospectable
     *            Whether {@link AllSeenIntrospectable} should be added to the
     *            Announced interfaces
     * @return Array of interface names that should be announced
     */
    String[] getAnnouncedInterfaces(Object obj, String[] notAnnouncedArr, boolean addIntrospectable) {

        Set<String> announced = new HashSet<String>();
        Set<String> notAnnounced = new HashSet<String>();

        if (notAnnouncedArr != null) {

            Collections.addAll(notAnnounced, notAnnouncedArr);
        }

        Class<?>[] ifaces = obj.getClass().getInterfaces();

        // Search for the @BusInterface(s)
        for (Class<?> iface : ifaces) {

            BusInterface busIface = iface.getAnnotation(BusInterface.class);

            // Not a BusInterface
            if (busIface == null) {
                continue;
            }

            announced.add(busIface.name());
        }

        if (addIntrospectable) {

            announced.add(AllSeenIntrospectable.class.getAnnotation(BusInterface.class).name());
        }

        announced.removeAll(notAnnounced);
        return announced.toArray(new String[] {});
    }

    /**
     * Checks validity of the received {@link BusAttachment}.
     * {@link BusAttachment} should be initialized and connected to the Routing
     * Node.
     * 
     * @param bus
     *            {@link BusAttachment} to test, should be initialized and
     *            connected.
     * @throws TimeServiceException
     *             if the {@link BusAttachment} hasn't initialized or connected
     */
    private void checkBusValidity(BusAttachment bus) throws TimeServiceException {

        if (bus == null) {

            throw new TimeServiceException("BusAttachment is not initialized");
        }

        if (!bus.isConnected()) {

            throw new TimeServiceException("BusAttachment is not connected");
        }
    }

    /**
     * Create and returns {@link SessionOpts} object
     * 
     * @return {@link SessionOpts}
     */
    private SessionOpts getSessionOpts() {

        SessionOpts sessionOpts = new SessionOpts();
        sessionOpts.traffic = SessionOpts.TRAFFIC_MESSAGES; // Use reliable
                                                            // message-based
                                                            // communication to
                                                            // move data between
                                                            // session endpoints
        sessionOpts.isMultipoint = true; // A session is multi-point if it can
                                         // be joined multiple times
        sessionOpts.proximity = SessionOpts.PROXIMITY_ANY; // Holds the
                                                           // proximity for this
                                                           // SessionOpt
        sessionOpts.transports = SessionOpts.TRANSPORT_ANY; // Holds the allowed
                                                            // transports for
                                                            // this SessionOpts

        return sessionOpts;
    }
}
