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

import org.allseen.timeservice.DateTime;

import android.util.Log;

/**
 * Extend this class to receive events related to this {@link Clock} object.
 */
public abstract class Clock {
    private static final String TAG = "ajts" + Clock.class.getSimpleName();

    /**
     * {@link BaseClockBusObj} implementing AllJoyn functionality
     */
    private BaseClockBusObj baseClockBusObj;

    /**
     * @return {@link DateTime} object.
     */
    public abstract DateTime getDateTime();

    /**
     * Set the {@link DateTime} object
     * @param dateTime {@link DateTime}
     */
    public abstract void setDateTime(DateTime dateTime);

    /**
     * Whether or not this clock has ever been set.
     * @return Returns TRUE if the {@link Clock} has been set since the last reboot
     */
    public abstract boolean getIsSet();

    /**
     * Returns the object path of this {@link Clock} object
     * @return object path
     */
    public String getObjectPath() {

        if ( baseClockBusObj == null ) {

            Log.w(TAG, "This Clock hasn't been created yet");
            return null;
        }

        return baseClockBusObj.getObjectPath();
    }

    /**
     * Release object resources. <br>
     * It's a programming error to call another method on this object after the release method has been called.
     */
    public void release() {

        if ( baseClockBusObj == null ) {

            Log.w(TAG, "This Clock hasn't been created yet");
            return;
        }

        Log.i(TAG, "Releasing Server Clock");
        baseClockBusObj.release();
        baseClockBusObj = null;
    }

    /**
     * Associate this {@link Clock} with the {@link ClockBusObj} which implements AllJoyn functionality
     * @param clockBusObj {@link ClockBusObj}
     */
    void setClockBusObj(BaseClockBusObj clockBusObj) {

        this.baseClockBusObj = clockBusObj;
    }
}
