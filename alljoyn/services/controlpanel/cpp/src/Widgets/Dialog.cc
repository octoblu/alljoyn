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

#include <alljoyn/controlpanel/Dialog.h>
#include <alljoyn/controlpanel/ControlPanelService.h>
#include "../ControlPanelConstants.h"
#include "../BusObjects/DialogBusObject.h"
#include <alljoyn/controlpanel/LogModule.h>

namespace ajn {
namespace services {
using namespace cpsConsts;

Dialog::Dialog(qcc::String const& name, Widget* rootWidget) : RootWidget(name, rootWidget, WIDGET_TYPE_DIALOG),
    m_NumActions(0), m_GetMessages(0), m_LabelAction1(""), m_GetLabelsAction1(0), m_LabelAction2(""),
    m_GetLabelsAction2(0), m_LabelAction3(""), m_GetLabelsAction3(0)
{
}

Dialog::Dialog(qcc::String const& name, Widget* rootWidget, qcc::String const& objectPath, ControlPanelDevice* device) :
    RootWidget(name, rootWidget, objectPath, device, WIDGET_TYPE_DIALOG), m_NumActions(0), m_GetMessages(0), m_LabelAction1(""),
    m_GetLabelsAction1(0), m_LabelAction2(""), m_GetLabelsAction2(0), m_LabelAction3(""), m_GetLabelsAction3(0)
{
}

Dialog::Dialog(qcc::String const& name, Widget* rootWidget, ControlPanelDevice* device) :
    RootWidget(name, rootWidget, "", device, WIDGET_TYPE_DIALOG), m_NumActions(0), m_GetMessages(0), m_LabelAction1(""),
    m_GetLabelsAction1(0), m_LabelAction2(""), m_GetLabelsAction2(0), m_LabelAction3(""), m_GetLabelsAction3(0)
{
}

Dialog::~Dialog()
{
}

WidgetBusObject* Dialog::createWidgetBusObject(BusAttachment* bus, qcc::String const& objectPath,
                                               uint16_t langIndx, QStatus& status)
{
    return new DialogBusObject(bus, objectPath, langIndx, status, this);
}

const uint16_t Dialog::getNumActions() const
{
    return m_NumActions;
}

void Dialog::setNumActions(const uint16_t numActions)
{
    m_NumActions = numActions;
}

const qcc::String& Dialog::getMessage() const
{
    return m_Message;
}

const std::vector<qcc::String>& Dialog::getMessages() const
{
    return m_Messages;
}

void Dialog::setMessages(const std::vector<qcc::String>& messages)
{
    m_Messages = messages;
}

GetStringFptr Dialog::getGetMessages() const
{
    return m_GetMessages;
}

void Dialog::setGetMessages(GetStringFptr getMessages)
{
    m_GetMessages = getMessages;
}

const qcc::String& Dialog::getLabelAction1() const
{
    return m_LabelAction1;
}

const qcc::String& Dialog::getLabelAction2() const
{
    return m_LabelAction2;
}

const qcc::String& Dialog::getLabelAction3() const
{
    return m_LabelAction3;
}

const std::vector<qcc::String>& Dialog::getLabelsAction1() const
{
    return m_LabelsAction1;
}

void Dialog::setLabelsAction1(const std::vector<qcc::String>& labelsAction1)
{
    m_LabelsAction1 = labelsAction1;
}

GetStringFptr Dialog::getGetLabelsAction1() const
{
    return m_GetLabelsAction1;
}

void Dialog::setGetLabelsAction1(GetStringFptr getLabelsAction1)
{
    m_GetLabelsAction1 = getLabelsAction1;
}

const std::vector<qcc::String>& Dialog::getLabelsAction2() const
{
    return m_LabelsAction2;
}

void Dialog::setLabelsAction2(const std::vector<qcc::String>& labelsAction2)
{
    m_LabelsAction2 = labelsAction2;
}

GetStringFptr Dialog::getGetLabelsAction2() const
{
    return m_GetLabelsAction2;
}

void Dialog::setGetLabelsAction2(GetStringFptr getLabelsAction2)
{
    m_GetLabelsAction2 = getLabelsAction2;
}

const std::vector<qcc::String>& Dialog::getLabelsAction3() const
{
    return m_LabelsAction3;
}

void Dialog::setLabelsAction3(const std::vector<qcc::String>& labelsAction3)
{
    m_LabelsAction3 = labelsAction3;
}

QStatus Dialog::fillMessageArg(MsgArg& val, uint16_t languageIndx)
{
    if (!(m_Messages.size() > languageIndx) && !m_GetMessages) {
        return ER_BUS_PROPERTY_VALUE_NOT_SET;
    }

    return val.Set(AJPARAM_STR.c_str(), m_GetMessages ? m_GetMessages(languageIndx) :
                   m_Messages[languageIndx].c_str());
}

QStatus Dialog::fillNumActionArg(MsgArg& val, uint16_t languageIndx)
{
    if (!m_NumActions) {
        return ER_BUS_PROPERTY_VALUE_NOT_SET;
    }

    return val.Set(AJPARAM_UINT16.c_str(), m_NumActions);
}

GetStringFptr Dialog::getGetLabelsAction3() const
{
    return m_GetLabelsAction3;
}

void Dialog::setGetLabelsAction3(GetStringFptr getLabelAction3)
{
    m_GetLabelsAction3 = getLabelAction3;
}

QStatus Dialog::fillOptParamsArg(MsgArg& val, uint16_t languageIndx)
{
    QStatus status;
    MsgArg* optParams = new MsgArg[OPT_PARAM_KEYS::NUM_OPT_PARAMS];

    size_t optParamIndx = 0;

    status = Widget::fillOptParamsArg(optParams, languageIndx, optParamIndx);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not marshal optParams"));
        delete[] optParams;
        return status;
    }

