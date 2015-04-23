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

import org.alljoyn.bus.BusException;
import org.alljoyn.bus.SignalEmitter;
import org.alljoyn.bus.SignalEmitter.GlobalBroadcast;
import org.allseen.timeservice.AuthorityType;
import org.allseen.timeservice.TimeServiceException;
import org.allseen.timeservice.ajinterfaces.Clock;
import org.allseen.timeservice.ajinterfaces.TimeAuthority;

import android.util.Log;

/**
 * This class implements {@link Clock} and {@link TimeAuthority} interfaces and
 * realizes AllJoyn communication with this Time Authority Clock.
 */
class TimeAuthorityClockBusObj extends BaseClockBusObj implements Clock, TimeAuthority {
    private static final String TAG = "ajts" + TimeAuthorityClockBusObj.class.getSimpleName();

    /**
     * Prefix of the AuthorityClock object path
     */
    private static final String OBJ_PATH_PREFIX = "/AuthorityClock";

    /**
     * {@link AuthorityType}
     */
    private final AuthorityType authorityType;

    /**
     * TTL of the Time Sync signal
     */
    private final static int TIME_SYNC_TTL_SECONDS = 180;

    /**
     * Constructor
     * 
     * @param type
     * @param clock
     *            Clock events delegate. This clock receives all the
     *            {@link TimeAuthorityClock} related events.
     * @param objectPath
     * @throws TimeServiceException
     */
    TimeAuthorityClockBusObj(AuthorityType authorityType, TimeAuthorityClock clock) throws TimeServiceException {

        super(clock, GlobalStringSequencer.append(OBJ_PATH_PREFIX));

        this.authorityType = authorityType;
    }

    /**
     * @see org.allseen.timeservice.ajinterfaces.TimeAuthority#getAuthorityType()
     */
    @Override
    public byte getAuthorityType() throws BusException {

        byte authType = authorityType.getValue();

        Log.d(TAG, "getAuthorityType is called, returning: '" + authorityType + "', value: '" + authType + "', objPath: '" + getObjectPath() + "'");
        return authType;
    }

    /**
     * @see org.allseen.timeservice.ajinterfaces.TimeAuthority#timeSync()
     */
    @Override
    public void timeSync() throws BusException {
    }

    /**
     * Send {@link TimeAuthority#timeSync()} signal
     * 
     * @throws TimeServiceException
     *             Is thrown if failed to send the signal
     */
    void sendTimeSync() throws TimeServiceException {

        SignalEmitter emitter = new SignalEmitter(this, GlobalBroadcast.Off);
        emitter.setSessionlessFlag(true);
        emitter.setTimeToLive(TIME_SYNC_TTL_SECONDS);

        try {

            Log.d(TAG, "Emitting TimeSync signal, TTL: '" + TIME_SYNC_TTL_SECONDS + "', objPath: '" + getObjectPath() + "'");
            emitter.getInterface(TimeAuthority.class).timeSync();
        } catch (BusException be) {

            throw new TimeServiceException("Failed to emit 'TymeSync' signal", be);
        }
    }

    /**
     * @see org.allseen.timeservice.server.BaseClockBusObj#release()
     */
    @Override
    protected void release() {

        super.release();

    }
}
