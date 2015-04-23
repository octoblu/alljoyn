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

import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.OnJoinSessionListener;
import org.alljoyn.bus.SessionListener;
import org.alljoyn.bus.SessionOpts;
import org.alljoyn.bus.Status;
import org.allseen.timeservice.TimeServiceConst;
import org.allseen.timeservice.server.TimeServiceServer;

import android.util.Log;

/**
 * Utility class handling AllJoyn session related events and managing the session
 */
class TimeServiceSessionListener extends SessionListener {
    private static final String TAG = "ajts" + TimeServiceSessionListener.class.getSimpleName();

    /**
     * @see OnJoinSessionListener
     */
    private class SessionJoinedCB extends OnJoinSessionListener {

        /**
         * @see org.alljoyn.bus.OnJoinSessionListener#onJoinSession(org.alljoyn.bus.Status, int, org.alljoyn.bus.SessionOpts, java.lang.Object)
         */
        @Override
        public void onJoinSession(Status status, int sessionId, SessionOpts opts, Object context) {

            tsClient.getBus().enableConcurrentCallbacks();

            synchronized (TimeServiceSessionListener.this) {

                Log.d(TAG, "SessionJoined, status: '" + status + "', sessionId: '" + sessionId +
                                "', bus: '" + tsClient.getServerBusName() + "'");

                if ( status == Status.OK || status == Status.ALLJOYN_JOINSESSION_REPLY_ALREADY_JOINED ) {

                    sid = sessionId;
                }

                if ( sessionHandler != null && tsClient != null ) {

                    Log.d(TAG, "Delegating SessionJoined to the SessionHandler");
                    sessionHandler.sessionJoined(tsClient, status);
                }
            }
        }
    }

    //=========================================================//

    /**
     * The session id.
     * Null means no session is established
     */
    private Integer sid;

    /**
     * Listener object to be notified about the session related events
     */
    private SessionListenerHandler sessionHandler;

    /**
     * {@link TimeServiceClient}
     */
    private TimeServiceClient tsClient;

    /**
     * Constructor
     * @param tsClient {@link TimeServiceClient}
     */
    TimeServiceSessionListener(TimeServiceClient tsClient) {

        this.tsClient       = tsClient;
    }

    /**
     * Join session asynchronously with a {@link TimeServiceServer}
     * @param sessionHandler Listener object to be notified about the session related events
     */
    synchronized void joinSessionAsync(SessionListenerHandler sessionHandler) {

        if ( sessionHandler == null || tsClient == null ) {

            Log.e(TAG, "Looks like TimeServiceSessionListener has been previously released, returning");
            return;
        }

        this.sessionHandler = sessionHandler;
        BusAttachment bus   = tsClient.getBus();
        String busName      = tsClient.getServerBusName();

        if ( sid != null ) {

            Log.d(TAG, "The session is already joined with busName: '" + busName + "', sessionId: '" + sid + "'");
            sessionHandler.sessionJoined(tsClient, Status.OK);

            return;
        }

        Log.d(TAG, "Joining session, busName: '" + busName + "'");
        Status status = bus.joinSession(busName, TimeServiceConst.PORT_NUM, getSessionOpts(), this, new SessionJoinedCB(),
                                           null);

        if ( status != Status.OK ) {

           Log.e(TAG, "joinSession call has failed, Status: '" + status + "', busName: '" + busName + "'");
           sessionHandler.sessionJoined(tsClient, status);
        }
    }

    /**
     * Leave the session
     * @return {@link Status}
     */
    synchronized Status leaveSession() {

        if ( sid == null ) {

            Log.w(TAG, "leaveSession was called but, sid is UNDEFINED, returning Status.FAIL, busName: '" +
                    tsClient.getServerBusName() + "'");

            sessionHandler = null;
            return Status.FAIL;
        }
        else if ( sid == 0 ) {

            Log.w(TAG, "leaveSession was called but, sid is ZERO, returning Status.OK, busName: '" +
                    tsClient.getServerBusName() + "'");

            sessionHandler = null;
            sid            = null;

            return Status.OK;
        }

        Status status = tsClient.getBus().leaveSession(sid);

        Log.d(TAG, "leaveSession was called, sid: '" + sid + "', busName: '" + tsClient.getServerBusName() +
                   "', Status: '" + status + "'");

        if ( status == Status.OK ) {

            sessionHandler = null;
            sid            = null;
        }

        return status;
    }

    /**
     * Releases this object resources
     */
    synchronized void release() {

        Log.d(TAG, "Releasing SessionListener");
        leaveSession();
        sessionHandler = null;
        tsClient       = null;
    }

    /**
     * @return Session Id
     */
    synchronized Integer getSessionId() {

        return sid;
    }

    /**
     * @see org.alljoyn.bus.SessionListener#sessionLost(int, int)
     */
    @Override
    public synchronized void sessionLost(int sessionId, int reason) {

        tsClient.getBus().enableConcurrentCallbacks();

        sid = null;

        Log.d(TAG, "Received sessionLost for sid: '" + sessionId + "', reason: '" + reason +
                       "', busName: '" + tsClient.getServerBusName() + "'");


        if ( sessionHandler != null && tsClient != null) {

            Log.d(TAG, "Delegating SessionLost to the SessionHandler");
            sessionHandler.sessionLost(reason, tsClient);
        }
    }

    /**
     * Create and returns {@link SessionOpts} object
     * @return {@link SessionOpts}
     */
    private SessionOpts getSessionOpts() {

        SessionOpts sessionOpts  = new SessionOpts();
        sessionOpts.traffic      = SessionOpts.TRAFFIC_MESSAGES;  // Use reliable message-based communication to move data between session endpoints
        sessionOpts.isMultipoint = true;                          // A session is multi-point if it can  be joined multiple times
        sessionOpts.proximity    = SessionOpts.PROXIMITY_ANY;     // Holds the proximity for this SessionOpt
        sessionOpts.transports   = SessionOpts.TRANSPORT_ANY;     // Holds the allowed transports for this SessionOpts

        return sessionOpts;
    }
}
