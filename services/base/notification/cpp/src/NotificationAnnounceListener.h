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

#ifndef NOTIFICATIONANNOUNCELISTENER_H_
#define NOTIFICATIONANNOUNCELISTENER_H_

#include <alljoyn/AboutListener.h>
#include <alljoyn/about/AnnounceHandler.h>
#include <alljoyn/about/AnnouncementRegistrar.h>

namespace ajn {
namespace services {

class SuperAgentBusListener;

/**
 * Notification announce lisener. Receives announce signal from super agent.
 */
class NotificationAnnounceListener : public ajn::AboutListener, public ajn::services::AnnounceHandler {

  public:
    /**
     * Constructor of class NotificationAnnounceListener
     */
    NotificationAnnounceListener();
    /**
     * Destructor of class NotificationAnnounceListener
     */
    ~NotificationAnnounceListener();
    /**
     * a callback for getting announce message
     * @param busName - the bus unique name of the announce sender
     * @param version
     * @param port
     * @param objectDescriptionArg - object description related to the sender
     * @param aboutDataArg
     */
    virtual void Announced(const char* busName, uint16_t version, ajn::SessionPort port, const ajn::MsgArg& objectDescriptionArg, const ajn::MsgArg& aboutDataArg);

    /**
     * a callback for getting announce message
     * @param version
     * @param port
     * @param busName - the bus unique name of the announce sender
     * @param objectDescs description related to the sender
     * @param aboutData
     */
    void Announce(uint16_t version, uint16_t port, const char* busName, const ObjectDescriptions& objectDescs, const AboutData& aboutData);

  private:
};
} //namespace services
} //namespace ajn

#endif /* NOTIFICATIONANNOUNCELISTENER_H_ */
