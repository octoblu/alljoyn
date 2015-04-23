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
import org.allseen.timeservice.Schedule;
import org.allseen.timeservice.TimeServiceConst;
import org.allseen.timeservice.TimeServiceException;

import android.util.Log;

/**
 * Extend this class to receive events related to this {@link Alarm} object
 * and send {@link Alarm#alarmReached()} signal.
 */
public abstract class Alarm {
    private static final String TAG = "ajts" + Alarm.class.getSimpleName();

    /**
     * {@link BaseAlarmBusObj} implementing AllJoyn functionality
     */
    private BaseAlarmBusObj baseAlarmBusObj;

    /**
     * Returns {@link Schedule} of this {@link Alarm}
     * @return {@link Alarm}
     */
    public abstract Schedule getSchedule();

    /**
     * Set {@link Schedule} to this {@link Alarm} object
     * @param schedule {@link Schedule}
     * @throws ErrorReplyBusException If this {@link Alarm} does not support one of the values of {@link Schedule},
     * it throws {@link ErrorReplyBusException#ErrorReplyBusException(String, String)} with the error code of
     * {@link TimeServiceConst#INVALID_VALUE_ERROR}.
     * The message body of the ERROR should contain a string explaining what is not supported.
     */
    public abstract void setSchedule(Schedule schedule) throws ErrorReplyBusException;

    /**
     * Returns {@link Alarm} title.
     * @return Optional textual description of what this alarm is
     */
    public abstract String getTitle();

    /**
     * Set title to this {@link Alarm}.
     * @param title Alarm title.
     * Title is an optional textual description of what this Alarm is set for.
     */
    public abstract void setTitle(String title);

    /**
     * Returns whether or not this {@link Alarm} is enabled
     * @return Is {@link Alarm} enabled
     */
    public abstract boolean isEnabled();

    /**
     * Set whether or not this {@link Alarm} is enabled
     * @param enabled Is {@link Alarm} enabled
     */
    public abstract void setEnabled(boolean enabled);

    /**
     * Return the object path of this {@link Alarm} object
     * @return Object path
     */
    public String getObjectPath() {

        if ( baseAlarmBusObj == null ) {

            Log.w(TAG, "This Alarm hasn't been created yet");
            return null;
        }

        return baseAlarmBusObj.getObjectPath();
    }

    /**
     * Emit the signal when the Alarm is reached
     * @throws TimeServiceException Is thrown if failed to emit AlarmReached signal
     */
    public void alarmReached() throws TimeServiceException{

        if ( baseAlarmBusObj == null ) {

            Log.w(TAG, "This Alarm hasn't been created yet");
            return;
        }

        baseAlarmBusObj.sendAlarmReached();
    }

    /**
     * Release object resources. <br>
     * It's a programming error to call another method on this object after the release method has been called.
     */
    public void release() {

        if ( baseAlarmBusObj == null ) {

            Log.w(TAG, "This Alarm hasn't been created yet");
            return;
        }

        Log.i(TAG, "Releasing Server Alarm");
        baseAlarmBusObj.release();
        baseAlarmBusObj = null;
    }

    /**
     * Associate this {@link Alarm} with the {@link AlarmBusObj} which implements AllJoyn functionality
     * @param alarmBusObj {@link AlarmBusObj}
     */
    void setAlarmBusObj(BaseAlarmBusObj alarmBusObj) {

        baseAlarmBusObj = alarmBusObj;
    }
}
