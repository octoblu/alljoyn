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

#ifndef NOTIFICATIONRECEIVERIMPL_H_
#define NOTIFICATIONRECEIVERIMPL_H_

#include <alljoyn/notification/NotificationReceiver.h>
#include <alljoyn/notification/Notification.h>

/**
 * NotificationReceiverImpl
 */
class NotificationReceiverImpl : public ajn::services::NotificationReceiver {
  public:
    /**
     * Constructor
     */
    NotificationReceiverImpl();

    /**
     * Destructor
     */
    ~NotificationReceiverImpl();

    /**
     * Receive - function that receives a notification
     * @param notification
     */
    void Receive(ajn::services::Notification const& notification);

    /**
     * receive Dismiss signal
     * @param msgId message id
     * @param appId application id
     */
    void Dismiss(const int32_t msgId, const qcc::String appId);

};

#endif /* NOTIFICATIONRECEIVERIMPL_H_ */
