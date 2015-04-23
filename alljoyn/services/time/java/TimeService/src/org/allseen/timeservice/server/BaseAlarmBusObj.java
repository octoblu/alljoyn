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

import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.BusException;
import org.alljoyn.bus.BusObject;
import org.alljoyn.bus.ErrorReplyBusException;
import org.alljoyn.bus.SignalEmitter;
import org.alljoyn.bus.SignalEmitter.GlobalBroadcast;
import org.alljoyn.bus.Translator;
import org.allseen.timeservice.Schedule;
import org.allseen.timeservice.TimeServiceConst;
import org.allseen.timeservice.TimeServiceException;
import org.allseen.timeservice.ajinterfaces.Alarm;
import org.allseen.timeservice.ajinterfaces.AlarmFactory;
import org.allseen.timeservice.ajinterfaces.ScheduleAJ;

import android.util.Log;

/**
 * This is the base class for all the classes implementing {@link Alarm}
 * interface. Extend this class to handle AllJoyn communication with this
 * {@link Alarm}. Most of the {@link Alarm} related calls delegated to the
 * {@link org.allseen.timeservice.server.Alarm} object.
 */
// Commented out because currently JavaBinding doesn't support finding AJ
// interfaces in the SuperClasses
// so they must be implemented by the leaf classes
public abstract class BaseAlarmBusObj implements BusObject /* implements Alarm */{
    private static final String TAG = "ajts" + BaseAlarmBusObj.class.getSimpleName();

    /**
     * Alarm to be notified with the
     * {@link org.allseen.timeservice.server.Alarm} related calls
     */
    private org.allseen.timeservice.server.Alarm alarm;

    /**
     * Prefix of the Alarm object path
     */
    private static final String OBJ_PATH_PREFIX = "/Alarm";

    /**
     * Alarm reached TTL
     */
    private static final int ALARM_REACHED_TTL_SECONDS = 50;

    /**
     * The object path of this {@link BusObject}
     */
    private String objectPath;

    /**
     * Interfaces that to be announced
     */
    private String[] announcedInterfaces;

    /**
     * Initialize the object
     *
     * @param alarm
     *            {@link org.allseen.timeservice.server.Alarm} delegate
     * @param parentObj
     *            Parent object path if the object was created via
     *            {@link AlarmFactory}
     * @param needAnnounce
     *            If the object was created via {@link AlarmFactory} no need to
     *            Announce
     * @param notAnnounced
     *            Interfaces that shouldn't be announced
     * @param description
     *            Events&Actions description
     * @param language
     *            Events&Actions description language
     * @param translator
     *            Events&Actions {@link Translator}
     * @throws TimeServiceException
     *             Is thrown if failed to initialized the object
     */
    void init(org.allseen.timeservice.server.Alarm alarm, String parentObj, boolean needAnnounce, String[] notAnnounced, String description, String language, Translator translator)
            throws TimeServiceException {

        if (alarm == null) {

            throw new TimeServiceException("Undefined alarm");
        }

        if (!(this instanceof BusObject)) {

            throw new TimeServiceException("Class not a BusObject");
        }

        this.alarm = alarm;
        objectPath = parentObj + GlobalStringSequencer.append(OBJ_PATH_PREFIX);

        TimeServiceServer tsServer = TimeServiceServer.getInstance();
        tsServer.registerBusObject(this, objectPath, description, language, translator);

        // No need to send announcement if this Alarm object was created with
        // AlarmFactory
        if (needAnnounce) {

            announcedInterfaces = tsServer.getAnnouncedInterfaces(this, notAnnounced, true);

            for (String announcedInterface : announcedInterfaces) {

                try {
                    getBus().setAnnounceFlag(this, announcedInterface, true);
                    Log.i(TAG, "Added announced interface: '" + announcedInterface + "'");
                } catch (TimeServiceException tse) {
                    Log.e(TAG, "Failed to setAnnounceFlag to true, interface: '" + announcedInterface + "'", tse);
                    throw tse;
                }
            }
        }
    }

