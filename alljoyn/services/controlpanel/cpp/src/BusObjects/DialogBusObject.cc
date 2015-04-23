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

#include "DialogBusObject.h"
#include "../ControlPanelConstants.h"
#include <alljoyn/controlpanel/ControlPanelService.h>
#include <alljoyn/controlpanel/Dialog.h>
#include <alljoyn/controlpanel/LogModule.h>

namespace ajn {
namespace services {
using namespace qcc;
using namespace cpsConsts;

DialogBusObject::DialogBusObject(BusAttachment* bus, String const& objectPath, uint16_t langIndx,
                                 QStatus& status, Widget* widget) :
    WidgetBusObject(objectPath, langIndx, status, widget)
{
    if (status != ER_OK) {
        QCC_DbgHLPrintf(("Could not create the BusObject"));
        return;
    }

    String interfaceName = widget->getIsSecured() ? AJ_SECURED_DIALOG_INTERFACE : AJ_DIALOG_INTERFACE;
    m_InterfaceDescription = (InterfaceDescription*) bus->GetInterface(interfaceName.c_str());
    if (!m_InterfaceDescription) {
        do {
            CHECK_AND_BREAK(bus->CreateInterface(interfaceName.c_str(), m_InterfaceDescription, widget->getIsSecured()));
            CHECK_AND_BREAK(addDefaultInterfaceVariables(m_InterfaceDescription));
            CHECK_AND_BREAK(m_InterfaceDescription->AddProperty(AJ_PROPERTY_MESSAGE.c_str(), AJPARAM_STR.c_str(), PROP_ACCESS_READ));
            CHECK_AND_BREAK(m_InterfaceDescription->AddProperty(AJ_PROPERTY_NUMACTIONS.c_str(), AJPARAM_UINT16.c_str(), PROP_ACCESS_READ));
            CHECK_AND_BREAK(m_InterfaceDescription->AddMethod(AJ_METHOD_ACTION1.c_str(), AJPARAM_EMPTY.c_str(),
                                                              AJPARAM_EMPTY.c_str(), AJPARAM_EMPTY.c_str()));
            CHECK_AND_BREAK(m_InterfaceDescription->AddMethod(AJ_METHOD_ACTION2.c_str(), AJPARAM_EMPTY.c_str(),
                                                              AJPARAM_EMPTY.c_str(), AJPARAM_EMPTY.c_str()));
            CHECK_AND_BREAK(m_InterfaceDescription->AddMethod(AJ_METHOD_ACTION3.c_str(), AJPARAM_EMPTY.c_str(),
                                                              AJPARAM_EMPTY.c_str(), AJPARAM_EMPTY.c_str()));

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
    } else {
        const ajn::InterfaceDescription::Member* execMember1 = m_InterfaceDescription->GetMember(AJ_METHOD_ACTION1.c_str());
        const ajn::InterfaceDescription::Member* execMember2 = m_InterfaceDescription->GetMember(AJ_METHOD_ACTION2.c_str());
        const ajn::InterfaceDescription::Member* execMember3 = m_InterfaceDescription->GetMember(AJ_METHOD_ACTION3.c_str());

        status = AddMethodHandler(execMember1, static_cast<MessageReceiver::MethodHandler>(&DialogBusObject::DialogExecute));
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not register the MethodHandler"));
            return;
        }

        status = AddMethodHandler(execMember2, static_cast<MessageReceiver::MethodHandler>(&DialogBusObject::DialogExecute));
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not register the MethodHandler"));
            return;
        }

        status = AddMethodHandler(execMember3, static_cast<MessageReceiver::MethodHandler>(&DialogBusObject::DialogExecute));
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not register the MethodHandler"));            return;
        }
    }
    QCC_DbgPrintf(("Created DialogBusObject successfully"));
}

DialogBusObject::~DialogBusObject()
{
}

QStatus DialogBusObject::Get(const char* interfaceName, const char* propName, MsgArg& val)
{
    QCC_DbgTrace(("Get property was called - in DialogBusObject class."));
    if (0 == strcmp(AJ_PROPERTY_NUMACTIONS.c_str(), propName)) {
        return ((Dialog*)m_Widget)->fillNumActionArg(val, m_LanguageIndx);
    }

    if (0 == strcmp(AJ_PROPERTY_MESSAGE.c_str(), propName)) {
        return ((Dialog*)m_Widget)->fillMessageArg(val, m_LanguageIndx);
    }

    return WidgetBusObject::Get(interfaceName, propName, val);
}

