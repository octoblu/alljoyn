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

#include "NotificationProducerSender.h"
#include "Transport.h"
#include "NotificationConstants.h"
#include <alljoyn/notification/NotificationService.h>
#include <alljoyn/notification/LogModule.h>

using namespace ajn;
using namespace services;
using namespace qcc;
using namespace nsConsts;

NotificationProducerSender::NotificationProducerSender(ajn::BusAttachment* bus, QStatus& status) :
    NotificationProducer(bus, status)
{

    /**
     * Do not add code until the status that returned from the base class is verified.
     */
    if (status != ER_OK) {
        return;
    }

}

NotificationProducerSender::~NotificationProducerSender()
{
}

QStatus NotificationProducerSender::Dismiss(const char* busName, ajn::SessionId sessionId, int32_t mgsId)
{
    QCC_DbgPrintf(("NotificationProducerSender::Dismiss busName:%s sessionId:%u mgsId:%d", busName, sessionId, mgsId));

    QStatus status = ER_OK;
    if (!m_InterfaceDescription) {
        return ER_FAIL;
    }
    ProxyBusObject*proxyBusObj = new ProxyBusObject(*m_BusAttachment, busName, AJ_NOTIFICATION_PRODUCER_PATH.c_str(), sessionId);
    if (!proxyBusObj) {
        return ER_FAIL;
    }
    status = proxyBusObj->AddInterface(*m_InterfaceDescription);
    if (status != ER_OK) {
        QCC_LogError(status, ("MethodCallDismiss - AddInterface."));
        delete proxyBusObj;
        proxyBusObj = NULL;
        return status;
    }
    MsgArg args[1];
    Message replyMsg(*m_BusAttachment);

    args[0].Set(AJPARAM_INT.c_str(), mgsId);
    status = proxyBusObj->MethodCall(AJ_NOTIFICATION_PRODUCER_INTERFACE.c_str(), AJ_DISMISS_METHOD_NAME.c_str(), args, 1, replyMsg);
    if (status != ER_OK) {
        QCC_LogError(status, ("MethodCallDismiss - MethodCall."));
        delete proxyBusObj;
        proxyBusObj = NULL;
        return status;
    }

    delete proxyBusObj;
    proxyBusObj = NULL;
    return status;
}
