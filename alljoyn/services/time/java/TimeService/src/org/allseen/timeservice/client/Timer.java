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

package org.allseen.timeservice.client;

import org.alljoyn.bus.BusException;
import org.alljoyn.bus.ProxyBusObject;
import org.allseen.timeservice.Period;
import org.allseen.timeservice.TimeServiceException;
import org.allseen.timeservice.ajinterfaces.PeriodAJ;
import org.allseen.timeservice.server.TimeServiceServer;

import android.util.Log;

/**
 * Time Service Timer.
 * Use this class to communication with {@link TimeServiceServer} Timer object.
 */
public class Timer extends ObjectIntrospector {
    private static final String TAG = "ajts" + TimeClientBase.class.getSimpleName();

    /**
     * Implement this interface to receive {@link Timer} related events, such as: <br>
     * - {@link org.allseen.timeservice.ajinterfaces.Timer#timerEvent()} <br>
     * - {@link org.allseen.timeservice.ajinterfaces.Timer#runStateChanged(boolean)}
     */
    public static interface TimerHandler {

        /**
         * Handle Timer Event which was sent from the given {@link Timer}
         * @param timer The {@link Timer} that sent the TimerEvent
         * @see org.allseen.timeservice.ajinterfaces.Timer#timerEvent()
         */
        void handleTimerEvent(Timer timer);

        /**
         * Handle Run State Changed event which was sent from the given {@link Timer}
         * @param timer The {@link Timer} that sent the RunStateChanged event
         * @param isRunning TRUE if the {@link Timer} is running
         * @see org.allseen.timeservice.ajinterfaces.Timer#runStateChanged(boolean)
         */
        void handleRunStateChanged(Timer timer, boolean isRunning);
    }

    //=========================================//

    /**
     * Handler for the {@link Timer} related events
     */
    private TimerHandler timerHandler;

    /**
     * Constructor
     * @param tsClient {@link TimeServiceClient} managing this {@link Timer} object
     * @param objectPath {@link TimeServiceServer} Timer object
     */
    public Timer(TimeServiceClient tsClient, String objectPath) {

        super(tsClient, objectPath);
    }

    /**
     * Retrieve {@link Version} from the {@link TimeServiceServer} Timer object.
     * @return {@link Version}
     * @throws TimeServiceException Is thrown if failed to retrieve the {@link Version}
     */
    public short retrieveVersion() throws TimeServiceException {

        Log.d(TAG, "Retrieving Version, objPath: '" + objectPath + "'");

        try {

            short verAJ = getRemoteTimer().getVersion();
            
            Log.d(TAG, "Retrieved Version: '" + verAJ + "', objPath: '" + objectPath + "'");

            return verAJ;
        }
        catch (Exception e) {

            throw new TimeServiceException("Failed to call Timer.retrieveVersion()", e);
        }
    }

    /**
     * Retrieve timer interval from the {@link TimeServiceServer} Timer object.
     * @return {@link Timer} interval.
     * @throws TimeServiceException Is thrown if failed to retrieve the interval
     */
    public Period retrieveInterval() throws TimeServiceException {

        Log.d(TAG, "Retrieving Timer interval, objPath: '" + objectPath + "'");

        try {

            PeriodAJ periodAJ = getRemoteTimer().getInterval();
            Period period     = periodAJ.toPeriod();

            Log.d(TAG, "Retrieved Timer interval: '" + period + "', objPath: '" + objectPath + "'");

            return period;
        }
        catch (Exception e) {

            throw new TimeServiceException("Failed to call Timer.getInterval()", e);
        }
    }

    /**
     * Set Timer interval to the {@link TimeServiceServer} Timer object.
     * @param period {@link Timer} period
     * @throws TimeServiceException Is thrown if failed to set the interval
     */
    public void setInterval(Period period) throws TimeServiceException {

        if ( period == null ) {

            throw new TimeServiceException("Undefined period");
        }

        Log.d(TAG, "Setting Timer interval: '" + period + "', objPath: '" + objectPath + "'");

        try {

            getRemoteTimer().setInterval( new PeriodAJ(period) );
        }
        catch (BusException be) {

            throw new TimeServiceException("Failed to call Timer.setInterval()", be);
        }
    }

