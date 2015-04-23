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

#include <alljoyn/controlpanel/ControlPanelDevice.h>
#include <alljoyn/controlpanel/ControlPanelService.h>
#include "ControlPanelConstants.h"
#include <alljoyn/controlpanel/LogModule.h>

namespace ajn {
namespace services {
using namespace cpsConsts;

ControlPanelDevice::ControlPanelDevice(qcc::String const& deviceBusName) :
    m_DeviceBusName(deviceBusName), m_SessionHandler(this), m_Listener(0)
{

}

ControlPanelDevice::~ControlPanelDevice()
{
}

QStatus ControlPanelDevice::startSessionAsync()
{
    BusAttachment* busAttachment = ControlPanelService::getInstance()->getBusAttachment();
    if (!busAttachment) {
        QCC_DbgHLPrintf(("BusAttachment is not set"));
        return ER_BUS_BUS_NOT_STARTED;
    }
    busAttachment->EnableConcurrentCallbacks();

    if (m_SessionHandler.getSessionId() != 0) {
        QCC_DbgPrintf(("Session already started, firing Listener"));

        ControlPanelListener* listener = getListener();
        if (listener) {
            listener->sessionEstablished(this);
        }

        return ER_OK;
    }

    SessionOpts opts(SessionOpts::TRAFFIC_MESSAGES, true, SessionOpts::PROXIMITY_ANY, TRANSPORT_ANY);
    QStatus status = busAttachment->JoinSessionAsync(m_DeviceBusName.c_str(), (ajn::SessionPort)CONTROLPANELSERVICE_PORT, &m_SessionHandler,
                                                     opts, &m_SessionHandler, NULL);

    if (status != ER_OK) {
        QCC_LogError(status, ("Unable to JoinSession with %s", m_DeviceBusName.c_str()));
    }
    return status;
}

QStatus ControlPanelDevice::startSession()
{
    BusAttachment* busAttachment = ControlPanelService::getInstance()->getBusAttachment();

    if (!busAttachment) {
        QCC_DbgHLPrintf(("BusAttachment is not set"));
        return ER_BUS_BUS_NOT_STARTED;
    }
    busAttachment->EnableConcurrentCallbacks();

    if (m_SessionHandler.getSessionId() != 0) {
        QCC_DbgPrintf(("Session already started, firing Listener"));

        ControlPanelListener* listener = getListener();
        if (listener) {
            listener->sessionEstablished(this);
        }
        return ER_OK;
    }

    SessionOpts opts(SessionOpts::TRAFFIC_MESSAGES, true, SessionOpts::PROXIMITY_ANY, TRANSPORT_ANY);
    SessionId sessionId;
    ajn::SessionPort port = CONTROLPANELSERVICE_PORT;
    QStatus status = busAttachment->JoinSession(m_DeviceBusName.c_str(), port, &m_SessionHandler, sessionId, opts);
    if (status != ER_OK) {
        QCC_LogError(status, ("Unable to JoinSession with %s", m_DeviceBusName.c_str()));
    }
    m_SessionHandler.JoinSessionCB(status, sessionId, opts, this);
    return status;
}

QStatus ControlPanelDevice::endSession()
{
    BusAttachment* busAttachment = ControlPanelService::getInstance()->getBusAttachment();

    if (!busAttachment) {
        QCC_DbgHLPrintf(("BusAttachment is not set"));
        return ER_BUS_BUS_NOT_STARTED;
    }

    SessionId sessionId = m_SessionHandler.getSessionId();
    if (sessionId == 0) {
        QCC_DbgPrintf(("Session not started. Can't end Session"));
        return ER_OK;
    }

    QStatus status = busAttachment->LeaveSession(sessionId);

    if (status != ER_OK) {
        QCC_LogError(status, ("Unable to LeaveSession with %s", m_DeviceBusName.c_str()));
    }

    return status;
}

QStatus ControlPanelDevice::shutdownDevice()
{
    QStatus status = endSession();
    if (status != ER_OK) {
        QCC_LogError(status, ("Call to endSession failed"));
    }

    std::map<qcc::String, ControlPanelControllerUnit*>::iterator iter;
    std::map<qcc::String, ControlPanelControllerUnit*>::iterator deliter;
    for (iter = m_DeviceUnits.begin(); iter != m_DeviceUnits.end();) {
        ControlPanelControllerUnit* unit = iter->second;
        deliter = iter++;

        m_DeviceUnits.erase(deliter);
        unit->shutdownUnit();
        delete unit;
    }
    return status;
}

void ControlPanelDevice::handleSessionJoined()
{
    BusAttachment* busAttachment = ControlPanelService::getInstance()->getBusAttachment();
    if (busAttachment) {
        busAttachment->EnableConcurrentCallbacks();
    }

    ControlPanelListener* listener = getListener();
    std::map<qcc::String, ControlPanelControllerUnit*>::iterator iter;
    for (iter = m_DeviceUnits.begin(); iter != m_DeviceUnits.end(); iter++) {
        QStatus status = iter->second->registerObjects();
        if (status != ER_OK) {
            if (listener) {
                listener->errorOccured(this, status, REGISTER_OBJECTS, "Could not register Objects for this Device's Units");
            }
        }
    }

    if (listener) {
        listener->sessionEstablished(this);
    }
}

ControlPanelControllerUnit* ControlPanelDevice::getControlPanelUnit(qcc::String const& objectPath)
{
    qcc::String unitName = "";
    qcc::String panelName = "";

    ControlPanelControllerUnit* unit = getControlPanelUnit(objectPath, unitName, panelName, false);
    if (!unit) {
        QCC_DbgHLPrintf(("Could not retrieve ControlPanelUnit"));
    }
    return unit;
}

ControlPanelControllerUnit* ControlPanelDevice::addControlPanelUnit(qcc::String const& objectPath, std::vector<qcc::String> const& interfaces)
{
    bool hasControlPanel = false;
    bool hasHttpControl = false;

    for (size_t i = 0; i < interfaces.size(); i++) {
        if (interfaces[i].compare(AJ_CONTROLPANEL_INTERFACE) == 0) {
            hasControlPanel = true;
            QCC_DbgPrintf(("ObjectPath contains ControlPanel"));
        }
        if (interfaces[i].compare(AJ_HTTPCONTROL_INTERFACE) == 0) {
            hasHttpControl = true;
            QCC_DbgPrintf(("ObjectPath contains HttpControl"));
        }
    }

    if (!hasControlPanel && !hasHttpControl) {
        QCC_DbgHLPrintf(("ObjectPath does not contain a ControlPanel or HttpControl"));
        return NULL;
    }

    qcc::String unitName = "";
    qcc::String panelName = "";

    ControlPanelControllerUnit* unit = getControlPanelUnit(objectPath, unitName, panelName);
    if (!unit) {
        QCC_DbgHLPrintf(("Could not add ControlPanelUnit"));
        return NULL;
    }

    if (hasHttpControl) {
        unit->addHttpControl(objectPath);
    }

    if (hasControlPanel) {
        unit->addControlPanel(objectPath, panelName);
    }

    return unit;
}

ControlPanelListener* ControlPanelDevice::getListener() const
{
    if (m_Listener) {
        return m_Listener;
    }
    return ControlPanelService::getInstance()->getControlPanelListener();
}

QStatus ControlPanelDevice::setListener(ControlPanelListener* listener)
{
    if (!listener) {
        QCC_DbgHLPrintf(("Listener cannot be NULL"));
        return ER_BAD_ARG_1;
    }
    m_Listener = listener;
    return ER_OK;
}

ControlPanelControllerUnit* ControlPanelDevice::getControlPanelUnit(qcc::String const& objectPath, qcc::String& unitName, qcc::String& panel,
                                                                    bool createIfNotFound)
{
    std::vector<qcc::String> splitObjectPath = ControlPanelService::SplitObjectPath(objectPath);
    if (splitObjectPath.size() < 3) {
        QCC_DbgHLPrintf(("ObjectPath is unexpected size"));
        return NULL;
    }

    unitName = splitObjectPath[1];
    panel = splitObjectPath[2];

    std::map<qcc::String, ControlPanelControllerUnit*>::iterator it;
    if ((it = m_DeviceUnits.find(unitName)) != m_DeviceUnits.end()) {
        return it->second;
    } else if (createIfNotFound) {
        QCC_DbgPrintf(("Creating new unit %s", unitName.c_str()));
        ControlPanelControllerUnit* unit = new ControlPanelControllerUnit(unitName, this);
        m_DeviceUnits[unitName] = unit;
        return unit;
    }
    //Unit not found in map and createIfNotFound is false
    return NULL;
}

NotificationAction* ControlPanelDevice::addNotificationAction(qcc::String const& objectPath)
{
    if (!objectPath.size()) {
        QCC_DbgHLPrintf(("Cannot add a Notification Action. ObjectPath is empty"));
        return NULL;
    }

    if (m_SessionHandler.getSessionId() == 0) {
        QCC_DbgHLPrintf(("Cannot add a Notification Action. Device not in session"));
        return NULL;
    }

    qcc::String unitName = "";
    qcc::String actionName = "";

    ControlPanelControllerUnit* unit = getControlPanelUnit(objectPath, unitName, actionName);
    if (!unit) {
        QCC_DbgHLPrintf(("Could not add ControlPanelUnit"));
        return NULL;
    }

    QStatus status = unit->addNotificationAction(objectPath, actionName);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not add NotificationAction"));
        return NULL;
    }

