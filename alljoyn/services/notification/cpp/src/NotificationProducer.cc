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

#include "NotificationProducer.h"
#include <alljoyn/BusAttachment.h>
#include <qcc/Debug.h>
#include "NotificationConstants.h"
#include <alljoyn/notification/NotificationService.h>
#include <alljoyn/notification/LogModule.h>

using namespace ajn;
using namespace services;
using namespace qcc;
using namespace nsConsts;

NotificationProducer::NotificationProducer(ajn::BusAttachment* bus, QStatus& status) :
    BusObject(AJ_NOTIFICATION_PRODUCER_PATH.c_str()), m_InterfaceDescription(NULL), m_BusAttachment(bus)
{
    m_InterfaceDescription = const_cast<InterfaceDescription*>(m_BusAttachment->GetInterface(AJ_NOTIFICATION_PRODUCER_INTERFACE.c_str()));
    if (!m_InterfaceDescription) {
        status = m_BusAttachment->CreateInterface(nsConsts::AJ_NOTIFICATION_PRODUCER_INTERFACE.c_str(), m_InterfaceDescription, false);
        if (status != ER_OK) {
            QCC_LogError(status, ("CreateInterface failed"));
            return;
        }

        if (!m_InterfaceDescription) {
            status = ER_FAIL;
            QCC_LogError(status, ("m_InterfaceDescription is NULL"));
            return;
        }

        status = m_InterfaceDescription->AddMethod(AJ_DISMISS_METHOD_NAME.c_str(), AJ_DISMISS_METHOD_PARAMS.c_str(), NULL, AJ_DISMISS_METHOD_PARAMS_NAMES.c_str());
        if (status != ER_OK) {
            QCC_LogError(status, ("AddMethod failed."));
            return;
        }

        status = m_InterfaceDescription->AddProperty(AJ_PROPERTY_VERSION.c_str(), AJPARAM_UINT16.c_str(), (uint8_t) PROP_ACCESS_READ);
        if (status != ER_OK) {
            QCC_LogError(status, ("AddMethod failed."));
            return;
        }

        m_InterfaceDescription->Activate();
    }

    status = AddInterface(*m_InterfaceDescription);
    if (status != ER_OK) {
        QCC_LogError(status, ("AddInterface failed."));
        return;
    }

}

NotificationProducer::~NotificationProducer()
{

}

QStatus NotificationProducer::Get(const char* ifcName, const char* propName, MsgArg& val)
{
    QCC_DbgTrace(("Get property was called."));

    if (0 != strcmp(AJ_PROPERTY_VERSION.c_str(), propName)) {
        QCC_LogError(ER_BUS_NO_SUCH_PROPERTY, ("Called for property different than version."));
        return ER_BUS_NO_SUCH_PROPERTY;
    }

    val.typeId = ALLJOYN_UINT16;
    val.v_uint16 = NOTIFICATION_PRODUCER_VERSION;
    return ER_OK;
}

QStatus NotificationProducer::Set(const char* ifcName, const char* propName, MsgArg& val)
{
    return ER_ALLJOYN_ACCESS_PERMISSION_ERROR;
}
