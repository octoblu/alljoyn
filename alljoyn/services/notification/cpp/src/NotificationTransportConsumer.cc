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

#include <alljoyn/notification/NotificationService.h>

#include "NotificationTransportConsumer.h"
#include "NotificationConstants.h"
#include "PayloadAdapter.h"
#include <alljoyn/notification/LogModule.h>

using namespace ajn;
using namespace services;
using namespace nsConsts;
using namespace qcc;

NotificationTransportConsumer::NotificationTransportConsumer(
    BusAttachment* bus, String const& servicePath, QStatus& status) :
    NotificationTransport(bus, servicePath, status, AJ_NOTIFICATION_INTERFACE_NAME),
    m_IsStopping(false)
{
    if (status != ER_OK) {
        return;
    }

    pthread_mutex_init(&m_Lock, NULL);
    pthread_cond_init(&m_QueueChanged, NULL);

    status =  bus->RegisterSignalHandler(this,
                                         static_cast<MessageReceiver::SignalHandler>(&NotificationTransportConsumer::handleSignal),
                                         m_SignalMethod,
                                         NULL);

    if (status != ER_OK) {
        QCC_LogError(status, ("Could not register the SignalHandler"));
    } else {
        QCC_DbgPrintf(("Registered the SignalHandler successfully"));
    }

    pthread_create(&m_ReceiverThread, NULL, ReceiverThreadWrapper, this);
}


void NotificationTransportConsumer::handleSignal(const InterfaceDescription::Member* member, const char* srcPath, Message& msg)
{
    QCC_DbgPrintf(("Received Message from producer."));
    pthread_mutex_lock(&m_Lock);
    m_MessageQueue.push(msg);
    pthread_cond_signal(&m_QueueChanged);
    pthread_mutex_unlock(&m_Lock);
}

void NotificationTransportConsumer::unregisterHandler(BusAttachment* bus)
{
    pthread_mutex_lock(&m_Lock);
    while (!m_MessageQueue.empty()) {
        m_MessageQueue.pop();
    }
    m_IsStopping = true;
    pthread_cond_signal(&m_QueueChanged);
    pthread_mutex_unlock(&m_Lock);
    pthread_join(m_ReceiverThread, NULL);

    bus->UnregisterSignalHandler(this,
                                 static_cast<MessageReceiver::SignalHandler>(&NotificationTransportConsumer::handleSignal),
                                 m_SignalMethod,
                                 NULL);

    pthread_cond_destroy(&m_QueueChanged);
    pthread_mutex_destroy(&m_Lock);
}

void* NotificationTransportConsumer::ReceiverThreadWrapper(void* context)
{
    NotificationTransportConsumer* consumer = reinterpret_cast<NotificationTransportConsumer*>(context);
    if (consumer == NULL) { // should not happen
        return NULL;
    }
    consumer->ReceiverThread();
    return NULL;
}


void NotificationTransportConsumer::ReceiverThread()
{
    pthread_mutex_lock(&m_Lock);
    while (!m_IsStopping) {
        while (!m_MessageQueue.empty()) {
            Message message = m_MessageQueue.front();
            m_MessageQueue.pop();
            pthread_mutex_unlock(&m_Lock);
            PayloadAdapter::receivePayload(message);
            pthread_mutex_lock(&m_Lock);
        }
        if (!m_IsStopping) {
            pthread_cond_wait(&m_QueueChanged, &m_Lock);
        }
    }
    pthread_mutex_unlock(&m_Lock);
}








