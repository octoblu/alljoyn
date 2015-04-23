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

import org.alljoyn.bus.ErrorReplyBusException;
import org.allseen.timeservice.Period;
import org.allseen.timeservice.TimeServiceConst;
import org.allseen.timeservice.TimeServiceException;

import android.util.Log;

/**
 * Extend this class to receive events related to this {@link Timer} object
 * and send {@link Timer#timerEvent()} signal.
 */
public abstract class Timer {
    private static final String TAG = "ajts" + Timer.class.getSimpleName();

    /**
     * {@link BaseTimerBusObj} implementing AllJoyn functionality
     */
    private BaseTimerBusObj baseTimerBusObj;

    /**
     * Returns total time the {@link Timer} should count down
     * @return {@link Period} of time to count down
     */
    public abstract Period getInterval();

    /**
     * Set total time the {@link Timer} should count down
     */
    public abstract void setInterval(Period period);

    /**
     * Returns amount of time left
     * @return Amount of time left
     * @return {@link Period}
     */
    public abstract Period getTimeLeft();

    /**
     * Whether the {@link Timer} is currently running
     * @return True if the {@link Timer} is currently running
     */
    public abstract boolean isRunning();

    /**
     * Returns how many times this {@link Timer} should repeat itself.
     * The value of {@link org.allseen.timeservice.ajinterfaces.Timer#REPEAT_FOREVER},
     * means to repeat the {@link Timer} forever
     * @return How many times this timer should repeat itself.
     */
    public abstract short getRepeat();

    /**
     * Set how many times this {@link Timer} should repeat itself.
     * The value of {@link org.allseen.timeservice.ajinterfaces.Timer#REPEAT_FOREVER},
     * means to repeat the timer forever
     * @param repeat How many times this {@link Timer} should repeat itself.
     * @throws ErrorReplyBusException If this {@link Timer} doesn't support the "Repeat"  feature
     * it throws {@link ErrorReplyBusException#ErrorReplyBusException(String, String)} with
     * the error code of {@link TimeServiceConst#INVALID_VALUE_ERROR}.
     * The message body of the ERROR should contain a string explaining that the feature is not
     * supported.
     */
    public abstract void setRepeat(short repeat) throws ErrorReplyBusException;

    /**
     * Returns get textual description of what this {@link Timer} is.
     * @return Textual description of what this {@link Timer} is
     */
    public abstract String getTitle();

    /**
     * Set title to this {@link Timer}.
     * @param title Timer title
     * Title is an optional textual description of what this Timer is set for.
     */
    public abstract void setTitle(String title);

    /**
     * Start the {@link Timer}
     */
    public abstract void start();

    /**
     * Pause the {@link Timer}
     */
    public abstract void pause();

    /**
     * Reset the {@link Timer} so that the TimeLeft is equal to the {@link Timer} Interval
     */
    public abstract void reset();

    /**
     * Return the object path of this {@link Timer} object
     * @return Object path
     */
    public String getObjectPath() {

        if ( baseTimerBusObj == null ) {

            Log.w(TAG, "This Timer hasn't been created yet");
            return null;
        }

        return baseTimerBusObj.getObjectPath();
    }

    /**
     * Emit the signal when the {@link Timer} fires
     * @throws TimeServiceException Is thrown if failed to emit the TimerEvent signal
     */
    public void timerEvent() throws TimeServiceException {

        if ( baseTimerBusObj == null ) {

            Log.w(TAG, "This Timer hasn't been created yet");
            return;
        }

        baseTimerBusObj.sendTimerEvent();
    }

    /**
     * Emit the signal when the {@link Timer} changes its running state as a result of
     * {@link Timer#start()} or {@link Timer#pause()}.
     * @param isRunning TRUE if the {@link Timer} is running
     * @throws TimeServiceException Is thrown if failed to emit the RunStateChanged signal
     */
    public void runStateChanged(boolean isRunning) throws TimeServiceException {

        if ( baseTimerBusObj == null ) {

            Log.w(TAG, "This Timer hasn't been created yet");
            return;
        }

        baseTimerBusObj.sendRunStateChanged(isRunning);
    }

    /**
     * Release object resources. <br>
     * It's a programming error to call another method on this object after the release method has been called.
     */
    public void release() {

        if ( baseTimerBusObj == null ) {

            Log.w(TAG, "This Timer hasn't been created yet");
            return;
        }

        Log.i(TAG, "Releasing server Timer");
        baseTimerBusObj.release();
        baseTimerBusObj = null;
    }

    /**
     * Associate this {@link Timer} with the {@link TimerBusObj} which implements AllJoyn functionality
     * @param timerBusObj {@link TimerBusObj}
     */
    void setTimerBusObj(BaseTimerBusObj timerBusObj) {

        baseTimerBusObj = timerBusObj;
    }
}
