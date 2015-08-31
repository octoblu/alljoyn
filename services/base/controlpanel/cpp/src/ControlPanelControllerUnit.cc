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

#include <alljoyn/controlpanel/ControlPanelControllerUnit.h>
#include <alljoyn/controlpanel/ControlPanelDevice.h>
#include <alljoyn/controlpanel/ControlPanel.h>
#include <alljoyn/controlpanel/NotificationAction.h>
#include <alljoyn/controlpanel/LanguageSets.h>
#include <alljoyn/controlpanel/ControlPanelService.h>
#include "ControlPanelConstants.h"
#include <alljoyn/controlpanel/LogModule.h>

namespace ajn {
namespace services {
using namespace cpsConsts;

ControlPanelControllerUnit::ControlPanelControllerUnit(qcc::String const& unitName, ControlPanelDevice* device) :
    m_UnitName(unitName), m_Device(device), m_HttpControl(0)
{

}

ControlPanelControllerUnit::~ControlPanelControllerUnit()
{
}

QStatus ControlPanelControllerUnit::addHttpControl(qcc::String const& objectPath)
{
    BusAttachment* busAttachment = ControlPanelService::getInstance()->getBusAttachment();

    if (!busAttachment) {
        QCC_DbgHLPrintf(("BusAttachment is not set"));
        return ER_BUS_BUS_NOT_STARTED;
    }

    if (m_HttpControl) {
        QCC_DbgPrintf(("HttpControl for this unit already exists"));
        return ER_OK;
    }

    m_HttpControl = new HttpControl(objectPath, m_Device);
    QStatus status = ER_OK;

    if (m_Device->getSessionId() != 0) {
        status = m_HttpControl->registerObjects(busAttachment);
    }
    return status;
}

QStatus ControlPanelControllerUnit::addControlPanel(qcc::String const& objectPath, qcc::String const& panelName)
{
    BusAttachment* busAttachment = ControlPanelService::getInstance()->getBusAttachment();

    if (!busAttachment) {
        QCC_DbgHLPrintf(("BusAttachment is not set"));
        return ER_BUS_BUS_NOT_STARTED;
    }

    std::map<qcc::String, ControlPanel*>::iterator it;
    ControlPanel* controlPanel = 0;

    if ((it = m_ControlPanels.find(panelName)) == m_ControlPanels.end()) {
        LanguageSet myLanguages(m_UnitName + "-CP-" + panelName);
        LanguageSets::add(myLanguages.getLanguageSetName(), myLanguages);
        controlPanel = new ControlPanel(*LanguageSets::get(myLanguages.getLanguageSetName()), objectPath, m_Device);
        m_ControlPanels[panelName] = controlPanel;
    } else {
        QCC_DbgPrintf(("ControlPanel for %s already exists", panelName.c_str()));
        controlPanel = it->second;
    }

    if (m_Device->getSessionId() != 0) {
        controlPanel->registerObjects(busAttachment);
    }
    return ER_OK;
}

QStatus ControlPanelControllerUnit::addNotificationAction(qcc::String const& objectPath, qcc::String const& actionName)
{
    BusAttachment* busAttachment = ControlPanelService::getInstance()->getBusAttachment();

    if (!busAttachment) {
        QCC_DbgHLPrintf(("BusAttachment is not set"));
        return ER_BUS_BUS_NOT_STARTED;
    }

    std::map<qcc::String, NotificationAction*>::iterator it;
    NotificationAction* notificationAction = 0;

    if ((it = m_NotificationActions.find(actionName)) == m_NotificationActions.end()) {
        LanguageSet myLanguages(m_UnitName + "-NA-" + actionName);
        LanguageSets::add(myLanguages.getLanguageSetName(), myLanguages);
        notificationAction = new NotificationAction(*LanguageSets::get(myLanguages.getLanguageSetName()), objectPath, m_Device);
        m_NotificationActions[actionName] = notificationAction;
    } else {
        QCC_DbgPrintf(("NotificationAction for %s already exists", actionName.c_str()));
        notificationAction = it->second;
    }

    QStatus status = ER_OK;
    if (m_Device->getSessionId() != 0) {
        status = notificationAction->registerObjects(busAttachment);
        if (status != ER_OK) {
            QCC_LogError(status, ("Registering NotificationAction Objects failed"));
        }
    }
    return status;
}

QStatus ControlPanelControllerUnit::removeNotificationAction(qcc::String const& actionName)
{
    BusAttachment* busAttachment = ControlPanelService::getInstance()->getBusAttachment();
    QStatus status = ER_OK;

    std::map<qcc::String, NotificationAction*>::iterator it;
    if ((it = m_NotificationActions.find(actionName)) != m_NotificationActions.end()) {
        NotificationAction* notificationAction = it->second;
        m_NotificationActions.erase(it);
        status = notificationAction->unregisterObjects(busAttachment);
        delete notificationAction;
    }
    return status;
}

QStatus ControlPanelControllerUnit::registerObjects()
{
    BusAttachment* busAttachment = ControlPanelService::getInstance()->getBusAttachment();

    if (!busAttachment) {
        QCC_DbgHLPrintf(("BusAttachment is not set"));
        return ER_BUS_BUS_NOT_STARTED;
    }

    busAttachment->EnableConcurrentCallbacks();

    QStatus status;
    QStatus returnStatus = ER_OK;

    if (m_HttpControl) {
        returnStatus = m_HttpControl->registerObjects(busAttachment);
    }

    std::map<qcc::String, ControlPanel*>::iterator it;
    for (it = m_ControlPanels.begin(); it != m_ControlPanels.end(); it++) {
        if ((status = it->second->registerObjects(busAttachment)) != ER_OK) {
            QCC_LogError(status, ("Registering ControlPanel Objects failed"));
            returnStatus = status;
        }
    }
    return returnStatus;
}

QStatus ControlPanelControllerUnit::shutdownUnit()
{
    BusAttachment* busAttachment = ControlPanelService::getInstance()->getBusAttachment();
    if (busAttachment) {
        busAttachment->EnableConcurrentCallbacks();
    }

    std::map<qcc::String, ControlPanel*>::iterator cpIter;
    std::map<qcc::String, ControlPanel*>::iterator cpDeliter;
    for (cpIter = m_ControlPanels.begin(); cpIter != m_ControlPanels.end();) {
        ControlPanel* controlpanel = cpIter->second;
        cpDeliter = cpIter++;

        m_ControlPanels.erase(cpDeliter);
        controlpanel->unregisterObjects(busAttachment);
        delete controlpanel;
    }

    std::map<qcc::String, NotificationAction*>::iterator naIter;
    std::map<qcc::String, NotificationAction*>::iterator naDeliter;
    for (naIter = m_NotificationActions.begin(); naIter != m_NotificationActions.end();) {
        NotificationAction* notificationAction = naIter->second;
        naDeliter = naIter++;

        m_NotificationActions.erase(naDeliter);
        notificationAction->unregisterObjects(busAttachment);
        delete notificationAction;
    }

    if (m_HttpControl) {
        m_HttpControl->unregisterObjects(busAttachment);
        delete m_HttpControl;
        m_HttpControl = 0;
    }

    return ER_OK;
}

ControlPanelDevice* ControlPanelControllerUnit::getDevice() const
{
    return m_Device;
}

const qcc::String& ControlPanelControllerUnit::getUnitName() const
{
    return m_UnitName;
}

const std::map<qcc::String, ControlPanel*>& ControlPanelControllerUnit::getControlPanels() const
{
    return m_ControlPanels;
}

const std::map<qcc::String, NotificationAction*>& ControlPanelControllerUnit::getNotificationActions() const
{
    return m_NotificationActions;
}

ControlPanel* ControlPanelControllerUnit::getControlPanel(qcc::String const& panelName) const
{
    std::map<qcc::String, ControlPanel*>::const_iterator it;
    if ((it = m_ControlPanels.find(panelName)) != m_ControlPanels.end()) {
        return it->second;
    }
    return NULL;
}

NotificationAction* ControlPanelControllerUnit::getNotificationAction(qcc::String const& actionName) const
{
    std::map<qcc::String, NotificationAction*>::const_iterator it;
    if ((it = m_NotificationActions.find(actionName)) != m_NotificationActions.end()) {
        return it->second;
    }
    return NULL;
}

HttpControl* ControlPanelControllerUnit::getHttpControl() const
{
    return m_HttpControl;
}

} /* namespace services */
} /* namespace ajn */