    if (m_LabelsAction1.size() > languageIndx || m_GetLabelsAction1) {
        MsgArg* labelArg = new MsgArg(AJPARAM_STR.c_str(), m_GetLabelsAction1 ?
                                      m_GetLabelsAction1(languageIndx) : m_LabelsAction1[languageIndx].c_str());

        if ((status = optParams[optParamIndx].Set(AJPARAM_DICT_UINT16_VAR.c_str(),
                                                  OPT_PARAM_KEYS::LABEL_ACTION1, labelArg)) != ER_OK) {
            delete labelArg;
            delete[] optParams;
            return status;
        }
        optParams[optParamIndx++].SetOwnershipFlags(MsgArg::OwnsArgs, true);
    }

    if (m_LabelsAction2.size() > languageIndx || m_GetLabelsAction2) {
        MsgArg* labelArg = new MsgArg(AJPARAM_STR.c_str(), m_GetLabelsAction2 ?
                                      m_GetLabelsAction2(languageIndx) : m_LabelsAction2[languageIndx].c_str());

        if ((status = optParams[optParamIndx].Set(AJPARAM_DICT_UINT16_VAR.c_str(),
                                                  OPT_PARAM_KEYS::LABEL_ACTION2, labelArg)) != ER_OK) {
            delete labelArg;
            delete[] optParams;
            return status;
        }
        optParams[optParamIndx++].SetOwnershipFlags(MsgArg::OwnsArgs, true);
    }

    if (m_LabelsAction3.size() > languageIndx || m_GetLabelsAction3) {
        MsgArg* labelArg = new MsgArg(AJPARAM_STR.c_str(), m_GetLabelsAction3 ?
                                      m_GetLabelsAction3(languageIndx) : m_LabelsAction3[languageIndx].c_str());

        if ((status = optParams[optParamIndx].Set(AJPARAM_DICT_UINT16_VAR.c_str(),
                                                  OPT_PARAM_KEYS::LABEL_ACTION3, labelArg)) != ER_OK) {
            delete labelArg;
            delete[] optParams;
            return status;
        }
        optParams[optParamIndx++].SetOwnershipFlags(MsgArg::OwnsArgs, true);
    }