    /**
     * Retrieve amount of Time Left until the {@link Timer} fires.
     * @return {@link Period} of Time Left
     * @throws TimeServiceException Is thrown if failed to retrieve the TimeLeft property
     */
    public Period retrieveTimeLeft() throws TimeServiceException {

        Log.d(TAG, "Retrieving Timer TimeLeft, objPath: '" + objectPath + "'");

        try {

            PeriodAJ periodAJ = getRemoteTimer().getTimeLeft();
            Period period     = periodAJ.toPeriod();

            Log.d(TAG, "Retrieved TimeLeft Period: '" + period + "', objPath: '" + objectPath + "'");

            return period;
        }
        catch(Exception e) {

            throw new TimeServiceException("Failed to call Timer.retrieveTimeLeft()", e);
        }
    }

    /**
     * Retrieve whether the {@link Timer} is currently running
     * @return TRUE if the {@link Timer} is running
     * @throws TimeServiceException Is thrown if failed to retrieve the IsRunning property
     */
    public boolean retrieveIsRunning() throws TimeServiceException {

        Log.d(TAG, "Retrieving Timer IsRunning, objPath: '" + objectPath + "'");

        try {

            boolean isRunning = getRemoteTimer().getIsRunning();

            Log.d(TAG, "Retrieved IsRunning: '" + isRunning + "', objPath: '" + objectPath + "'");

            return isRunning;
        }
        catch(BusException be) {

            throw new TimeServiceException("Failed to call Timer.retrieveIsRunning()", be);
        }
    }

    /**
     * Retrieve how many times this {@link Timer} should repeat itself. <br>
     * The value of {@link org.allseen.timeservice.ajinterfaces.Timer#REPEAT_FOREVER},
     * means to repeat the timer forever
     * @return How many times this {@link Timer} should repeat itself.
     * @throws TimeServiceException Is thrown if failed to retrieve the Repeat property.
     */
    public short retrieveRepeat() throws TimeServiceException {

        Log.d(TAG, "Retrieving Repeat, objPath: '" + objectPath + "'");

        try {

            short repeat = getRemoteTimer().getRepeat();

            Log.d(TAG, "Retrieved Repeat: '" + repeat + "', objPath: '" + objectPath + "'");

            return repeat;
        }
        catch (BusException be) {

            throw new TimeServiceException("Failed to call Timer.retrieveRepeat()", be);
        }
    }

    /**
     * Set how many times this {@link Timer} should repeat itself. <br>
     * The value of {@link org.allseen.timeservice.ajinterfaces.Timer#REPEAT_FOREVER},
     * means to repeat the timer forever.
     * @param repeat How many times this {@link Timer} should repeat itself
     * @throws TimeServiceException Is thrown if failed to retrieve the Repeat property
     */
    public void setRepeat(short repeat) throws TimeServiceException {

        Log.d(TAG, "Setting Timer Repeat: '" + repeat + "', objPath: '" + objectPath + "'");

        try {

            getRemoteTimer().setRepeat(repeat);
        }
        catch (BusException be) {

            throw new TimeServiceException("Failed to call Timer.setRepeat()", be);
        }
    }

    /**
     * Retrieve this {@link Timer} title.
     * @return {@link Timer} title
     * @throws TimeServiceException Is thrown if failed to retrieve the {@link Timer} title
     */
    public String retrieveTitle() throws TimeServiceException {

        Log.d(TAG, "Retrieving Timer title, objPath: '" + objectPath + "'");

        try {

            String title = getRemoteTimer().getTitle();

            Log.d(TAG, "Timer title: '" + title + "', objPath: '" + objectPath + "'");

            return title;
        }
        catch (BusException be) {

            throw new TimeServiceException("Failed to call Timer.retrieveTitle()", be);
        }
    }

