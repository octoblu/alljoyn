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

#ifndef NOTIFICATIONDISMISSERSENDER_H_
#define NOTIFICATIONDISMISSERSENDER_H_

#include <alljoyn/notification/NotificationEnums.h>
#include "NotificationDismisser.h"
#include "NotificationConstants.h"

namespace ajn {
namespace services {

/**
 * Implementing the sender side of the dismiss signal in dismisser interface
 */
class NotificationDismisserSender : public NotificationDismisser {

  public:

    /**
     * Constructor for TransportProducer. Creates Interface and prepares
     * infrastructure to be able to send Signal
     * @param bus            - BusAttachment that is used
     * @param objectPath     - Object path name of the dismisser
     * @param status         - success/failure
     */
    NotificationDismisserSender(ajn::BusAttachment* bus, qcc::String const& objectPath, QStatus& status);

    /**
     * Destructor for TransportProducer
     */
    ~NotificationDismisserSender() { };

    /**
     * Send Signal over Bus.
     * @param notificationArgs
     * @param ttl
     * @return status
     */
    QStatus sendSignal(ajn::MsgArg const dismisserArgs[nsConsts::AJ_DISMISSER_NUM_PARAMS],
                       uint16_t ttl);


};
} //namespace services
} //namespace ajn

#endif /* NOTIFICATIONDISMISSERSENDER_H_ */
