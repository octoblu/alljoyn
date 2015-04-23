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

import org.allseen.timeservice.TimeServiceException;

import android.util.Log;

/**
 * Extend this class to receive events related to this {@link TimeAuthorityClock} object
 * and send {@link TimeAuthorityClock#timeSync()} signal.
 */
public abstract class TimeAuthorityClock extends Clock {
    private static final String TAG = "ajts" + TimeAuthorityClock.class.getSimpleName();

    /**
     * {@link TimeAuthorityClockBusObj} implementing AllJoyn functionality of the
     * time authority clock
     */
    private TimeAuthorityClockBusObj timeAuthorityClockBusObj;

    /**
     * Send TimeSync signal to suggest this {@link TimeAuthorityClock} clients to
     * synchronize their time
     * @throws TimeServiceException Is thrown if failed to send the signal
     */
    public void timeSync() throws TimeServiceException {

        if ( timeAuthorityClockBusObj == null ) {

            Log.w(TAG, "This Clock hasn't been created yet");
            return;
        }

        timeAuthorityClockBusObj.sendTimeSync();
    }

    /**
     * @see org.allseen.timeservice.server.Clock#setClockBusObj(org.allseen.timeservice.server.BaseClockBusObj)
     */
    @Override
    void setClockBusObj(BaseClockBusObj clockBusObj) {

        timeAuthorityClockBusObj = (TimeAuthorityClockBusObj) clockBusObj;
        super.setClockBusObj(clockBusObj);
    }

    /**
     * @see org.allseen.timeservice.server.Clock#release()
     */
    @Override
    public void release() {

        super.release();
        timeAuthorityClockBusObj = null;
    }
}