void DialogBusObject::DialogExecute(const ajn::InterfaceDescription::Member* member, ajn::Message& msg)
{
    QStatus status = ER_OK;
    QCC_DbgTrace(("Execute was called"));

    if (member->name.compare(AJ_METHOD_ACTION1) == 0 &&
        ((Dialog*)m_Widget)->executeAction1CallBack()) {
        MsgArg replyArg;
        status = MethodReply(msg, &replyArg, 0);
        QCC_DbgPrintf(("Execute Action 1 completed successfully"));
    } else if (member->name.compare(AJ_METHOD_ACTION2) == 0 &&
               ((Dialog*)m_Widget)->executeAction2CallBack()) {
        MsgArg replyArg;
        status = MethodReply(msg, &replyArg, 0);
        QCC_DbgPrintf(("Execute Action 2 completed successfully"));
    } else if (member->name.compare(AJ_METHOD_ACTION3) == 0 &&
               ((Dialog*)m_Widget)->executeAction3CallBack()) {
        MsgArg replyArg;
        status = MethodReply(msg, &replyArg, 0);
        QCC_DbgPrintf(("Execute Action 3 completed successfully"));
    } else {
        QCC_DbgHLPrintf(("Unknown Execute Action was called, or action did not complete successfully"));
        status = MethodReply(msg, AJ_ERROR_UNKNOWN.c_str(), AJ_ERROR_UNKNOWN_MESSAGE.c_str());
    }

    if (ER_OK != status) {
        QCC_LogError(status, ("Did not reply successfully"));
    }
}

QStatus DialogBusObject::ExecuteAction1()
{
    if (!m_Proxy) {
        QCC_DbgHLPrintf(("Cannot execute the Action 1. ProxyBusObject is not set"));
        return ER_BUS_PROPERTY_VALUE_NOT_SET;
    }

    const ajn::InterfaceDescription::Member* execMember = m_InterfaceDescription->GetMember(AJ_METHOD_ACTION1.c_str());
    if (!execMember) {
        QCC_DbgHLPrintf(("Cannot execute the Action 1. ExecMember is not set"));
        return ER_BUS_PROPERTY_VALUE_NOT_SET;
    }

    Message replyMsg(*bus);
    QStatus status = m_Proxy->MethodCall(*execMember, NULL, 0, replyMsg);
    if (status != ER_OK) {
        QCC_DbgHLPrintf(("Call to execute the Action 1 failed"));
    }
    return status;
}

QStatus DialogBusObject::ExecuteAction2()
{
    if (!m_Proxy) {
        QCC_DbgHLPrintf(("Cannot execute the Action 2. ProxyBusObject is not set"));
        return ER_BUS_PROPERTY_VALUE_NOT_SET;
    }

    const ajn::InterfaceDescription::Member* execMember = m_InterfaceDescription->GetMember(AJ_METHOD_ACTION2.c_str());
    if (!execMember) {
        QCC_DbgHLPrintf(("Cannot execute the Action 2. ExecMember is not set"));
        return ER_BUS_PROPERTY_VALUE_NOT_SET;
    }

    Message replyMsg(*bus);
    QStatus status = m_Proxy->MethodCall(*execMember, NULL, 0, replyMsg);
    if (status != ER_OK) {
        QCC_DbgHLPrintf(("Call to execute the Action 2 failed"));
    }
    return status;
}

QStatus DialogBusObject::ExecuteAction3()
{
    if (!m_Proxy) {
        QCC_DbgHLPrintf(("Cannot execute the Action 3. ProxyBusObject is not set"));
        return ER_BUS_PROPERTY_VALUE_NOT_SET;
    }

    const ajn::InterfaceDescription::Member* execMember = m_InterfaceDescription->GetMember(AJ_METHOD_ACTION3.c_str());
    if (!execMember) {
        QCC_DbgHLPrintf(("Cannot execute the Action 3. ExecMember is not set"));
        return ER_BUS_PROPERTY_VALUE_NOT_SET;
    }

    Message replyMsg(*bus);
    QStatus status = m_Proxy->MethodCall(*execMember, NULL, 0, replyMsg);
    if (status != ER_OK) {
        QCC_DbgHLPrintf(("Call to execute the Action 3 failed"));
    }
    return status;
}

QStatus DialogBusObject::fillProperty(char* key, MsgArg* variant)
{
    QStatus status = ER_BUS_NO_SUCH_PROPERTY;

    if (strcmp(key, AJ_PROPERTY_MESSAGE.c_str()) == 0) {
        CHECK_AND_RETURN(((Dialog*)m_Widget)->readMessageArg(variant))
    } else if (strcmp(key, AJ_PROPERTY_NUMACTIONS.c_str()) == 0) {
        CHECK_AND_RETURN(((Dialog*)m_Widget)->readNumActionsArg(variant))
    }

    return status;
}

} /* namespace services */
} /* namespace ajn */



