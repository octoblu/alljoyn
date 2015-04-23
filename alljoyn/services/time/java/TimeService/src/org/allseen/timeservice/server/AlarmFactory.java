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
import org.allseen.timeservice.TimeServiceConst;

import android.util.Log;

/**
 * Extend this class to receive events related to this {@link AlarmFactory} object
 */
public abstract class AlarmFactory {
    private static final String TAG = "ajts" + AlarmFactory.class.getSimpleName();

    /**
     * {@link AlarmFactoryBusObj} implementing AllJoyn functionality
     */
    private AlarmFactoryBusObj alarmFactoryBusObj;

    /**
     * Create new {@link Alarm}
     * @return {@link Alarm}
     * @throws ErrorReplyBusException Is thrown if create new Alarm fails due to max permitted number of Alarm objects
     * reached. {@link ErrorReplyBusException} should be thrown with the error code of:
     * {@link TimeServiceConst#OUT_OF_RANGE} and an error description.
     */
    public abstract Alarm newAlarm() throws ErrorReplyBusException;

    /**
     * Find {@link Alarm} by the given object path.
     * To delete the {@link Alarm}, call {@link Alarm#release()}.
     * @param objectPath Object path of the {@link Alarm} to delete.
     * @throws ErrorReplyBusException Is thrown if for any reason failed to delete the {@link Alarm}.
     * Throw {@link ErrorReplyBusException} with the error code of: {@link TimeServiceConst#GENERIC_ERROR}
     * and an error description.
     */
    public abstract void deleteAlarm(String objectPath) throws ErrorReplyBusException;

    /**
     * Return the object path of this {@link AlarmFactory} object
     * @return Object path
     */
    public String getObjectPath() {

        if ( alarmFactoryBusObj == null ) {

            Log.w(TAG, "This Alarm Factory hasn't been created yet");
            return null;
        }

        return alarmFactoryBusObj.getObjectPath();
    }

    /**
     * Release object resources. <br>
     * It's a programming error to call another method on this object after the release method has been called.
     */
    public void release() {

        if ( alarmFactoryBusObj == null ) {

            Log.w(TAG, "The Alarm Factory hasn't been created yet");
            return;
        }

        Log.i(TAG, "Releasing server AlarmFactory");
        alarmFactoryBusObj.release();
        alarmFactoryBusObj = null;
    }

    /**
     * Associate this {@link AlarmFactory} with the {@link AlarmFactoryBusObj} which implements AllJoyn functionality
     * @param clockBusObj {@link AlarmFactoryBusObj}
     */
    void setAlarmBusObj(AlarmFactoryBusObj alarmFactoryBusObj) {

        this.alarmFactoryBusObj = alarmFactoryBusObj;
    }
}