    /**
     * @see org.allseen.timeservice.ajinterfaces.Alarm#getVersion()
     */
    public short getVersion() throws BusException {

        return Alarm.VERSION;
    }

    /**
     * @see org.allseen.timeservice.ajinterfaces.Alarm#getSchedule()
     */
    public ScheduleAJ getSchedule() throws BusException {

        ScheduleAJ scheduleAJ;

        try {

            getBus().enableConcurrentCallbacks();

            Log.d(TAG, "getSchedule is called, handling, objPath: '" + objectPath + "'");

            Schedule schedule = getAlarm().getSchedule();

            if (schedule == null) {

                throw new TimeServiceException("Undefined schedule, objPath: '" + objectPath + "'");
            }

            scheduleAJ = new ScheduleAJ(schedule);
        } catch (Exception e) {

            Log.e(TAG, "Failed to execute 'getSchedule', objPath: '" + objectPath + "'", e);
            throw new ErrorReplyBusException(TimeServiceConst.GENERIC_ERROR, e.getMessage());
        }

        Log.d(TAG, "Returning schedule: '" + scheduleAJ + "', objPath: '" + objectPath + "'");
        return scheduleAJ;
    }

    /**
     * @see org.allseen.timeservice.ajinterfaces.Alarm#setSchedule(ScheduleAJ)
     */
    public void setSchedule(ScheduleAJ scheduleAJ) throws BusException {

        try {

            Log.d(TAG, "setSchedule is called, setting to: '" + scheduleAJ + "', objPath: '" + objectPath + "'");

            getBus().enableConcurrentCallbacks();
            getAlarm().setSchedule(scheduleAJ.toSchedule());
        } catch (IllegalArgumentException ilae) {

            Log.e(TAG, "Failed to execute 'setSchedule', objPath: '" + objectPath + "'", ilae);
            throw new ErrorReplyBusException(TimeServiceConst.INVALID_VALUE_ERROR, ilae.getMessage());
        } catch (ErrorReplyBusException erbe) {

            Log.e(TAG, "Failed to execute 'setSchedule', objPath: '" + objectPath + "'", erbe);
            throw erbe;
        } catch (Exception e) {

            Log.e(TAG, "Failed to execute 'setSchedule', objPath: '" + objectPath + "'", e);
            throw new ErrorReplyBusException(TimeServiceConst.GENERIC_ERROR, e.getMessage());
        }
    }

    /**
     * @see org.allseen.timeservice.ajinterfaces.Alarm#getTitle()
     */
    public String getTitle() throws BusException {

        String title;

        try {

            getBus().enableConcurrentCallbacks();

            Log.d(TAG, "getTitle is called, objPath: '" + objectPath + "', handling");
            title = getAlarm().getTitle();

            if (title == null) {

                throw new TimeServiceException("Undefined title");
            }
        } catch (Exception e) {

            Log.e(TAG, "Failed to execute 'getTitle', objPath: '" + objectPath + "'", e);
            throw new ErrorReplyBusException(TimeServiceConst.GENERIC_ERROR, e.getMessage());
        }

        Log.d(TAG, "Returning title: '" + title + "', objPath: '" + objectPath + "'");
        return title;
    }

    /**
     * @see org.allseen.timeservice.ajinterfaces.Alarm#setTitle(String)
     */
    public void setTitle(String title) throws BusException {

        try {

            Log.d(TAG, "setTitle is called. Setting to: '" + title + "', objPath: '" + objectPath + "'");

            getBus().enableConcurrentCallbacks();
            getAlarm().setTitle(title);
        } catch (Exception e) {

            Log.e(TAG, "Failed to execute 'setTitle', objPath: '" + objectPath + "'", e);
            throw new ErrorReplyBusException(TimeServiceConst.GENERIC_ERROR, e.getMessage());
        }
    }

