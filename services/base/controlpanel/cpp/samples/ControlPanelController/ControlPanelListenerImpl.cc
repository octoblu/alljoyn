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

#include "ControlPanelListenerImpl.h"
#include "ControllerUtil.h"
#include <alljoyn/controlpanel/ControlPanel.h>
#include <iostream>
#include <algorithm>

using namespace ajn;
using namespace services;

ControlPanelListenerImpl::ControlPanelListenerImpl(ControlPanelController* controller) : m_Controller(controller)
{
}

ControlPanelListenerImpl::~ControlPanelListenerImpl()
{
}

void ControlPanelListenerImpl::sessionEstablished(ControlPanelDevice* device)
{
    if (find(m_ConnectedDevices.begin(), m_ConnectedDevices.end(), device->getDeviceBusName()) != m_ConnectedDevices.end()) {
        std::cout << "Received session established for device which was already parsed - ignoring: " << device->getDeviceBusName().c_str() << std::endl;
        return;
    }

    std::map<qcc::String, ControlPanelControllerUnit*> units = device->getDeviceUnits();
    std::map<qcc::String, ControlPanelControllerUnit*>::iterator iter;
    std::map<qcc::String, ControlPanel*>::iterator it;

    std::cout << "Session has been established with device: " << device->getDeviceBusName().c_str() << std::endl;
    m_ConnectedDevices.push_back(device->getDeviceBusName());
    for (iter = units.begin(); iter != units.end(); iter++) {
        std::cout << "Now parsing unit: " << iter->first.c_str() << std::endl;
        HttpControl* httpControl = iter->second->getHttpControl();
        if (httpControl) {
            std::cout << "Unit has a HttpControl: " << std::endl;
            std::cout << "  HttpControl version: " << httpControl->getInterfaceVersion() << std::endl;
            std::cout << "  HttpControl url: " << httpControl->getUrl().c_str() << std::endl;
        }
        std::map<qcc::String, ControlPanel*> controlPanels = iter->second->getControlPanels();
        for (it = controlPanels.begin(); it != controlPanels.end(); it++) {
            std::cout << "Now parsing panelName: " << it->first.c_str() << std::endl;
            std::vector<qcc::String> languages = it->second->getLanguageSet().getLanguages();
            for (size_t i = 0; i < languages.size(); i++) {
                std::cout << "Now parsing language: " << languages[i].c_str() << std::endl;
                Container* rootContainer = it->second->getRootWidget(languages[i]);
                std::cout << "Finished loading widget: " << rootContainer->getWidgetName().c_str() << std::endl;
                ControllerUtil::printRootWidget(rootContainer);
            }
        }
    }
}

void ControlPanelListenerImpl::sessionLost(ControlPanelDevice* device)
{
    std::cout << "Received sessionLost for device " << device->getDeviceBusName().c_str() << std::endl;
    std::cout << "Sleeping 5 seconds before cleaning up device" << std::endl;
#ifdef _WIN32
    Sleep(5000);
#else
    sleep(5);
#endif

    std::vector<qcc::String>::iterator iter;
    iter = find(m_ConnectedDevices.begin(), m_ConnectedDevices.end(), device->getDeviceBusName());
    if (iter != m_ConnectedDevices.end()) {
        m_ConnectedDevices.erase(iter);
    }

    if (m_Controller) {
        QStatus status = m_Controller->deleteControllableDevice(device->getDeviceBusName());
        std::cout << "    Deleting Controllable Device " << (status == ER_OK ? "succeeded" : "failed") << std::endl;
    }
}

void ControlPanelListenerImpl::errorOccured(ControlPanelDevice* device, QStatus status, ControlPanelTransaction transaction,
                                            qcc::String const& error)
{
    std::cout << "Received an error from service for device " << device->getDeviceBusName().c_str() << std::endl;
    std::cout << "    Error Message: " << error.c_str() << std::endl;
}

void ControlPanelListenerImpl::signalPropertiesChanged(ControlPanelDevice* device, Widget* widget)
{
    std::cout << "Received PropertiesChanged Signal for Widget " << widget->getWidgetName().c_str() << std::endl;
}

void ControlPanelListenerImpl::signalPropertyValueChanged(ControlPanelDevice* device, Property* property)
{
    std::cout << "Received ValueChanged Signal for Widget " << property->getWidgetName().c_str() << std::endl;
    ControllerUtil::printPropertyValue(property->getPropertyValue(), property->getPropertyType());
}

void ControlPanelListenerImpl::signalDismiss(ControlPanelDevice* device, NotificationAction* notificationAction)
{
    std::cout << "Received Dismiss Signal for NotificationAction" << std::endl;
}