    return unit->getNotificationAction(actionName);
}

QStatus ControlPanelDevice::removeNotificationAction(NotificationAction* notificationAction)
{
    if (!notificationAction) {
        QCC_DbgHLPrintf(("Cannot remove the NotificationAction. NotificationAction is NULL"));
        return ER_BAD_ARG_1;
    }

    qcc::String objectPath = notificationAction->getObjectPath();
    if (!objectPath.size()) {
        QCC_DbgHLPrintf(("Cannot remove the NotificationAction. The ObjectPath is empty"));
        return ER_BAD_ARG_1;
    }

    qcc::String unitName = "";
    qcc::String actionName = "";

    ControlPanelControllerUnit* unit = getControlPanelUnit(objectPath, unitName, actionName, false);
    if (!unit) {
        QCC_DbgHLPrintf(("Unit for NotificationAction does not exist"));
        return ER_BAD_ARG_1;
    }

    return unit->removeNotificationAction(actionName);
}

const qcc::String& ControlPanelDevice::getDeviceBusName() const
{
    return m_DeviceBusName;
}

const ajn::SessionId ControlPanelDevice::getSessionId() const
{
    return m_SessionHandler.getSessionId();
}

const std::map<qcc::String, ControlPanelControllerUnit*>& ControlPanelDevice::getDeviceUnits() const
{
    return m_DeviceUnits;
}

void ControlPanelDevice::getAllControlPanels(std::vector<ControlPanel*>& controlPanelsVec)
{
    controlPanelsVec.clear();
    std::map<qcc::String, ControlPanelControllerUnit*>::iterator iter;
    for (iter = m_DeviceUnits.begin(); iter != m_DeviceUnits.end(); iter++) {
        std::map<qcc::String, ControlPanel*> controlPanels = iter->second->getControlPanels();
        std::map<qcc::String, ControlPanel*>::iterator it;
        for (it = controlPanels.begin(); it != controlPanels.end(); it++) {
            controlPanelsVec.push_back(it->second);
        }
    }
}

} /* namespace services */
} /* namespace ajn */
