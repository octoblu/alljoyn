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

#include <alljoyn/controlpanel/ActionWithDialog.h>
#include <alljoyn/controlpanel/ControlPanelService.h>
#include "../BusObjects/ActionBusObject.h"
#include "../ControlPanelConstants.h"
#include <alljoyn/controlpanel/LogModule.h>

namespace ajn {
namespace services {
using namespace cpsConsts;

ActionWithDialog::ActionWithDialog(qcc::String const& name, Widget* rootWidget) :
    Widget(name, rootWidget, WIDGET_TYPE_ACTION_WITH_DIALOG), m_Dialog(0)
{
}

ActionWithDialog::ActionWithDialog(qcc::String const& name, Widget* rootWidget, ControlPanelDevice* device) :
    Widget(name, rootWidget, device, WIDGET_TYPE_ACTION_WITH_DIALOG), m_Dialog(0)
{
}

ActionWithDialog::~ActionWithDialog()
{
    if (m_ControlPanelMode == CONTROLLER_MODE && m_Dialog) {
        delete m_Dialog;
    }
}

WidgetBusObject* ActionWithDialog::createWidgetBusObject(BusAttachment* bus, qcc::String const& objectPath,
                                                         uint16_t langIndx, QStatus& status)
{
    return new ActionBusObject(bus, objectPath, langIndx, status, this);
}

Dialog* ActionWithDialog::getChildDialog() const
{
    return m_Dialog;
}

QStatus ActionWithDialog::addChildDialog(Dialog* childDialog)
{
    if (!childDialog) {
        QCC_DbgHLPrintf(("Cannot add a childDialog that is NULL"));
        return ER_BAD_ARG_1;
    }

    m_Dialog = childDialog;

    QCC_DbgPrintf(("Adding childDialog named: %s", childDialog->getWidgetName().c_str()));
    return ER_OK;
}

QStatus ActionWithDialog::registerObjects(BusAttachment* bus, LanguageSet const& languageSet,
                                          qcc::String const& objectPathPrefix, qcc::String const& objectPathSuffix, bool isRoot)
{
    QStatus status = Widget::registerObjects(bus, languageSet, objectPathPrefix, objectPathSuffix, isRoot);
    if (status != ER_OK) {
        return status;
    }

    qcc::String newObjectPathSuffix = isRoot ? objectPathSuffix : objectPathSuffix + "/" + m_Name;

    if (!m_Dialog) {
        QCC_DbgHLPrintf(("Could not register. ActionWithDialog is missing the child Dialog"));
        return ER_FAIL;
    }

    status = m_Dialog->registerObjects(bus, languageSet, objectPathPrefix, newObjectPathSuffix);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not register childDialog objects"));
        return status;
    }
    return status;
}

QStatus ActionWithDialog::unregisterObjects(BusAttachment* bus)
{
    QStatus returnStatus = ER_OK;

    QStatus status = Widget::unregisterObjects(bus);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not unregister BusObjects"));
        returnStatus = status;
    }

    if (m_Dialog) {
        status = m_Dialog->unregisterObjects(bus);
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not unregister Objects for the childDialog"));
            returnStatus = status;
        }
    }
    return returnStatus;
}

QStatus ActionWithDialog::addChildren(BusAttachment* bus)
{
    if (!m_BusObjects.size()) {
        QCC_DbgHLPrintf(("BusObject is not set"));
        return ER_BUS_BUS_NOT_STARTED;
    }

    std::vector<IntrospectionNode> childNodes;
    QStatus status = m_BusObjects[0]->Introspect(childNodes);
    if (status != ER_OK) {
        QCC_LogError(status, ("Introspection failed"));
        return status;
    }

    for (size_t i = 0; i < childNodes.size(); i++) {
        qcc::String const& objectPath = childNodes[i].getObjectPath();
        std::vector<qcc::String> splitObjectPath = ControlPanelService::SplitObjectPath(objectPath.c_str());
        qcc::String name = splitObjectPath.back();
        Dialog* dialog = new Dialog(name, this, m_Device);
        dialog->setIsSecured(childNodes[i].isSecured());
        addChildDialog(dialog);
        dialog->registerObjects(bus, objectPath);
    }
    return ER_OK;
}

QStatus ActionWithDialog::refreshChildren(BusAttachment* bus)
{
    if (!m_Dialog) {
        QCC_DbgHLPrintf(("Could not refresh. ActionWithDialog is missing the child Dialog"));
        return ER_FAIL;
    }

    QStatus status = m_Dialog->refreshObjects(bus);
    if (status != ER_OK) {
        QCC_LogError(status, ("Error refreshing Child: %s", m_Dialog->getWidgetName().c_str()));
    }

    return status;
}

} /* namespace services */
} /* namespace ajn */
