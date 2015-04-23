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

#include <alljoyn/notification/NotificationAsyncTaskEvents.h>
#include <alljoyn/services_common/AsyncTaskQueue.h>
#include <alljoyn/notification/NotificationService.h>
#include <alljoyn/notification/Notification.h>
#include "Transport.h"
#include "NotificationProducerSender.h"
#include "NotificationDismisserSender.h"
#include <alljoyn/notification/Notification.h>
#include <alljoyn/notification/NotificationService.h>
#include <sstream>
#include <alljoyn/notification/LogModule.h>
#include <qcc/StringUtil.h>

using namespace ajn;
using namespace services;
using namespace qcc;

NotificationAsyncTaskEvents::NotificationAsyncTaskEvents()
{
}

NotificationAsyncTaskEvents::~NotificationAsyncTaskEvents()
{
}

void NotificationAsyncTaskEvents::OnEmptyQueue() {
}

void NotificationAsyncTaskEvents::OnTask(TaskData const* taskData)
{
    QCC_DbgTrace(("NotificationAsyncTaskEvents", "OnTask() called"));

    NotificationMsg const* notificationMsg = static_cast<NotificationMsg const*>(taskData);

    QStatus status = ER_OK;
    Transport* pTransport = Transport::getInstance();
    SessionOpts opts(SessionOpts::TRAFFIC_MESSAGES, false, SessionOpts::PROXIMITY_ANY, TRANSPORT_ANY);
    ajn::SessionId sessionId(0);
    if (notificationMsg->m_OriginalSender.length() > 0) {
        status = pTransport->getBusAttachment()->JoinSession(notificationMsg->m_OriginalSender.c_str(), (ajn::SessionPort)nsConsts::AJ_NOTIFICATION_PRODUCER_SERVICE_PORT, NULL, sessionId, opts);
    } else {
        QCC_DbgHLPrintf(("There is no original sender in the message. Can't join session."));
        status = ER_FAIL;
    }
    if ((ER_OK != status) && (status != ER_ALLJOYN_JOINSESSION_REPLY_ALREADY_JOINED)) {
        QCC_LogError(status, ("Failed to JoinSession to %s", notificationMsg->m_OriginalSender.c_str()));
        sendDismissSignal(notificationMsg);
        return;
    } else {
        QCC_DbgPrintf(("JoinSession to %s SUCCEEDED (Session id=%u)", notificationMsg->m_OriginalSender.c_str(), sessionId));
    }

    NotificationProducerSender* pNotificationProducerSender = Transport::getInstance()->getNotificationProducerSender();
    if (pNotificationProducerSender == NULL) {
        status = ER_FAIL;
        return;
    }

    status = pNotificationProducerSender->Dismiss(notificationMsg->m_OriginalSender.c_str(), sessionId, notificationMsg->m_MessageId);
    if (status != ER_OK) {
        QCC_LogError(status, ("Dismiss failed"));
        sendDismissSignal(taskData);
    } else {
        QCC_DbgPrintf(("Dismiss succeeded"));
    }

    status = Transport::getInstance()->getBusAttachment()->LeaveSession(sessionId);
}

void NotificationAsyncTaskEvents::sendDismissSignal(TaskData const* taskData)
{
    QCC_DbgPrintf(("NotificationAsyncTaskEvents", "sendDismissSignal() called!"));

    NotificationMsg const*  notificationMsg = static_cast<NotificationMsg const*>(taskData);

    QStatus status = ER_OK;
    MsgArg msgIdArg;
    MsgArg appIdArg;

    status = msgIdArg.Set(nsConsts::AJPARAM_INT.c_str(), notificationMsg->m_MessageId);
    if (status != ER_OK) {
        return;
    }

    uint8_t AppId[16];
    HexStringToBytes(notificationMsg->m_AppId, AppId, 16);
    status = appIdArg.Set(nsConsts::AJPARAM_ARR_BYTE.c_str(), 16, AppId);
    if (status != ER_OK) {
        return;
    }

    {
        MsgArg dismisserArgs[nsConsts::AJ_DISMISSER_NUM_PARAMS];
        dismisserArgs[0] = msgIdArg;
        dismisserArgs[1] = appIdArg;

        /**Code commented below is for future use
         * In case dismiss signal will not need to be sent via different object path each time.
         * In that case enable code below and disable next paragraph.
         *
         * status = Transport::getInstance()->getNotificationDismisserSender()->sendSignal(dismisserArgs,nsConsts::TTL_MAX);
         * if (status != ER_OK) {
         * QCC_LogError(status,("NotificationAsyncTaskEvents", "sendSignal failed."));
         * return;
         * }
         *
         * End of paragraph
         */

        /*
         * Paragraph to be disabled in case dismiss signal will not need to be sent via different object path each time
         */
        std::ostringstream stm;
        stm << abs(notificationMsg->m_MessageId);
        qcc::String objectPath = nsConsts::AJ_NOTIFICATION_DISMISSER_PATH + "/" + notificationMsg->m_AppId + "/" + std::string(stm.str()).c_str();
        NotificationDismisserSender notificationDismisserSender(Transport::getInstance()->getBusAttachment(), objectPath, status);
        status = Transport::getInstance()->getBusAttachment()->RegisterBusObject(notificationDismisserSender);
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not register NotificationDismisserSender."));
            return;
        }
        status = notificationDismisserSender.sendSignal(dismisserArgs, nsConsts::TTL_MAX);
        if (status != ER_OK) {
            QCC_LogError(status, ("sendSignal failed."));
            return;
        }

        Transport::getInstance()->getBusAttachment()->UnregisterBusObject(notificationDismisserSender);

        /*
         * End of paragraph
         */
    }
}


