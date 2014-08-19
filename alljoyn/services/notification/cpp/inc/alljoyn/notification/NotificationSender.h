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

#ifndef NOTIFICATIONSENDER_H_
#define NOTIFICATIONSENDER_H_

#include <vector>
#include <map>
#include <alljoyn/Status.h>
#include <alljoyn/about/PropertyStore.h>
#include <alljoyn/notification/NotificationEnums.h>

namespace ajn {
namespace services {

class Notification;

/**
 * The class used to send Notifications or delete the last sent Notification
 */
class NotificationSender {
  public:

    /**
     * Constructor for NotificationSenderImpl
     * @param propertyStore - propertyStoreImplementation that includes entries
     * for deviceId, deviceName, appId and appName
     */
    NotificationSender(ajn::services::PropertyStore* propertyStore);

    /**
     * Destructor for NotificationSenderImpl
     */
    ~NotificationSender() { };

    /**
     * Send notification
     * @param notification
     * @param ttl
     * @return
     */
    QStatus send(Notification const& notification, uint16_t ttl);

    /**
     * Delete last message that was sent with given MessageType
     * @param messageType MessageType of message to be deleted
     * @return success/failure
     */
    QStatus deleteLastMsg(NotificationMessageType messageType);


  private:

    /**
     * Device Id that will be sent with the Notifications
     */
    ajn::services::PropertyStore* m_PropertyStore;
};
} //namespace services
} //namespace ajn

#endif /* NOTIFICATIONSENDER_H_ */
