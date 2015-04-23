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

#include "NotificationProducerReceiver.h"
#include "Transport.h"
#include "NotificationConstants.h"
#include <alljoyn/notification/NotificationService.h>
#include <sstream>
#include <iostream>
#include "NotificationDismisserSender.h"
#include <alljoyn/notification/LogModule.h>
#include <qcc/StringUtil.h>
#ifdef _WIN32
#include <process.h>
#endif

using namespace ajn;
using namespace services;
using namespace qcc;
using namespace nsConsts;


NotificationProducerReceiver::NotificationProducerReceiver(ajn::BusAttachment* bus, QStatus& status) :
    NotificationProducer(bus, status), m_IsStopping(false)
{
    /**
     * Do not add code until the status that returned from the base class is verified.
     */
    if (status != ER_OK) {
        return;
    }

    status = AddMethodHandler(m_InterfaceDescription->GetMember(AJ_DISMISS_METHOD_NAME.c_str()),
                              static_cast<MessageReceiver::MethodHandler>(&NotificationProducerReceiver::Dismiss));
    if (status != ER_OK) {
        QCC_LogError(status, ("AddMethodHandler failed."));
        return;
    }

#ifdef _WIN32
    InitializeCriticalSection(&m_Lock);
    InitializeConditionVariable(&m_QueueChanged);
    m_handle = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 256 * 1024, (unsigned int (__stdcall*)(void*))ReceiverThreadWrapper, this, 0, NULL));
#else
    pthread_mutex_init(&m_Lock, NULL);
    pthread_cond_init(&m_QueueChanged, NULL);
    pthread_create(&m_ReceiverThread, NULL, ReceiverThreadWrapper, this);
#endif
}

NotificationProducerReceiver::~NotificationProducerReceiver()
{
    QCC_DbgTrace(("start"));

    QCC_DbgTrace(("end"));
}

void NotificationProducerReceiver::unregisterHandler(BusAttachment* bus)
{
#ifdef _WIN32
    EnterCriticalSection(&m_Lock);
    while (!m_MessageQueue.empty()) {
        m_MessageQueue.pop();
    }
    m_IsStopping = true;
    WakeConditionVariable(&m_QueueChanged);
    LeaveCriticalSection(&m_Lock);
    WaitForSingleObject(m_handle, INFINITE);
    CloseHandle(m_handle);
    DeleteCriticalSection(&m_Lock);
#else
    pthread_mutex_lock(&m_Lock);
    while (!m_MessageQueue.empty()) {
        m_MessageQueue.pop();
    }
    m_IsStopping = true;
    pthread_cond_signal(&m_QueueChanged);
    pthread_mutex_unlock(&m_Lock);
    pthread_join(m_ReceiverThread, NULL);
    pthread_cond_destroy(&m_QueueChanged);
    pthread_mutex_destroy(&m_Lock);
#endif
}

void* NotificationProducerReceiver::ReceiverThreadWrapper(void* context)
{
    QCC_DbgTrace(("NotificationProducerReceiver::ReceiverThreadWrapper()"));
    NotificationProducerReceiver* pNotificationProducerReceiver = reinterpret_cast<NotificationProducerReceiver*>(context);
    if (pNotificationProducerReceiver == NULL) { // should not happen
        return NULL;
    }
    pNotificationProducerReceiver->Receiver();
    return NULL;
}

void NotificationProducerReceiver::Dismiss(const ajn::InterfaceDescription::Member* member, ajn::Message& msg)
{
    QCC_DbgTrace(("NotificationProducerReceiver::Dismiss()"));
    HandleMethodCall(member, msg);
}

void NotificationProducerReceiver::HandleMethodCall(const ajn::InterfaceDescription::Member* member, ajn::Message& msg)
{
    QCC_DbgTrace(("NotificationProducerReceiver::HandleMethodCall()"));
    const ajn::MsgArg* args = 0;
    size_t numArgs = 0;
    QStatus status = ER_OK;

    msg->GetArgs(numArgs, args);
    if (numArgs != 1) {
        status = ER_INVALID_DATA;
        goto exit;
    }
    int32_t msgId;
    status = args[0].Get(AJPARAM_INT.c_str(), &msgId);
    if (status != ER_OK) {
        goto exit;
    }

    QCC_DbgPrintf(("msgId:%d", msgId));

    MethodReply(msg, args, 0);
#ifdef _WIN32
    EnterCriticalSection(&m_Lock);
    {
        MsgQueueContent msgQueueContent(msgId);
        m_MessageQueue.push(msgQueueContent);
        QCC_DbgPrintf(("HandleMethodCall() - message pushed"));
    }
    WakeConditionVariable(&m_QueueChanged);
    LeaveCriticalSection(&m_Lock);
#else
    pthread_mutex_lock(&m_Lock);
    {
        MsgQueueContent msgQueueContent(msgId);
        m_MessageQueue.push(msgQueueContent);
        QCC_DbgPrintf(("HandleMethodCall() - message pushed"));
    }
    pthread_cond_signal(&m_QueueChanged);
    pthread_mutex_unlock(&m_Lock);
#endif

exit:
    if (status != ER_OK) {
        MethodReply(msg, ER_INVALID_DATA);
        QCC_LogError(status, (""));
    }
}

