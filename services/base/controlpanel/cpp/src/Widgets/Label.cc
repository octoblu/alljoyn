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

#include <alljoyn/controlpanel/Label.h>
#include "../ControlPanelConstants.h"
#include "../BusObjects/LabelBusObject.h"

namespace ajn {
namespace services {
using namespace cpsConsts;

Label::Label(qcc::String const& name, Widget* rootWidget) :
    Widget(name, rootWidget, WIDGET_TYPE_LABEL), m_LabelWidgetGetLabels(0)
{
}

Label::Label(qcc::String const& name, Widget* rootWidget, ControlPanelDevice* device) :
    Widget(name, rootWidget, device, WIDGET_TYPE_LABEL), m_LabelWidgetGetLabels(0)
{
}

Label::~Label()
{
}

WidgetBusObject* Label::createWidgetBusObject(BusAttachment* bus, qcc::String const& objectPath,
                                              uint16_t langIndx, QStatus& status)
{
    return new LabelBusObject(bus, objectPath, langIndx, status, this);
}

const std::vector<qcc::String>& Label::getLabels() const
{
    return m_LabelWidgetLabels;
}

void Label::setLabels(const std::vector<qcc::String>& labels)
{
    m_LabelWidgetLabels = labels;
}

GetStringFptr Label::getGetLabels() const
{
    return m_LabelWidgetGetLabels;
}

void Label::setGetLabels(GetStringFptr getLabels)
{
    m_LabelWidgetGetLabels = getLabels;
}

QStatus Label::fillLabelArg(MsgArg& val, uint16_t languageIndx)
{
    if (!(m_LabelWidgetLabels.size() > languageIndx) && !m_LabelWidgetGetLabels) {
        return ER_BUS_PROPERTY_VALUE_NOT_SET;
    }

    return val.Set(AJPARAM_STR.c_str(), m_LabelWidgetGetLabels ? m_LabelWidgetGetLabels(languageIndx) :
                   m_LabelWidgetLabels[languageIndx].c_str());
}

QStatus Label::readLabelArg(MsgArg* val)
{
    QStatus status = ER_OK;
    char* label;
    CHECK_AND_RETURN(val->Get(AJPARAM_STR.c_str(), &label))
    m_Label = label;
    return status;
}

} /* namespace services */
} /* namespace ajn */
