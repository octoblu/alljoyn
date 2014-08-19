/******************************************************************************
 * Copyright (c) 2013-2014, AllSeen Alliance. All rights reserved.
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

#ifndef NOTIFICATIONPRODUCERLISTENER_H_
#define NOTIFICATIONPRODUCERLISTENER_H_

#include <alljoyn/BusListener.h>
#include <alljoyn/SessionPortListener.h>
#include <vector>

namespace ajn {
namespace services {

/**
 * Class that implements SessionPortListener and SessionListener
 */
class NotificationProducerListener :  public SessionPortListener {
  public:

    /**
     * Constructor of NotificationProducerListener
     */
    NotificationProducerListener();

    /**
     * Destructor of NotificationProducerListener
     */
    ~NotificationProducerListener();

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

  private:

    /**
     * The port used as part of the join session request
     */
    SessionPort m_SessionPort;
};
}
}

#endif /* NOTIFICATIONPRODUCERLISTENER_H_ */
