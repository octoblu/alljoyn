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

#include <alljoyn/notification/NotificationService.h>

#include "NotificationDismisser.h"
#include "NotificationConstants.h"
#include <alljoyn/notification/LogModule.h>

using namespace ajn;
using namespace services;
using namespace nsConsts;
using namespace qcc;

NotificationDismisser::NotificationDismisser(ajn::BusAttachment* bus, qcc::String const& objectPath, QStatus& status) :
    BusObject(objectPath.c_str()), m_SignalMethod(0), m_InterfaceDescription(NULL)
{
    m_InterfaceDescription = const_cast<InterfaceDescription*>(bus->GetInterface(AJ_NOTIFICATION_DISMISSER_INTERFACE.c_str()));
    if (!m_InterfaceDescription) {
        if ((status = bus->CreateInterface(nsConsts::AJ_NOTIFICATION_DISMISSER_INTERFACE.c_str(), m_InterfaceDescription, false)) != ER_OK) {
            return;
        }

        if (!m_InterfaceDescription) {
            status = ER_FAIL;
            QCC_LogError(status, ("m_InterfaceDescription is NULL"));
            return;
        }

        status = m_InterfaceDescription->AddSignal(AJ_DISMISS_SIGNAL_NAME.c_str(), AJ_DISMISS_SIGNAL_PARAMS.c_str(), AJ_DISMISS_PARAM_NAMES.c_str(), 0);
        if (status != ER_OK) {
            QCC_LogError(status, ("AddSignal failed."));
            return;
        }
        status = m_InterfaceDescription->AddProperty(AJ_PROPERTY_VERSION.c_str(), AJPARAM_UINT16.c_str(), PROP_ACCESS_READ);
        if (status != ER_OK) {
            QCC_LogError(status, ("AddProperty failed."));
            return;
        }
        m_InterfaceDescription->Activate();
    }

    status = AddInterface(*m_InterfaceDescription, ANNOUNCED);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not add interface."));
        return;
    }

    // Get the signal method for future use
    m_SignalMethod = m_InterfaceDescription->GetMember(AJ_DISMISS_SIGNAL_NAME.c_str());
    if (m_SignalMethod == NULL) {
        status = ER_FAIL;
        QCC_LogError(status, ("Could not add interface."));
        return;
    }
}

NotificationDismisser::~NotificationDismisser()
{
}

QStatus NotificationDismisser::Get(const char* ifcName, const char* propName, MsgArg& val)
{
    QCC_DbgPrintf(("Get property was called:"));

    if (0 != strcmp(AJ_PROPERTY_VERSION.c_str(), propName)) {
        QCC_LogError(ER_BUS_NO_SUCH_PROPERTY, ("Called for property different than version."));
        return ER_BUS_NO_SUCH_PROPERTY;
    }

    val.typeId = ALLJOYN_UINT16;
    val.v_uint16 = NOTIFICATION_DISMISSER_VERSION;
    return ER_OK;
}

QStatus NotificationDismisser::Set(const char* ifcName, const char* propName, MsgArg& val)
{
    return ER_ALLJOYN_ACCESS_PERMISSION_ERROR;
}

const char* NotificationDismisser::getAppId() const
{
    return m_AppId;
}
