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

#ifndef NOTIFICATIONRECEIVER_H_
#define NOTIFICATIONRECEIVER_H_

#include <alljoyn/notification/Notification.h>

namespace ajn {
namespace services {

/**
 * An Abstract class with function receive. The implementation of this class
 * can be passed in to the initReceive function and will be the callback for
 * when notifications are received
 */
class NotificationReceiver {
  public:

    /**
     * Constructor for NotificationReceiver
     */
    NotificationReceiver() { };

    /**
     * Destructor for NotificationReceiver
     */
    virtual ~NotificationReceiver() { };

    /**
     * Pure abstract function that receives a notification
     * Consumer Application must override this method
     * @param notification the notification that is received
     */
    virtual void Receive(Notification const& notification) = 0;

    /**
     *   Dismiss handler
     */
    virtual void Dismiss(const int32_t msgId, const qcc::String appId) = 0;
};
} //namespace services
} //namespace ajn

#endif /* NOTIFICATIONRECEIVER_H_ */
