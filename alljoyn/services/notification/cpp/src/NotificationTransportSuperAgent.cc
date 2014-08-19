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

#include "NotificationConstants.h"
#include "NotificationTransportSuperAgent.h"
#include "PayloadAdapter.h"
#include "Transport.h"
#include <alljoyn/notification/LogModule.h>

using namespace qcc;
using namespace ajn;
using namespace services;
using namespace nsConsts;

ReceiveNotificationTaskData::ReceiveNotificationTaskData(Message& msg) : message(msg)
{
}

void ReceiveNotificationAsyncTask::OnEmptyQueue()
{
}

void ReceiveNotificationAsyncTask::OnTask(TaskData const* taskdata)
{
    QCC_DbgPrintf(("Handling notification from superagent"));
    const ReceiveNotificationTaskData* receiveNotificationTaskData = static_cast<const ReceiveNotificationTaskData*>(taskdata);
    if (receiveNotificationTaskData) {
        PayloadAdapter::receivePayload(const_cast<ReceiveNotificationTaskData*>(receiveNotificationTaskData)->message);
    }
}

NotificationTransportSuperAgent::NotificationTransportSuperAgent(
    BusAttachment* bus, String const& servicePath, QStatus& status, bool isFirstSuperAgent)  :
    NotificationTransport(bus, servicePath, status, AJ_SA_INTERFACE_NAME),
    m_IsFirstSuperAgent(isFirstSuperAgent)
{
    if (status != ER_OK) {
        return;
    }

    m_receiveNotificationTaskQueue = new AsyncTaskQueue(&m_receiveNotificationAsyncTask);
    m_receiveNotificationTaskQueue->Start();

    status =  bus->RegisterSignalHandler(this,
                                         static_cast<MessageReceiver::SignalHandler>(&NotificationTransportSuperAgent::handleSignal),
                                         m_SignalMethod,
                                         NULL);

    if (status != ER_OK) {
        QCC_LogError(status, ("Could not register the SignalHandler"));
    } else {
        QCC_DbgPrintf(("Registered the SignalHandler successfully"));
    }

}


void NotificationTransportSuperAgent::handleSignal(const InterfaceDescription::Member* member, const char* srcPath, Message& msg)
{
    QCC_DbgPrintf(("Got notification from super agent"));
    Transport* transport = Transport::getInstance();
    // We must enable concurrent callbacks since some of the calls below are blocking
    transport->getBusAttachment()->EnableConcurrentCallbacks();

    if (m_IsFirstSuperAgent) {
        m_IsFirstSuperAgent = false;
        QCC_DbgPrintf(("Found first super agent."));

        const char* messageSender = msg->GetSender();
        if (transport->FindSuperAgent(messageSender) != ER_OK) {
            QCC_DbgHLPrintf(("Could not listen to SuperAgent."));
            m_IsFirstSuperAgent = true;     //so that we try again
            return;
        }
    }

    String sender = msg->GetSender();
    QCC_DbgPrintf(("Received Message from super agent: %s", sender.c_str()));
    if (m_receiveNotificationTaskQueue) {
        m_receiveNotificationTaskQueue->Enqueue(new ReceiveNotificationTaskData(msg));
    }
}

void NotificationTransportSuperAgent::unregisterHandler(BusAttachment* bus)
{
    bus->UnregisterSignalHandler(this,
                                 static_cast<MessageReceiver::SignalHandler>(&NotificationTransportSuperAgent::handleSignal),
                                 m_SignalMethod,
                                 NULL);

    if (m_receiveNotificationTaskQueue) {
        m_receiveNotificationTaskQueue->Stop();
        delete m_receiveNotificationTaskQueue;
        m_receiveNotificationTaskQueue = NULL;
    }
}

void NotificationTransportSuperAgent::setIsFirstSuperAgent(bool isFirstSuperAgent)
{
    m_IsFirstSuperAgent = isFirstSuperAgent;
}
