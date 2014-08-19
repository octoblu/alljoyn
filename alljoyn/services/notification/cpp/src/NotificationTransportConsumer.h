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

#ifndef NOTIFICATIONTRANSPORTCONSUMER_H_
#define NOTIFICATIONTRANSPORTCONSUMER_H_

#include <queue>
#include <pthread.h>

#include "NotificationTransport.h"
#include <pthread.h>

namespace ajn {
namespace services {

/**
 * Notification Transport Consumer. Used to Create the Notification Interface
 * on the receiver side and register a signal handler to receive the notify signals
 */
class NotificationTransportConsumer : public NotificationTransport {
  public:

    /**
     * Constructor for TransportConsumer
     * @param bus         - BusAttachment that is used
     * @param servicePath - servicePath of BusObject
     * @param status      - success/failure
     */
    NotificationTransportConsumer(ajn::BusAttachment* bus,
                                  qcc::String const& servicePath, QStatus& status);

    /**
     * Destructor of TransportConsumer
     */
    ~NotificationTransportConsumer() { };

    /**
     * Callback when Signal arrives
     * @param member    Method or signal interface member entry.
     * @param srcPath   Object path of signal emitter.
     * @param message   The received message.
     */
    void handleSignal(const ajn::InterfaceDescription::Member* member,
                      const char* srcPath, ajn::Message& msg);

    void unregisterHandler(ajn::BusAttachment* bus);

  private:

    /**
     * The thread responsible for receiving the notification
     */
    pthread_t m_ReceiverThread;

    /**
     * A Queue that holds the messages
     */
    std::queue<ajn::Message> m_MessageQueue;

    /**
     * The mutex Lock
     */
    pthread_mutex_t m_Lock;

    /**
     * The Queue Changed thread condition
     */
    pthread_cond_t m_QueueChanged;

    /**
     * is the thread in the process of shutting down
     */
    bool m_IsStopping;

    /**
     * A wrapper for the receiver Thread
     * @param context
     */
    static void* ReceiverThreadWrapper(void* context);

    /**
     * The function run in the ReceiverThread
     */
    void ReceiverThread();
};
} //namespace services
} //namespace ajn

#endif /* NOTIFICATIONTRANSPORTCONSUMER_H_ */
