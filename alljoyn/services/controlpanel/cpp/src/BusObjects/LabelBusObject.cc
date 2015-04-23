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

#include "LabelBusObject.h"
#include "../ControlPanelConstants.h"
#include <alljoyn/controlpanel/ControlPanelService.h>
#include <alljoyn/controlpanel/Label.h>
#include <alljoyn/controlpanel/LogModule.h>

namespace ajn {
namespace services {
using namespace qcc;
using namespace cpsConsts;

LabelBusObject::LabelBusObject(BusAttachment* bus, String const& objectPath, uint16_t langIndx,
                               QStatus& status, Widget* widget) : WidgetBusObject(objectPath, langIndx, status, widget)
{
    if (status != ER_OK) {
        QCC_DbgHLPrintf(("Could not create the BusObject"));
        return;
    }

    m_InterfaceDescription = (InterfaceDescription*) bus->GetInterface(AJ_LABEL_INTERFACE.c_str());
    if (!m_InterfaceDescription) {
        do {
            CHECK_AND_BREAK(bus->CreateInterface(AJ_LABEL_INTERFACE.c_str(), m_InterfaceDescription));
            CHECK_AND_BREAK(addDefaultInterfaceVariables(m_InterfaceDescription));
            CHECK_AND_BREAK(m_InterfaceDescription->AddProperty(AJ_PROPERTY_LABEL.c_str(), AJPARAM_STR.c_str(), PROP_ACCESS_READ));
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
    QCC_DbgPrintf(("Created LabelBusObject successfully"));
}

LabelBusObject::~LabelBusObject() {
}

QStatus LabelBusObject::Get(const char* interfaceName, const char* propName, MsgArg& val)
{
    QCC_DbgTrace(("Get property was called - in LabelBusObject class."));

    if (0 == strcmp(AJ_PROPERTY_LABEL.c_str(), propName)) {
        return ((Label*)m_Widget)->fillLabelArg(val, m_LanguageIndx);
    }

    return WidgetBusObject::Get(interfaceName, propName, val);
}

QStatus LabelBusObject::fillProperty(char* key, MsgArg* variant)
{
    QStatus status = ER_BUS_NO_SUCH_PROPERTY;

    if (strcmp(key, AJ_PROPERTY_LABEL.c_str()) == 0) {
        CHECK_AND_RETURN(((Label*)m_Widget)->readLabelArg(variant))
    }
    return status;
}

} /* namespace services */
} /* namespace ajn */




