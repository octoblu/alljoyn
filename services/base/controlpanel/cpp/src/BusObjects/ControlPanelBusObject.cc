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

#include "ControlPanelBusObject.h"
#include "../ControlPanelConstants.h"
#include <alljoyn/controlpanel/ControlPanelService.h>
#include <alljoyn/controlpanel/LogModule.h>

using namespace qcc;
namespace ajn {
namespace services {
using namespace cpsConsts;

#define CONTROLPANEL_INTERFACE_VERSION 1

ControlPanelBusObject::ControlPanelBusObject(BusAttachment* bus, String const& objectPath, QStatus& status) :
    BusObject(objectPath.c_str()), m_Proxy(0), m_ObjectPath(objectPath), m_InterfaceDescription(0),
    m_IsNotificationAction(false), m_SignalDismiss(0)
{
    status = ER_OK;

    String interfaceName = AJ_CONTROLPANEL_INTERFACE;
    m_InterfaceDescription = (InterfaceDescription*) bus->GetInterface(interfaceName.c_str());
    if (!m_InterfaceDescription) {
        do {
            CHECK_AND_BREAK(bus->CreateInterface(interfaceName.c_str(), m_InterfaceDescription));
            CHECK_AND_BREAK(m_InterfaceDescription->AddProperty(AJ_PROPERTY_VERSION.c_str(), AJPARAM_UINT16.c_str(), PROP_ACCESS_READ));
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
    QCC_DbgPrintf(("Created ControlPanelBusObject successfully"));
}

ControlPanelBusObject::~ControlPanelBusObject()
{
    if (m_Proxy) {
        delete m_Proxy;
    }
}

QStatus ControlPanelBusObject::setIsNotificationAction(BusAttachment* bus)
{
    QStatus status = ER_OK;
    String interfaceName = AJ_NOTIFICATIONACTION_INTERFACE;

    InterfaceDescription* interfaceDescription = (InterfaceDescription*) bus->GetInterface(interfaceName.c_str());
    if (!interfaceDescription) {
        do {
            CHECK_AND_BREAK(bus->CreateInterface(interfaceName.c_str(), interfaceDescription));
            CHECK_AND_BREAK(interfaceDescription->AddProperty(AJ_PROPERTY_VERSION.c_str(), AJPARAM_UINT16.c_str(), PROP_ACCESS_READ));
            CHECK_AND_BREAK(interfaceDescription->AddSignal(AJ_SIGNAL_DISMISS.c_str(), AJPARAM_EMPTY.c_str(), AJPARAM_EMPTY.c_str()));
            interfaceDescription->Activate();
        } while (0);
    }
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not create interface"));
        return status;
    }

    status = AddInterface(*interfaceDescription);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not add interface"));
        return status;
    }

    //Get the signal methods for future use
    m_SignalDismiss = interfaceDescription->GetMember(AJ_SIGNAL_DISMISS.c_str());
    QCC_DbgPrintf(("Created NotificationAction interface successfully"));
    m_IsNotificationAction = true;
    return status;
}

QStatus ControlPanelBusObject::SendDismissSignal()
{
    QStatus status = ER_BUS_PROPERTY_VALUE_NOT_SET;
    if (!m_IsNotificationAction) {
        QCC_LogError(status, ("ControlPanel is not set as a Notification with Action"));
        return status;
    }

    ControlPanelBusListener* busListener = ControlPanelService::getInstance()->getBusListener();
    if (!m_SignalDismiss) {
        QCC_DbgHLPrintf(("Can't send Dismiss signal. Signal not set"));
        return status;
    }

    if (!busListener) {
        QCC_DbgHLPrintf(("Can't send valueChanged signal. SessionIds are unknown"));
        return status;
    }

    QCC_DbgPrintf(("Sending dismiss Signal to all connected sessions"));

    const std::vector<SessionId>& sessionIds = busListener->getSessionIds();
    for (size_t indx = 0; indx < sessionIds.size(); indx++) {
        status = Signal(NULL, sessionIds[indx], *m_SignalDismiss, NULL, 0);
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not send Dismiss Signal for sessionId: %s", sessionIds[indx]));
        }
    }
    return status;
}

QStatus ControlPanelBusObject::Get(const char* interfaceName, const char* propName, MsgArg& val)
{
    QCC_DbgPrintf(("Get property was called - in ControlPanelBusObject class."));

    if (0 == strcmp(AJ_PROPERTY_VERSION.c_str(), propName)) {
        return val.Set(AJPARAM_UINT16.c_str(), CONTROLPANEL_INTERFACE_VERSION);
    }
    return ER_BUS_NO_SUCH_PROPERTY;
}

QStatus ControlPanelBusObject::Set(const char* interfaceName, const char* propName, MsgArg& val)
{
    return ER_ALLJOYN_ACCESS_PERMISSION_ERROR;
}

QStatus ControlPanelBusObject::setRemoteController(BusAttachment* bus, qcc::String const& deviceBusName, SessionId sessionId)
{
    if (m_Proxy && m_Proxy->GetSessionId() == sessionId) {
        QCC_DbgPrintf(("ProxyBusObject already set - ignoring"));
        return ER_OK;
    }

    if (!m_InterfaceDescription) {
        QCC_DbgHLPrintf(("InterfaceDescription is not set. Cannot set RemoteController"));
        return ER_FAIL;
    }

    if (m_Proxy) { // delete existing ProxyBusObject and create new one with correct sessionId
        delete m_Proxy;
    }

    m_Proxy = new ProxyBusObject(*bus, deviceBusName.c_str(), m_ObjectPath.c_str(), sessionId);
    QStatus status = m_Proxy->AddInterface(*m_InterfaceDescription);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not add Interface to ProxyBusobject"));
    }
    return status;
}