    return val.Set(AJPARAM_ARRAY_DICT_UINT16_VAR.c_str(), optParamIndx, optParams);
}

QStatus Dialog::readMessageArg(MsgArg* val)
{
    QStatus status = ER_OK;
    char* message;
    CHECK_AND_RETURN(val->Get(AJPARAM_STR.c_str(), &message))
    m_Message = message;
    return status;
}

QStatus Dialog::readNumActionsArg(MsgArg* val)
{
    QStatus status = ER_OK;
    uint16_t numActions;
    CHECK_AND_RETURN(val->Get(AJPARAM_UINT16.c_str(), &numActions))
    m_NumActions = numActions;
    return status;
}

QStatus Dialog::readOptParamsArg(uint16_t key, MsgArg* val)
{
    QStatus status = ER_BUS_NO_SUCH_PROPERTY;
    switch (key) {
    case OPT_PARAM_KEYS::LABEL_ACTION1:
        {
            char* label;
            CHECK_AND_RETURN(val->Get(AJPARAM_STR.c_str(), &label))
            m_LabelAction1 = label;
            break;
        }

    case OPT_PARAM_KEYS::LABEL_ACTION2:
        {
            char* label;
            CHECK_AND_RETURN(val->Get(AJPARAM_STR.c_str(), &label))
            m_LabelAction2 = label;
            break;
        }

    case OPT_PARAM_KEYS::LABEL_ACTION3:
        {
            char* label;
            CHECK_AND_RETURN(val->Get(AJPARAM_STR.c_str(), &label))
            m_LabelAction3 = label;
            break;
        }

    default:
        status = Widget::readOptParamsArg(key, val);
    }

    return status;
}

bool Dialog::executeActionNotDefined()
{
    QCC_DbgHLPrintf(("Could not execute this Action. It is not defined."));
    return false;
}

QStatus Dialog::executeAction1()
{
    if (m_ControlPanelMode == CONTROLLEE_MODE) {
        QCC_DbgHLPrintf(("Cannot execute Action 1. Widget is a Controllee widget"));
        return ER_NOT_IMPLEMENTED;
    }

    if (!m_BusObjects.size()) {
        QCC_DbgHLPrintf(("Cannot execute Action 1. BusObject is not set"));
        return ER_BUS_BUS_NOT_STARTED;
    }

    if (m_NumActions < 1) {
        QCC_DbgHLPrintf(("Cannot execute Action 1 - numActions is less than 1"));
        return ER_NOT_IMPLEMENTED;
    }

    return ((DialogBusObject*)m_BusObjects[0])->ExecuteAction1();
}

QStatus Dialog::executeAction2()
{
    if (m_ControlPanelMode == CONTROLLEE_MODE) {
        QCC_DbgHLPrintf(("Cannot execute Action 2. Widget is a Controllee widget"));
        return ER_NOT_IMPLEMENTED;
    }

    if (!m_BusObjects.size()) {
        QCC_DbgHLPrintf(("Cannot execute Action 2. BusObject is not set"));
        return ER_BUS_BUS_NOT_STARTED;
    }

    if (m_NumActions < 2) {
        QCC_DbgHLPrintf(("Cannot execute Action2 - numActions is less than 2"));
        return ER_NOT_IMPLEMENTED;
    }

    return ((DialogBusObject*)m_BusObjects[0])->ExecuteAction2();
}

QStatus Dialog::executeAction3()
{
    if (m_ControlPanelMode == CONTROLLEE_MODE) {
        QCC_DbgHLPrintf(("Cannot execute Action 3. Widget is a Controllee widget"));
        return ER_NOT_IMPLEMENTED;
    }

    if (!m_BusObjects.size()) {
        QCC_DbgHLPrintf(("Cannot execute Action 3. BusObject is not set"));
        return ER_BUS_BUS_NOT_STARTED;
    }

    if (m_NumActions < 3) {
        QCC_DbgHLPrintf(("Cannot execute Action3 - numActions is less than 3"));
        return ER_NOT_IMPLEMENTED;
    }

    return ((DialogBusObject*)m_BusObjects[0])->ExecuteAction3();
}

bool Dialog::executeAction1CallBack()
{
    return false;
}

bool Dialog::executeAction2CallBack()
{
    return false;
}

bool Dialog::executeAction3CallBack()
{
    return false;
}

} /* namespace services */
} /* namespace ajn */
