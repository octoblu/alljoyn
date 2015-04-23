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

#ifndef SESSIONHANDLER_H_
#define SESSIONHANDLER_H_

#include <alljoyn/BusAttachment.h>
#include <alljoyn/SessionListener.h>
#include <alljoyn/time/TimeServiceClient.h>
#include <alljoyn/time/TimeServiceSessionListener.h>

namespace ajn {
namespace services {

/**
 * Utility class handling AllJoyn session related events and managing the session
 */
class TimeServiceSessionHandler : public BusAttachment::JoinSessionAsyncCB, public SessionListener {

  public:

    /**
     * Constructor
     *
     * @param TimeServiceClient hosting the session
     */
    TimeServiceSessionHandler(TimeServiceClient* tsClient);

    /**
     * Destructor
     */
    ~TimeServiceSessionHandler();

    /**
     * Release this object resources
     */
    void release();

    /**
     * Whether the session has been established with the server side
     */
    bool isConnected() const;

    /**
     * Returns session id
     *
     * @return ajn::SessionId
     */
    ajn::SessionId getSessionId() const;

    /**
     * Join session asynchronously with the server side
     *
     * @param sessionListener Listener object to be notified about the session related events
     */
    QStatus joinSessionAsync(TimeServiceSessionListener* sessionListener);

    /**
     * Leave the session
     *
     * @return QStatus Whether leaving session was successful
     */
    QStatus leaveSession();

  private:

    /**
     * TimeServiceClient hosting this TimeServiceSessionHandler
     */
    TimeServiceClient* m_TsClient;

    /**
     * Listener object to be notified about the session related events
     */
    TimeServiceSessionListener* m_SessionListener;

    /**
     * Session that was established with the server side
     */
    ajn::SessionId m_SessionId;

    /**
     * Gets TRUE when the session is established
     */
    bool m_IsConnected;

    /**
     * Called when JoinSessionAsync() completes.
     *
     * @param status       ER_OK if successful
     * @param sessionId    Unique identifier for session.
     * @param opts         Session options.
     * @param context      User defined context which will be passed as-is to callback.
     */
    void JoinSessionCB(QStatus status, ajn::SessionId id, const ajn::SessionOpts& opts, void* context);

    /**
     * SessionLost called when a session is lost
     *
     * @param sessionId - the session Id of the lost session
     * @param reason - The reason for the session being lost
     */
    void SessionLost(ajn::SessionId sessionId, SessionLostReason reason);

    /**
     * Checks that TimeServiceClient and BusAttachment aren't NULL
     *
     * return status ER_OK of the objects are defined
     */
    bool objectsValid(BusAttachment** bus);
};

} /* namespace services */
} /* namespace ajn */

#endif /* SESSIONHANDLER_H_ */