QStatus ControlPanelBusObject::checkVersions()
{
    if (!m_Proxy) {
        QCC_DbgHLPrintf(("Cannot Check Versions. ProxyBusObject is not set."));
        return ER_BUS_PROPERTY_VALUE_NOT_SET;
    }

    MsgArg value;
    QStatus status = m_Proxy->GetProperty(AJ_CONTROLPANEL_INTERFACE.c_str(), AJ_PROPERTY_VERSION.c_str(), value);
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

    if (CONTROLPANEL_INTERFACE_VERSION < version) {
        QCC_DbgHLPrintf(("The versions of the interface are not compatible"));
        return ER_BUS_INTERFACE_MISMATCH;
    }
    return ER_OK;
}

QStatus ControlPanelBusObject::Introspect(std::vector<IntrospectionNode>& childNodes)
{
    if (!m_Proxy) {
        QCC_DbgHLPrintf(("Cannot Check Versions. ProxyBusObject is not set"));
        return ER_BUS_PROPERTY_VALUE_NOT_SET;
    }

    QStatus status = m_Proxy->IntrospectRemoteObject();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not introspect RemoteObject"));
        return status;
    }

    size_t numChildren = m_Proxy->GetChildren();
    if (numChildren == 0) {
        return ER_OK;
    }

    ProxyBusObject** proxyBusObjectChildren = new ProxyBusObject *[numChildren];
    numChildren = m_Proxy->GetChildren(proxyBusObjectChildren, numChildren);

    for (size_t i = 0; i < numChildren; i++) {

        String const& objectPath = proxyBusObjectChildren[i]->GetPath();
        QCC_DbgPrintf(("ObjectPath is: %s", objectPath.c_str()));

        status = proxyBusObjectChildren[i]->IntrospectRemoteObject();
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not introspect RemoteObjectChild"));
            delete[] proxyBusObjectChildren;
            return status;
        }

        size_t numInterfaces = proxyBusObjectChildren[i]->GetInterfaces();

        if (numInterfaces == 0) {
            continue;
        }

        const InterfaceDescription** ifaces = new const InterfaceDescription *[numInterfaces];
        numInterfaces = proxyBusObjectChildren[i]->GetInterfaces(ifaces, numInterfaces);
        for (size_t j = 0; j < numInterfaces; j++) {
            QCC_DbgPrintf(("InterfaceName is : %s", ifaces[j]->GetName()));
            if (strcmp(ifaces[j]->GetName(), AJ_CONTAINER_INTERFACE.c_str()) == 0) {
                IntrospectionNode node(objectPath, WIDGET_TYPE_CONTAINER, false);
                childNodes.push_back(node);
            } else if (strcmp(ifaces[j]->GetName(), AJ_SECURED_CONTAINER_INTERFACE.c_str()) == 0) {
                IntrospectionNode node(objectPath, WIDGET_TYPE_CONTAINER, true);
                childNodes.push_back(node);
            } else {
                QCC_DbgPrintf(("Ignoring interface - not a container interface"));
            }
        }
        delete[] ifaces;
    }
    delete[] proxyBusObjectChildren;
    return ER_OK;
}

} /* namespace services */
} /* namespace ajn */