    /**
     * @see org.allseen.timeservice.ajinterfaces.Alarm#getEnabled()
     */
    public boolean getEnabled() throws BusException {

        boolean enabled;

        try {

            getBus().enableConcurrentCallbacks();

            Log.d(TAG, "getEnabled is called, objPath: '" + objectPath + "', handling");
            enabled = getAlarm().isEnabled();
        } catch (Exception e) {

            Log.e(TAG, "Failed to execute 'getEnabled', objPath: '" + objectPath + "'", e);
            throw new ErrorReplyBusException(TimeServiceConst.GENERIC_ERROR, e.getMessage());
        }

        Log.d(TAG, "Returning IsEnabled: '" + enabled + "', objPath: '" + objectPath + "'");
        return enabled;
    }

    /**
     * @see org.allseen.timeservice.ajinterfaces.Alarm#setEnabled(boolean)
     */
    public void setEnabled(boolean enabled) throws BusException {

        try {

            Log.d(TAG, "setEnabled is called, objPath: '" + objectPath + "'. Setting to: '" + enabled + "'");

            getBus().enableConcurrentCallbacks();
            getAlarm().setEnabled(enabled);
        } catch (Exception e) {

            Log.e(TAG, "Failed to execute, objPath: '" + objectPath + "', 'setEnabled'", e);
            throw new ErrorReplyBusException(TimeServiceConst.GENERIC_ERROR, e.getMessage());
        }
    }

    /**
     * @see org.allseen.timeservice.ajinterfaces.Alarm#alarmReached()
     */
    public void alarmReached() {
    }

    /**
     * {@link org.allseen.timeservice.server.Alarm} object path
     *
     * @return object path
     */
    String getObjectPath() {

        return objectPath;
    }

    /**
     * Send AlarmReached signal
     *
     * @throws TimeServiceException
     *             Is thrown if failed to send the signal
     * @see org.allseen.timeservice.ajinterfaces.Alarm#alarmReached()
     */
    void sendAlarmReached() throws TimeServiceException {

        SignalEmitter emitter = new SignalEmitter(this, GlobalBroadcast.Off);
        emitter.setSessionlessFlag(true);
        emitter.setTimeToLive(ALARM_REACHED_TTL_SECONDS);

        try {

            Log.d(TAG, "Emitting AlarmReaches signal, TTL: '" + ALARM_REACHED_TTL_SECONDS + "', objPath: '" + objectPath + "'");
            emitter.getInterface(Alarm.class).alarmReached();
        } catch (BusException be) {

            throw new TimeServiceException("Failed to emit 'AlarmReached' signal", be);
        }
    }

    /**
     * Releases object resources
     */
    void release() {

        Log.d(TAG, "Releasing Server Alarm object, objPath: '" + objectPath + "'");
        alarm = null;
        try {

            getBus().unregisterBusObject(this);
        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to unregister BusObject, objPath: '" + objectPath + "'", tse);
        }

        if (announcedInterfaces != null && announcedInterfaces.length > 0) {
            for (String announcedInterface : announcedInterfaces) {
                try {
                    getBus().setAnnounceFlag(this, announcedInterface, false);
                    Log.i(TAG, "Removed announced interface: '" + announcedInterface + "'");
                } catch (TimeServiceException tse) {
                    Log.e(TAG, "Failed to setAnnounceFlag to false, interface: '" + announcedInterface + "'", tse);
                }
            }
            announcedInterfaces = new String[] {};
        }
    }

    /**
     * Access {@link TimeServiceServer} to get the {@link BusAttachment}. If
     * {@link BusAttachment} is undefined, {@link TimeServiceException} is
     * thrown.
     *
     * @return {@link BusAttachment}
     * @throws TimeServiceException
     */
    BusAttachment getBus() throws TimeServiceException {

        BusAttachment bus = TimeServiceServer.getInstance().getBusAttachment();

        if (bus == null) {

            throw new TimeServiceException("TimeServiceServer is not initialized");
        }

        return bus;
    }

    /**
     * Checks whether the object has already been created
     *
     * @return {@link org.allseen.timeservice.server.Alarm}
     * @throws TimeServiceException
     *             if {@link org.allseen.timeservice.server.Alarm} hasn't been
     *             created yet
     */
    org.allseen.timeservice.server.Alarm getAlarm() throws TimeServiceException {

        if (alarm == null) {

            throw new TimeServiceException("The Alarm object hasn't been initialized yet");
        }

        return alarm;
    }
}
