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

#include "ContainerBusObject.h"
#include "../ControlPanelConstants.h"
#include <alljoyn/controlpanel/ControlPanelService.h>
#include <alljoyn/controlpanel/LogModule.h>

namespace ajn {
namespace services {
using namespace qcc;
using namespace cpsConsts;

ContainerBusObject::ContainerBusObject(BusAttachment* bus, String const& objectPath, uint16_t langIndx,
                                       QStatus& status, Widget* widget) :
    WidgetBusObject(objectPath, langIndx, status, widget)
{
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not create the BusObject"));
        return;
    }

    String interfaceName = widget->getIsSecured() ? AJ_SECURED_CONTAINER_INTERFACE : AJ_CONTAINER_INTERFACE;
    m_InterfaceDescription = (InterfaceDescription*) bus->GetInterface(interfaceName.c_str());
    if (!m_InterfaceDescription) {
        do {
            CHECK_AND_BREAK(bus->CreateInterface(interfaceName.c_str(), m_InterfaceDescription, widget->getIsSecured()));
            CHECK_AND_BREAK(addDefaultInterfaceVariables(m_InterfaceDescription));
            m_InterfaceDescription->Activate();
        } while (0);
    }
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not create interface"));
        return;
    }

    status = AddInterface(*m_InterfaceDescription);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not add interface"));
        return;
    }

    //Get the signal methods for future use
    m_SignalPropertyChanged = m_InterfaceDescription->GetMember(AJ_SIGNAL_PROPERTIES_CHANGED.c_str());
    if (widget->getControlPanelMode() == CONTROLLER_MODE) {
        status = addSignalHandler(bus);
    }

    QCC_DbgPrintf(("Created ContainerBusObject successfully"));
}

ContainerBusObject::~ContainerBusObject() {
}

QStatus ContainerBusObject::Introspect(std::vector<IntrospectionNode>& childNodes)
{
    if (!m_Proxy) {
        QCC_LogError(ER_BUS_PROPERTY_VALUE_NOT_SET, ("Cannot Check Versions. ProxyBusObject is not set"));
        return ER_BUS_PROPERTY_VALUE_NOT_SET;
    }

    QStatus status = m_Proxy->IntrospectRemoteObject();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not introspect RemoteObject"));
        return status;
    }

    size_t numChildren = m_Proxy->GetChildren();
    if (numChildren == 0) {
        QCC_LogError(ER_FAIL, ("Container does not have children"));
        return ER_FAIL;
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
            } else if (strcmp(ifaces[j]->GetName(), AJ_PROPERTY_INTERFACE.c_str()) == 0) {
                IntrospectionNode node(objectPath, WIDGET_TYPE_PROPERTY, false);
                childNodes.push_back(node);
            } else if (strcmp(ifaces[j]->GetName(), AJ_SECURED_PROPERTY_INTERFACE.c_str()) == 0) {
                IntrospectionNode node(objectPath, WIDGET_TYPE_PROPERTY, true);
                childNodes.push_back(node);
            } else if (strcmp(ifaces[j]->GetName(), AJ_LABEL_INTERFACE.c_str()) == 0) {
                IntrospectionNode node(objectPath, WIDGET_TYPE_LABEL, false);
                childNodes.push_back(node);
            } else if (strcmp(ifaces[j]->GetName(), AJ_ACTION_INTERFACE.c_str()) == 0) {
                if (proxyBusObjectChildren[i]->GetChildren()) {
                    IntrospectionNode node(objectPath, WIDGET_TYPE_ACTION_WITH_DIALOG, false);
                    childNodes.push_back(node);
                } else {
                    IntrospectionNode node(objectPath, WIDGET_TYPE_ACTION, false);
                    childNodes.push_back(node);
                }
            } else if (strcmp(ifaces[j]->GetName(), AJ_SECURED_ACTION_INTERFACE.c_str()) == 0) {
                if (proxyBusObjectChildren[i]->GetChildren()) {
                    IntrospectionNode node(objectPath, WIDGET_TYPE_ACTION_WITH_DIALOG, true);
                    childNodes.push_back(node);
                } else {
                    IntrospectionNode node(objectPath, WIDGET_TYPE_ACTION, true);
                    childNodes.push_back(node);
                }
            } else {
                QCC_DbgPrintf(("Ignoring interfaceName: %s", ifaces[j]->GetName()));
            }
        }
        delete[] ifaces;
    }
    delete[] proxyBusObjectChildren;
    return ER_OK;
}

} /* namespace services */
} /* namespace ajn */



