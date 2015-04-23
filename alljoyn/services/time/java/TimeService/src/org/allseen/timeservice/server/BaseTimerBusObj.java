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
import org.allseen.timeservice.Period;
import org.allseen.timeservice.Time;
import org.allseen.timeservice.TimeServiceConst;
import org.allseen.timeservice.TimeServiceException;
import org.allseen.timeservice.ajinterfaces.PeriodAJ;
import org.allseen.timeservice.ajinterfaces.Timer;
import org.allseen.timeservice.ajinterfaces.TimerFactory;

import android.util.Log;

/**
 * This is the base class for all the classes implementing {@link Timer}
 * interface. Extend this class to handle AllJoyn communication with this
 * {@link Timer}. Most of the {@link Timer} related calls delegated to the
 * {@link org.allseen.timeservice.server.Timer} object.
 */
// Commented out because currently JavaBinding doesn't support finding AJ
// interfaces in the SuperClasses
// so they must be implemented by the leaf classes
public abstract class BaseTimerBusObj implements BusObject /* implements Timer */{
    private static final String TAG = "ajts" + BaseTimerBusObj.class.getSimpleName();

    /**
     * Timer to be notified with the
     * {@link org.allseen.timeservice.server.Timer} related calls
     */
    private org.allseen.timeservice.server.Timer timer;

    /**
     * Prefix of the Timer object path
     */
    private static final String OBJ_PATH_PREFIX = "/Timer";

