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

#include "PropertyBusObject.h"
#include "../ControlPanelConstants.h"
#include <alljoyn/controlpanel/ControlPanelService.h>
#include <alljoyn/controlpanel/Property.h>
#include <alljoyn/controlpanel/LogModule.h>

namespace ajn {
namespace services {
using namespace qcc;
using namespace cpsConsts;

PropertyBusObject::PropertyBusObject(BusAttachment* bus, String const& objectPath, uint16_t langIndx,
                                     QStatus& status, Widget* widget) :
    WidgetBusObject(objectPath, langIndx, status, widget), m_SignalValueChanged(0)
{
    if (status != ER_OK) {
        QCC_DbgHLPrintf(("Could not create the BusObject"));
        return;
    }

    String interfaceName = widget->getIsSecured() ? AJ_SECURED_PROPERTY_INTERFACE : AJ_PROPERTY_INTERFACE;
    m_InterfaceDescription = (InterfaceDescription*) bus->GetInterface(interfaceName.c_str());
    if (!m_InterfaceDescription) {
        do {
            CHECK_AND_BREAK(bus->CreateInterface(interfaceName.c_str(), m_InterfaceDescription, widget->getIsSecured()));
            CHECK_AND_BREAK(addDefaultInterfaceVariables(m_InterfaceDescription));
            CHECK_AND_BREAK(m_InterfaceDescription->AddProperty(AJ_PROPERTY_VALUE.c_str(), AJPARAM_VAR.c_str(), PROP_ACCESS_RW));
            CHECK_AND_BREAK(m_InterfaceDescription->AddSignal(AJ_SIGNAL_VALUE_CHANGED.c_str(), AJPARAM_VAR.c_str(), AJ_PROPERTY_VALUE.c_str(), 0));
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
    m_SignalValueChanged = m_InterfaceDescription->GetMember(AJ_SIGNAL_VALUE_CHANGED.c_str());
    if (widget->getControlPanelMode() == CONTROLLER_MODE) {
        status = addSignalHandler(bus);
    }
    QCC_DbgPrintf(("Created PropertyBusObject successfully"));
}

PropertyBusObject::~PropertyBusObject()
{
}

QStatus PropertyBusObject::addSignalHandler(BusAttachment* bus)
{
    QStatus status;
    CHECK_AND_RETURN(WidgetBusObject::addSignalHandler(bus))
    status = bus->RegisterSignalHandler(this, static_cast<MessageReceiver::SignalHandler>(&PropertyBusObject::ValueChanged),
                                        m_SignalValueChanged, m_ObjectPath.c_str());
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not register the SignalHandler"));
    }
    return status;
}

QStatus PropertyBusObject::UnregisterSignalHandler(BusAttachment* bus)
{
    WidgetBusObject::UnregisterSignalHandler(bus);
    QStatus status = bus->UnregisterSignalHandler(this,
                                                  static_cast<MessageReceiver::SignalHandler>(&PropertyBusObject::ValueChanged),
                                                  m_SignalValueChanged, m_ObjectPath.c_str());
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not unregister the SignalHandler"));
    }
    return status;
}

QStatus PropertyBusObject::Get(const char* interfaceName, const char* propName, MsgArg& val)
{
    QCC_DbgTrace(("Get property was called - in PropertyBusObject class."));

    if (0 == strcmp(AJ_PROPERTY_VALUE.c_str(), propName)) {
        return ((Property*)m_Widget)->fillPropertyValueArg(val, m_LanguageIndx);
    }

    return WidgetBusObject::Get(interfaceName, propName, val);
}

QStatus PropertyBusObject::Set(const char* interfaceName, const char* propName, MsgArg& val)
{
    QCC_DbgTrace(("Set property was called - in PropertyBusObject class."));

    if (0 != strcmp(AJ_PROPERTY_VALUE.c_str(), propName)) {
        return ER_BUS_PROPERTY_ACCESS_DENIED;
    }

    return ((Property*)m_Widget)->setPropertyValue(val, m_LanguageIndx);
}

QStatus PropertyBusObject::SetValue(MsgArg& value)
{
    QCC_DbgTrace(("Set Value was called"));

    if (!m_Proxy) {
        QCC_DbgHLPrintf(("Cannot set the Value. ProxyBusObject is not set"));
        return ER_BUS_PROPERTY_VALUE_NOT_SET;
    }

    if (!m_InterfaceDescription) {
        QCC_DbgHLPrintf(("Cannot set the Value. InterfaceDescription is not set"));
        return ER_BUS_PROPERTY_VALUE_NOT_SET;
    }

    QStatus status = m_Proxy->SetProperty(m_InterfaceDescription->GetName(), AJ_PROPERTY_VALUE.c_str(), value);
    if (status != ER_OK) {
        QCC_LogError(status, ("Call to set Property failed"));
    }

    return status;
}

QStatus PropertyBusObject::SendValueChangedSignal()
{
    ControlPanelBusListener* busListener = ControlPanelService::getInstance()->getBusListener();

    if (!m_SignalValueChanged) {
        QCC_DbgHLPrintf(("Can't send valueChanged signal. Signal not set"));
        return ER_BUS_PROPERTY_VALUE_NOT_SET;
    }

    if (!busListener) {
        QCC_DbgHLPrintf(("Can't send valueChanged signal. SessionIds are unknown"));
        return ER_BUS_PROPERTY_VALUE_NOT_SET;
    }

    QStatus status;
    MsgArg value;

    CHECK_AND_RETURN(((Property*)m_Widget)->fillPropertyValueArg(value, m_LanguageIndx));

    const std::vector<SessionId>& sessionIds = busListener->getSessionIds();
    for (size_t indx = 0; indx < sessionIds.size(); indx++) {
        status = Signal(NULL, sessionIds[indx], *m_SignalValueChanged, &value, 1);
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not send ValueChanged Signal for sessionId: %s", sessionIds[indx]));
        }
    }
    return status;
}

void PropertyBusObject::ValueChanged(const InterfaceDescription::Member* member, const char* srcPath, Message& msg)
{
    if (msg.unwrap()->GetSender() && strcmp(msg.unwrap()->GetSender(), m_Widget->getDevice()->getDeviceBusName().c_str()) != 0) {
        QCC_DbgPrintf(("Received ValueChanged signal for someone else"));
        return;
    }

    QCC_DbgPrintf(("Received ValueChanged signal"));

    return ((Property*)m_Widget)->ValueChanged(msg);
}

QStatus PropertyBusObject::fillProperty(char* key, MsgArg* variant)
{
    QStatus status = ER_BUS_NO_SUCH_PROPERTY;

    if (strcmp(key, AJ_PROPERTY_VALUE.c_str()) == 0) {
        CHECK_AND_RETURN(((Property*)m_Widget)->readValueArg(variant))

        return status;
    }
    return status;
}

} /* namespace services */
} /* namespace ajn */

