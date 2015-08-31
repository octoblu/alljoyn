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

#ifndef CONTROLPANELBUSLISTENER_H_
#define CONTROLPANELBUSLISTENER_H_

#include <alljoyn/BusListener.h>
#include <alljoyn/SessionPortListener.h>
#include <vector>

namespace ajn {
namespace services {

/**
 * Class that implements BusListener, SessionPortListener and SessionListener
 */
class ControlPanelBusListener : public BusListener, public SessionPortListener,
    public SessionListener {

  public:

    /**
     * Constructor of ControlPanelBusListener
     */
    ControlPanelBusListener();

    /**
     * Destructor of ControlPanelBusListener
     */
    ~ControlPanelBusListener();

    /**
     * Set the Value of the SessionPort associated with this SessionPortListener
     * @param sessionPort
     */
    void setSessionPort(SessionPort sessionPort);

    /**
     * Get the SessionPort of the listener
     * @return
     */
    SessionPort getSessionPort();

    /**
     * AcceptSessionJoiner - Receive request to join session and decide whether to accept it or not
     * @param sessionPort - the port of the request
     * @param joiner - the name of the joiner
     * @param opts - the session options
     * @return true/false
     */
    bool AcceptSessionJoiner(SessionPort sessionPort, const char* joiner, const SessionOpts& opts);

    /**
     * SessionJoined
     * @param sessionPort
     * @param sessionId
     * @param joiner
     */
    void SessionJoined(SessionPort sessionPort, SessionId sessionId, const char* joiner);

    /**
     * SessionMemberAdded
     * @param sessionId
     * @param uniqueName
     */
    void SessionMemberAdded(SessionId sessionId, const char* uniqueName);

    /**
     * SessionMemberRemoved
     * @param sessionId
     * @param uniqueName
     */
    void SessionMemberRemoved(SessionId sessionId, const char* uniqueName);

    /**
     * SessionLost
     * @param sessionId
     * @param reason
     */
    void SessionLost(SessionId sessionId, SessionLostReason reason);

    /**
     * Get the SessionIds associated with this Listener
     * @return vector of sessionIds
     */
    const std::vector<SessionId>& getSessionIds() const;

  private:

    /**
     * The port used as part of the join session request
     */
    SessionPort m_SessionPort;

    /**
     * The sessionIds for the port
     */
    std::vector<SessionId> m_SessionIds;
};
}
}

#endif /* CONTROLPANELBUSLISTENER_H_ */