    /**
     * Timer event TTL
     */
    private static final int TIMER_EVENT_TTL_SECONDS = 50;

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
     * @param timer
     *            {@link org.allseen.timeservice.server.Timer} delegate
     * @param parentObj
     *            Parent object path if the object was created via
     *            {@link TimerFactory}
     * @param needAnnounce
     *            If the object was created via {@link TimerFactory} no need to
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
    void init(org.allseen.timeservice.server.Timer timer, String parentObj, boolean needAnnounce, String[] notAnnounced, String description, String language, Translator translator)
            throws TimeServiceException {

        if (timer == null) {

            throw new TimeServiceException("Undefined timer");
        }

        if (!(this instanceof BusObject)) {

            throw new TimeServiceException("Class not a BusObject");
        }

        this.timer = timer;
        objectPath = parentObj + GlobalStringSequencer.append(OBJ_PATH_PREFIX);

        TimeServiceServer tsServer = TimeServiceServer.getInstance();
        tsServer.registerBusObject(this, objectPath, description, language, translator);

        // No need to send announcement if this Timer object was created with
        // TimerFactory
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
     * @see org.allseen.timeservice.ajinterfaces.Timer#getVersion()
     */
    public short getVersion() throws BusException {

        return Timer.VERSION;
    }

    /**
     * @see org.allseen.timeservice.ajinterfaces.Timer#getInterval()
     */
    public PeriodAJ getInterval() throws BusException {

        PeriodAJ periodAJ;

        try {

            getBus().enableConcurrentCallbacks();

            Log.d(TAG, "getInterval is called, objPath: '" + objectPath + "', handling");

            Period period = getTimer().getInterval();

            if (period == null) {

                throw new TimeServiceException("Undefined period");
            }

            periodAJ = new PeriodAJ(period);
        } catch (Exception e) {

            Log.e(TAG, "Failed to execute 'getInterval', objPath: '" + objectPath + "'", e);
            throw new ErrorReplyBusException(TimeServiceConst.GENERIC_ERROR, e.getMessage());
        }

        Log.d(TAG, "Returning Interval: '" + periodAJ + "', objPath: '" + objectPath + "'");
        return periodAJ;
    }

    /**
     * @see org.allseen.timeservice.ajinterfaces.Timer#setInterval(PeriodAJ)
     */
    public void setInterval(PeriodAJ periodAJ) throws BusException {

        try {

            Log.d(TAG, "setInterval is called, setting to: '" + periodAJ + "', objPath: '" + objectPath + "'");

            getBus().enableConcurrentCallbacks();
            getTimer().setInterval(periodAJ.toPeriod());
        } catch (IllegalArgumentException ilae) {

            Log.e(TAG, "Failed to execute 'setInterval', objPath: '" + objectPath + "'", ilae);
            throw new ErrorReplyBusException(TimeServiceConst.INVALID_VALUE_ERROR, ilae.getMessage());
        } catch (Exception e) {

            Log.e(TAG, "Failed to execute 'setInterval', objPath: '" + objectPath + "'", e);
            throw new ErrorReplyBusException(TimeServiceConst.GENERIC_ERROR, e.getMessage());
        }
    }

    /**
     * @see org.allseen.timeservice.ajinterfaces.Timer#getTimeLeft()
     */
    public PeriodAJ getTimeLeft() throws BusException {

        PeriodAJ periodAJ;

        try {

            getBus().enableConcurrentCallbacks();

            Log.d(TAG, "getTimeLeft is called, objPath: '" + objectPath + "', handling");

            Period period = getTimer().getTimeLeft();

            if (period == null) {

                throw new TimeServiceException("Undefined period");
            }

            periodAJ = new PeriodAJ(period);
        } catch (Exception e) {

            Log.e(TAG, "Failed to execute 'getTimeLeft', objPath: '" + objectPath + "'", e);
            throw new ErrorReplyBusException(TimeServiceConst.GENERIC_ERROR, e.getMessage());
        }

        Log.d(TAG, "Returning TimeLeft: '" + periodAJ + "', objPath: '" + objectPath + "'");
        return periodAJ;
    }

    /**
     * @see org.allseen.timeservice.ajinterfaces.Timer#getIsRunning()
     */
    public boolean getIsRunning() throws BusException {

        boolean isRunning;

        try {

            getBus().enableConcurrentCallbacks();

            Log.d(TAG, "getIsRunning is called, objPath: '" + objectPath + "', handling");

            isRunning = getTimer().isRunning();
        } catch (Exception e) {

            Log.e(TAG, "Failed to execute 'getIsRunning', objPath: '" + objectPath + "'", e);
            throw new ErrorReplyBusException(TimeServiceConst.GENERIC_ERROR, e.getMessage());
        }

        Log.d(TAG, "Returning IsRunning: '" + isRunning + "', objPath: '" + objectPath + "'");
        return isRunning;
    }

    /**
     * @see org.allseen.timeservice.ajinterfaces.Timer#getRepeat()
     */
    public short getRepeat() throws BusException {

        short repeat;

        try {

            getBus().enableConcurrentCallbacks();

            Log.d(TAG, "getRepeat is called, objPath: '" + objectPath + "', handling");

            repeat = getTimer().getRepeat();
        } catch (Exception e) {

            Log.e(TAG, "Failed to execute 'getRepeat', objPath: '" + objectPath + "'", e);
            throw new ErrorReplyBusException(TimeServiceConst.GENERIC_ERROR, e.getMessage());
        }

        Log.d(TAG, "Returning repeat: '" + repeat + "', objPath: '" + objectPath + "'");
        return repeat;
    }

    /**
     * @see org.allseen.timeservice.ajinterfaces.Timer#setRepeat(short)
     */
    public void setRepeat(short repeatTimes) throws BusException {

        try {

            Log.d(TAG, "setRepeat is called, setting to: '" + repeatTimes + "', objPath: '" + objectPath + "'");

            getBus().enableConcurrentCallbacks();
            getTimer().setRepeat(repeatTimes);
        } catch (ErrorReplyBusException erbe) {

            Log.e(TAG, "Failed to execute 'setRepeat', objPath: '" + objectPath + "'", erbe);
            throw erbe;
        } catch (Exception e) {

            Log.e(TAG, "Failed to execute 'setRepeat', objPath: '" + objectPath + "'", e);
            throw new ErrorReplyBusException(TimeServiceConst.GENERIC_ERROR, e.getMessage());
        }
    }

    /**
     * @see org.allseen.timeservice.ajinterfaces.Timer#getTitle()
     */
    public String getTitle() throws BusException {

        String title;

        try {

            getBus().enableConcurrentCallbacks();

            Log.d(TAG, "getTitle is called, objPath: '" + objectPath + "', handling");
            title = getTimer().getTitle();

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
     * @see org.allseen.timeservice.ajinterfaces.Timer#setTitle(String)
     */
    public void setTitle(String title) throws BusException {

        try {

            Log.d(TAG, "setTitle is called. Setting to: '" + title + "', objPath: '" + objectPath + "'");

            getBus().enableConcurrentCallbacks();
            getTimer().setTitle(title);
        } catch (Exception e) {

            Log.e(TAG, "Failed to execute 'setTitle', objPath: '" + objectPath + "'", e);
            throw new ErrorReplyBusException(TimeServiceConst.GENERIC_ERROR, e.getMessage());
        }
    }

    /**
     * @see org.allseen.timeservice.ajinterfaces.Timer#start()
     */
    public void start() throws BusException {

        try {

            getBus().enableConcurrentCallbacks();

            Log.d(TAG, "Timer start is called, objPath: '" + objectPath + "',  handling");
            getTimer().start();
        } catch (Exception e) {

            Log.e(TAG, "Failed to start the Timer, objPath: '" + objectPath + "'", e);
            throw new ErrorReplyBusException(TimeServiceConst.GENERIC_ERROR, e.getMessage());
        }
    }

    /**
     * @see org.allseen.timeservice.ajinterfaces.Timer#pause()
     */
    public void pause() throws BusException {

        try {

            getBus().enableConcurrentCallbacks();

            Log.d(TAG, "Timer pause is called, objPath: '" + objectPath + "', handling");
            getTimer().pause();
        } catch (Exception e) {

            Log.e(TAG, "Failed to pause the Timer, objPath: '" + objectPath + "'", e);
            throw new ErrorReplyBusException(TimeServiceConst.GENERIC_ERROR, e.getMessage());
        }
    }

    /**
     * @see org.allseen.timeservice.ajinterfaces.Timer#reset()
     */
    public void reset() throws BusException {

        try {

            getBus().enableConcurrentCallbacks();

            Log.d(TAG, "Timer reset is called, objPath: '" + objectPath + "', handling");
            getTimer().reset();
        } catch (Exception e) {

            Log.e(TAG, "Failed to reset the Timer, objPath: '" + objectPath + "'", e);
            throw new ErrorReplyBusException(TimeServiceConst.GENERIC_ERROR, e.getMessage());
        }
    }

    /**
     * @see org.allseen.timeservice.ajinterfaces.Timer#timerEvent()
     */
    public void timerEvent() throws BusException {
    }

    /**
     * @see Timer#runStateChanged(boolean)
     */
    public void runStateChanged(boolean runState) throws BusException {
    }

    /**
     * {@link Time} object path
     *
     * @return object path
     */
    String getObjectPath() {

        return objectPath;
    }

    /**
     * Send TimerEvent as a session-less-signal
     *
     * @throws TimeServiceException
     *             Is thrown if failed to send the signal
     * @see {@link org.allseen.timeservice.ajinterfaces.Timer#timerEvent()}
     */
    void sendTimerEvent() throws TimeServiceException {

        SignalEmitter emitter = new SignalEmitter(this, GlobalBroadcast.Off);
        emitter.setSessionlessFlag(true);
        emitter.setTimeToLive(TIMER_EVENT_TTL_SECONDS);

        try {

            Log.d(TAG, "Emitting TimerEvent signal, TTL: '" + TIMER_EVENT_TTL_SECONDS + "', objPath: '" + objectPath + "'");
            emitter.getInterface(Timer.class).timerEvent();
        } catch (BusException be) {

            throw new TimeServiceException("Failed to emit 'TimerEvent' signal", be);
        }
    }

    /**
     * Send RunStateChanged signal
     *
     * @throws TimeServiceException
     *             Is thrown if failed to send the signal
     * @see {@link org.allseen.timeservice.ajinterfaces.Timer#runStateChanged(boolean)}
     */
    void sendRunStateChanged(boolean isRunning) throws TimeServiceException {

        Integer sid = TimeServiceServer.getInstance().getSessionId();

        if (sid == null) {

            Log.w(TAG, "No session is established with the Timer, unable to send 'RunStateChanged' signal");
            return;
        }

        SignalEmitter emitter = new SignalEmitter(this, sid, GlobalBroadcast.Off);

        try {

            Log.d(TAG, "Emitting RunStateChanged signal, IsRunning: '" + isRunning + "', objPath: '" + objectPath + "'");
            emitter.getInterface(Timer.class).runStateChanged(isRunning);
        } catch (BusException be) {

            throw new TimeServiceException("Failed to emit 'RunStateChanged' signal", be);
        }
    }

    /**
     * Releases object resources
     */
    void release() {

        Log.d(TAG, "Releasing Server Timer object, objPath: '" + objectPath + "'");
        timer = null;
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
     * @return {@link org.allseen.timeservice.server.Timer}
     * @throws TimeServiceException
     *             if {@link org.allseen.timeservice.server.Timer} hasn't been
     *             created yet
     */
    org.allseen.timeservice.server.Timer getTimer() throws TimeServiceException {

        if (timer == null) {

            throw new TimeServiceException("The Timer object hasn't been initialized yet");
        }

        return timer;
    }

}