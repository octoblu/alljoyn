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

#ifndef NOTIFICATIONPRODUCERRECEIVER_H_
#define NOTIFICATIONPRODUCERRECEIVER_H_

#include "NotificationProducer.h"
#include <queue>
#include <pthread.h>

namespace ajn {
namespace services {

/**
 * class MsgQueueContent
 *
 */
class MsgQueueContent {
  public:
    MsgQueueContent(uint32_t msgId) : m_MsgId(msgId) { }

    uint32_t m_MsgId;
};

/**
 * class NotificationProducerReceiver
 * Implements NotificationProducer interface at producer side
 */
class NotificationProducerReceiver : public ajn::services::NotificationProducer {

  public:
    /**
     * constructor of NotificationProducerReceiver
     * @param bus attachment
     * @param status
     */
    NotificationProducerReceiver(ajn::BusAttachment* bus, QStatus& status);

    /**
     * destructor of NotificationProducerReceiver
     */
    ~NotificationProducerReceiver();

    /**
     *  Handles  Dismiss method
     * @param[in]  member
     * @param[in]  msg reference of AllJoyn Message
     */
    void Dismiss(const ajn::InterfaceDescription::Member* member, ajn::Message& msg);

    /**
     * SetAppIdArg
     * @param application id argument
     */
    void SetAppIdArg(MsgArg appIdArg) {
        m_AppIdArg = appIdArg;
    }
    /**
     * unregisterHandler
     * @param bus attachment
     */
    void unregisterHandler(BusAttachment* bus);
  private:
    /**
     * implement method calls from notification producer interface
     */
    void HandleMethodCall(const ajn::InterfaceDescription::Member* member, ajn::Message& msg);

    /**
     * The thread responsible for receiving the notification
     */
    pthread_t m_ReceiverThread;

    /**
     * A Queue that holds the messages
     */
    std::queue<MsgQueueContent> m_MessageQueue;

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
    void Receiver();

    /**
     * sendDismissSignal
     */
    QStatus sendDismissSignal(int32_t msgId);

    /**
     * appIdArg
     */
    MsgArg m_AppIdArg;

};
} //namespace services
} //namespace ajn

#endif /* NOTIFICATIONPRODUCERRECEIVER_H_ */
