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

#include "HttpControlBusObject.h"
#include "../ControlPanelConstants.h"
#include <alljoyn/controlpanel/ControlPanelService.h>
#include <alljoyn/controlpanel/HttpControl.h>
#include <alljoyn/controlpanel/LogModule.h>

namespace ajn {
namespace services {
using namespace qcc;
using namespace cpsConsts;

#define HTTP_INTERFACE_VERSION 1

HttpControlBusObject::HttpControlBusObject(BusAttachment* bus, String const& objectPath,
                                           QStatus& status, HttpControl* httpControl) :
    BusObject(objectPath.c_str()), m_HttpControl(httpControl), m_Proxy(0), m_ObjectPath(objectPath), m_InterfaceDescription(0)
{
    if (!httpControl) {
        QCC_DbgHLPrintf(("HttpControl cannot be NULL"));
        status = ER_BAD_ARG_4;
        return;
    }

    m_InterfaceDescription = (InterfaceDescription*) bus->GetInterface(AJ_HTTPCONTROL_INTERFACE.c_str());
    if (!m_InterfaceDescription) {
        do {
            CHECK_AND_BREAK(bus->CreateInterface(AJ_HTTPCONTROL_INTERFACE.c_str(), m_InterfaceDescription, false));
            CHECK_AND_BREAK(m_InterfaceDescription->AddProperty(AJ_PROPERTY_VERSION.c_str(), AJPARAM_UINT16.c_str(), PROP_ACCESS_READ));
            CHECK_AND_BREAK(m_InterfaceDescription->AddMethod(AJ_METHOD_GETROOTURL.c_str(), AJPARAM_EMPTY.c_str(),
                                                              AJPARAM_STR.c_str(), AJ_PROPERTY_URL.c_str()));
            m_InterfaceDescription->Activate();
        } while (0);
    }
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not create interface"));
        return;
    }

    status = AddInterface(*m_InterfaceDescription, ANNOUNCED);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not add interface"));
        return;
    }

    if (m_HttpControl->getControlPanelMode() == CONTROLLEE_MODE) {
        //Get the signal methods for future use
        const ajn::InterfaceDescription::Member* getRootUrlMember = m_InterfaceDescription->GetMember(AJ_METHOD_GETROOTURL.c_str());

        status = AddMethodHandler(getRootUrlMember, static_cast<MessageReceiver::MethodHandler>(&HttpControlBusObject::HttpControlGetUrl));
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not register the MethodHandler"));
            return;
        }
    }
    QCC_DbgPrintf(("Created HttpControlBusObject successfully"));
}

HttpControlBusObject::~HttpControlBusObject()
{
}

uint16_t HttpControlBusObject::getInterfaceVersion()
{
    return HTTP_INTERFACE_VERSION;
}

QStatus HttpControlBusObject::Get(const char* interfaceName, const char* propName, MsgArg& val)
{
    QCC_DbgTrace(("Get property was called - in HttpControlBusObject class."));

    if (0 == strcmp(AJ_PROPERTY_VERSION.c_str(), propName)) {
        return val.Set(AJPARAM_UINT16.c_str(), getInterfaceVersion());
    }

    return ER_BUS_NO_SUCH_PROPERTY;
}

QStatus HttpControlBusObject::Set(const char* interfaceName, const char* propName, MsgArg& val)
{
    return ER_ALLJOYN_ACCESS_PERMISSION_ERROR;
}

void HttpControlBusObject::HttpControlGetUrl(const ajn::InterfaceDescription::Member* member, ajn::Message& msg)
{
    QCC_DbgTrace(("Get Url was called"));

    MsgArg url;
    QStatus status = m_HttpControl->fillUrlArg(url);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not set Url"));
        MethodReply(msg, AJ_ERROR_UNKNOWN.c_str(), AJ_ERROR_UNKNOWN_MESSAGE.c_str());
        return;
    }

    status = MethodReply(msg, &url, 1);
    if (ER_OK != status) {
        QCC_LogError(status, ("Did not reply successfully"));
    } else {
        QCC_DbgPrintf(("Replied to GetUrl successfully"));
    }
}

QStatus HttpControlBusObject::setRemoteController(BusAttachment* bus, qcc::String const& deviceBusName, SessionId sessionId)
{
    if (m_Proxy && m_Proxy->GetSessionId() == sessionId) {
        QCC_DbgPrintf(("ProxyBusObject already set - ignoring"));
        return ER_OK;
    }

    if (!m_InterfaceDescription) {
        QCC_DbgHLPrintf(("InterfaceDescription is not set. Cannot set RemoteController"));
        return ER_FAIL;
    }

    if (m_Proxy) { // delete ProxyBusObject before creating a new one with correct sessionId
        delete m_Proxy;
    }

    m_Proxy = new ProxyBusObject(*bus, deviceBusName.c_str(), m_ObjectPath.c_str(), sessionId);
    QStatus status = m_Proxy->AddInterface(*m_InterfaceDescription);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not add Interface to ProxyBusobject"));
    }
    return status;
}

QStatus HttpControlBusObject::checkVersions()
{
    if (!m_Proxy) {
        QCC_DbgHLPrintf(("Cannot Check Versions. ProxyBusObject is not set"));
        return ER_BUS_PROPERTY_VALUE_NOT_SET;
    }

    MsgArg value;
    QStatus status = m_Proxy->GetProperty(m_InterfaceDescription->GetName(), AJ_PROPERTY_VERSION.c_str(), value);
    if (status != ER_OK) {
        QCC_LogError(status, ("Call to get Version Property failed"));
        return status;
    }

    uint16_t version = 1;
    status = value.Get(AJPARAM_UINT16.c_str(), &version);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not unmarshal version property"));
        return status;
    }

    if (getInterfaceVersion() < version) {
        QCC_DbgHLPrintf(("The versions of the interface are not compatible"));
        return ER_BUS_INTERFACE_MISMATCH;
    }

    QStatus setVersionStatus = m_HttpControl->readVersionArg(value);
    if (setVersionStatus != ER_OK) {
        QCC_LogError(status, ("Could not set version property"));
    }
    return status;
}

QStatus HttpControlBusObject::GetUrl(BusAttachment* bus)
{
    if (!m_Proxy) {
        QCC_DbgHLPrintf(("Cannot get the Url. ProxyBusObject is not set"));
        return ER_BUS_PROPERTY_VALUE_NOT_SET;
    }

    const ajn::InterfaceDescription::Member* getRootUrlMember = m_InterfaceDescription->GetMember(AJ_METHOD_GETROOTURL.c_str());
    if (!getRootUrlMember) {
        QCC_DbgHLPrintf(("Cannot get the Url. RootUrlMember is not set"));
        return ER_BUS_PROPERTY_VALUE_NOT_SET;
    }

    Message replyMsg(*bus);
    QStatus status = m_Proxy->MethodCall(*getRootUrlMember, NULL, 0, replyMsg);
    if (status != ER_OK) {
        QCC_LogError(status, ("Call to getUrl failed"));
        return status;
    }

    const ajn::MsgArg* returnArgs;
    size_t numArgs;
    replyMsg->GetArgs(numArgs, returnArgs);
    if (numArgs != 1) {
        QCC_DbgHLPrintf(("Received unexpected amount of returnArgs"));
        return ER_BUS_UNEXPECTED_SIGNATURE;
    }

    status = m_HttpControl->readUrlArg(returnArgs[0]);
    return status;
}

} /* namespace services */
} /* namespace ajn */