void NotificationProducerReceiver::Receiver()
{
#ifdef _WIN32
    EnterCriticalSection(&m_Lock);
    while (!m_IsStopping) {
        while (!m_MessageQueue.empty()) {
            MsgQueueContent message = m_MessageQueue.front();
            m_MessageQueue.pop();
            QCC_DbgPrintf(("NotificationProducerReceiver::ReceiverThread() - got a message."));
            LeaveCriticalSection(&m_Lock);
            Transport::getInstance()->deleteMsg(message.m_MsgId);
            sendDismissSignal(message.m_MsgId);
            EnterCriticalSection(&m_Lock);
        }

        // it's possible m_IsStopping changed while executing OnTask() (which is done unlocked)
        //  therefore we have to check it again here, otherwise we potentially deadlock here
        if (!m_IsStopping) {
            //pthread_testcancel(); //for win only?
            SleepConditionVariableCS(&m_QueueChanged, &m_Lock, INFINITE);
        }
    }
    LeaveCriticalSection(&m_Lock);
#else
    pthread_mutex_lock(&m_Lock);
    while (!m_IsStopping) {
        while (!m_MessageQueue.empty()) {
            MsgQueueContent message = m_MessageQueue.front();
            m_MessageQueue.pop();
            QCC_DbgPrintf(("NotificationProducerReceiver::ReceiverThread() - got a message."));
            pthread_mutex_unlock(&m_Lock);
            Transport::getInstance()->deleteMsg(message.m_MsgId);
            sendDismissSignal(message.m_MsgId);
            pthread_mutex_lock(&m_Lock);
        }

        // it's possible m_IsStopping changed while executing OnTask() (which is done unlocked)
        //  therefore we have to check it again here, otherwise we potentially deadlock here
        if (!m_IsStopping) {
            pthread_cond_wait(&m_QueueChanged, &m_Lock);
        }
    }
    pthread_mutex_unlock(&m_Lock);
#endif
}

QStatus NotificationProducerReceiver::sendDismissSignal(int32_t msgId)
{
    QCC_DbgTrace(("Notification::sendDismissSignal() called"));
    QStatus status;
    MsgArg msgIdArg;

    msgIdArg.Set(nsConsts::AJPARAM_INT.c_str(), msgId);

    MsgArg dismisserArgs[nsConsts::AJ_DISMISSER_NUM_PARAMS];
    dismisserArgs[0] = msgIdArg;
    dismisserArgs[1] = m_AppIdArg;

    /**Code commented below is for future use
     * In case dismiss signal will not need to be sent via different object path each time.
     * In that case enable code below and disable next paragraph.
     *
     * Transport::getInstance()->getNotificationDismisserSender()->sendSignal(dismisserArgs,nsConsts::TTL_MAX);
     * if (status != ER_OK) {
     * QCC_LogError(status,"NotificationAsyncTaskEvents", "sendSignal failed.");
     * return;
     * }
     *
     * End of paragraph
     */

    /*
     * Paragraph to be disabled in case dismiss signal will not need to be sent via different object path each time
     */
    String appId;
    uint8_t* appIdBin;
    size_t len;
    m_AppIdArg.Get(AJPARAM_ARR_BYTE.c_str(), &len, &appIdBin);

    appId = BytesToHexString(appIdBin, len);

    std::ostringstream stm;
    stm << abs(msgId);
    qcc::String objectPath = nsConsts::AJ_NOTIFICATION_DISMISSER_PATH + "/" + appId + "/" + std::string(stm.str()).c_str();
    NotificationDismisserSender notificationDismisserSender(Transport::getInstance()->getBusAttachment(), objectPath, status);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not create NotificationDismisserSender."));
    }
    status = Transport::getInstance()->getBusAttachment()->RegisterBusObject(notificationDismisserSender);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not register NotificationDismisserSender."));
        return status;
    }
    QCC_DbgPrintf(("sendDismissSignal: going to send dismiss signal with object path %s", objectPath.c_str()));
    status = notificationDismisserSender.sendSignal(dismisserArgs, nsConsts::TTL_MAX);
    if (status != ER_OK) {
        QCC_LogError(status, ("Failed to sendSignal"));
        return status;
    }

    Transport::getInstance()->getBusAttachment()->UnregisterBusObject(notificationDismisserSender);

    /*
     * End of paragraph
     */

    return status;
}