    /**
     * Set title to the {@link TimeServiceServer} Timer object.
     * @param title Timer title.
     * Title is an optional textual description of what this Timer is set for.
     * @throws TimeServiceException Is thrown if failed to set the Timer title
     */
    public void setTitle(String title) throws TimeServiceException {

        if ( title == null ) {

            throw new TimeServiceException("Undefined title");
        }

        Log.d(TAG, "Setting Timer title: '" + title + "', objPath: '" + objectPath + "'");

        try {

            getRemoteTimer().setTitle(title);
        }
        catch (BusException be) {

            throw new TimeServiceException("Failed to call Timer.setTitle()", be);
        }
    }

    /**
     * Start the {@link Timer}
     * @throws TimeServiceException Is thrown if failed to start the {@link Timer}
     */
    public void start() throws TimeServiceException {

        Log.d(TAG, "Starting the Timer, objPath: '" + objectPath + "'");

        try {

            getRemoteTimer().start();
        }
        catch (BusException be) {

            throw new TimeServiceException("Failed to call Timer.start()", be);
        }
    }

    /**
     * Pause the {@link Timer} execution
     * @throws TimeServiceException Is thrown if failed to pause the {@link Timer} execution
     */
    public void pause() throws TimeServiceException {

        Log.d(TAG, "Pausing the Timer, objPath: '" + objectPath + "'");

        try {

            getRemoteTimer().pause();
        }
        catch(BusException be) {

            throw new TimeServiceException("Failed to call Timer.pause()", be);
        }
    }

    /**
     * Reset the {@link Timer} so that TimeLeft is equal to Interval
     * @throws TimeServiceException Is thrown if failed to pause the {@link Timer} execution
     */
    public void reset() throws TimeServiceException {

        Log.d(TAG, "Resetting the Timer, objPath: '" + objectPath + "'");

        try {

            getRemoteTimer().reset();
        }
        catch (BusException be) {

            throw new TimeServiceException("Failed to call Timer.reset()", be);
        }
    }

    /**
     * Register {@link TimerHandler} to receive {@link Timer} related events
     * @param timerHandler {@link TimerHandler}
     * @throws IllegalArgumentException Is thrown if undefined timerHandler was received
     */
    public void registerTimerHandler(TimerHandler timerHandler) {

        if ( timerHandler == null ) {

            throw new IllegalArgumentException("Undefined timerHandler");
        }

        this.timerHandler = timerHandler;
        TimerSignalHandler.getInstance().registerTimer(tsClient.getBus(), this);
    }

    /**
     * Unregister {@link TimerHandler} to stop receiving {@link Timer} related events
     */
    public void unregisterTimerHandler() {

        if ( timerHandler == null ) {

            Log.w(TAG, "TimerHandler was never registered before, objPath: '" + objectPath + "'");
            return;
        }

        this.timerHandler = null;
        TimerSignalHandler.getInstance().unregisterTimer(this);
    }

    /**
     * @return {@link TimerHandler} or NULL if it hasn't been registered
     */
    public TimerHandler getTimerHandler() {

        return timerHandler;
    }

    /**
     * @see org.allseen.timeservice.client.TimeClientBase#release()
     */
    @Override
    public void release() {

        Log.i(TAG, "Releasing client Timer object: '" + getObjectPath() + "'");

        if ( timerHandler != null ) {

            Log.d(TAG, "Unregistering the 'TimerHandler'");
            unregisterTimerHandler();
        }

        super.release();
    }

    /**
     * Create and return {@link ProxyBusObject} casted to the
     * {@link org.allseen.timeservice.ajinterfaces.Timer}
     * @return {@link org.allseen.timeservice.ajinterfaces.Timer}
     * @throws TimeServiceException Is thrown if failed to create the {@link ProxyBusObject}
     * @see TimeClientBase#getProxyObject(Class[])
     */
    private org.allseen.timeservice.ajinterfaces.Timer getRemoteTimer() throws TimeServiceException {

        ProxyBusObject proxy = super.getProxyObject(new Class<?>[]{org.allseen.timeservice.ajinterfaces.Timer.class});
        return proxy.getInterface(org.allseen.timeservice.ajinterfaces.Timer.class);
    }

    /**
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {

        return "[Timer - objPath: '" + objectPath + "']";
    }
}
