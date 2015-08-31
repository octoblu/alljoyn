
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

#ifndef NOTIFICATIONPRODUCERSENDER_H_
#define NOTIFICATIONPRODUCERSENDER_H_

#include "NotificationProducer.h"

namespace ajn {
namespace services {

/**
 * NotificationProducerSender class
 * Calls NotificationProducer interface methods
 */
class NotificationProducerSender : public ajn::services::NotificationProducer {

  public:
    /**
     * constructor of NotificationProducerSender
     * @param bus attachment
     * @param status
     */
    NotificationProducerSender(ajn::BusAttachment* bus, QStatus& status);
    /**
     * destructor of NotificationProducerSender
     */
    ~NotificationProducerSender();

    /*
     * calls method Dismiss at the producer side
     * @param bus attachment
     * @param relevant session id
     * @param elevant message id
     * @return status
     */
    QStatus Dismiss(const char* busName, ajn::SessionId sessionId, int32_t mgsId);

  private:

};
} //namespace services
} //namespace ajn

#endif /* NOTIFICATIONPRODUCERSENDER_H_ */
