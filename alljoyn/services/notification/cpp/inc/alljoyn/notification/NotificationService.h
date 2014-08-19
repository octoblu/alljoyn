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

#ifndef NOTIFICATIONSERVICE_H_
#define NOTIFICATIONSERVICE_H_

#include <qcc/Debug.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/notification/NotificationSender.h>
#include <alljoyn/about/PropertyStore.h>

namespace ajn {
namespace services {

class NotificationReceiver;

/**
 * Notification Service class. Used to initialize and shutdown the service
 */
class NotificationService {

  public:

    /**
     * Get Instance of NotificationServiceImpl - singleton implementation
     * @return instance
     */
    static NotificationService* getInstance();

    /**
     * Destructor for NotificationServiceImpl
     */
    ~NotificationService();

    /**
     *  Initialize Producer side via Transport. Create and
     *  return NotificationSender.
     * @param bus
     * @param store
     * @return NotificationSender instance
     */

    NotificationSender* initSend(ajn::BusAttachment* bus, ajn::services::PropertyStore* store);


    /**
     * Initialize Consumer side via Transport.
     * Set NotificationReceiver to given receiver
     * @param bus
     * @param notificationReceiver
     * @return status
     */
    QStatus initReceive(ajn::BusAttachment* bus, NotificationReceiver* notificationReceiver);

    /**
     * Stops sender but leaves bus and other objects alive
     */
    void shutdownSender();

    /**
     * Stops receiving but leaves bus and other objects alive
     */
    void shutdownReceiver();

    /**
     * Cleanup and get ready for shutdown
     */
    void shutdown();

    /**
     * Disabling superagent mode. Needs to be called before
     * starting receiver
     * @return status
     */
    QStatus disableSuperAgent();

    /**
     * Virtual method to get the busAttachment used in the service.
     */
    ajn::BusAttachment* getBusAttachment();

    /**
     * Get the Version of the NotificationService
     * @return the NotificationService version
     */
    static uint16_t getVersion();

  private:

    /**
     * Default constructor for NotificationServiceImpl
     * Private to allow for singleton implementation
     */
    NotificationService();

    /**
     * Version of the API
     */
    static uint16_t const NOTIFICATION_SERVICE_VERSION;

    /**
     * instance variable - NotificationServiceImpl is a singleton
     */
    static NotificationService* s_Instance;
};
} //namespace services
} //namespace ajn

#endif /* NOTIFICATIONSERVICE_H_ */
