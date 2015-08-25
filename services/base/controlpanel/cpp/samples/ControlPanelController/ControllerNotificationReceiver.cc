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

#include "ControllerNotificationReceiver.h"
#include "ControllerUtil.h"
#include <iostream>

using namespace ajn;
using namespace services;
using namespace qcc;

ControllerNotificationReceiver::ControllerNotificationReceiver(ControlPanelController* controlPanelController) :
    m_Controller(controlPanelController)
{

}

ControllerNotificationReceiver::~ControllerNotificationReceiver()
{

}

void ControllerNotificationReceiver::Receive(Notification const& notification)
{
    if (m_Controller && notification.getControlPanelServiceObjectPath()) {
        std::cout << "Received ControlPanelService object path: " << notification.getControlPanelServiceObjectPath() << std::endl;

        ControlPanelDevice* device = m_Controller->getControllableDevice(notification.getSenderBusName());
        if (!device) {
            std::cout << "Could not get a Controllable Device" << std::endl;
            return;
        }

        QStatus status = device->startSession();
        if (status != ER_OK) {
            std::cout << "Could not start a session with the device" << std::endl;
            return;
        }

        NotificationAction* notficationAction = device->addNotificationAction(notification.getControlPanelServiceObjectPath());
        if (!notficationAction) {
            std::cout << "Could not add the notificationAction" << std::endl;
            return;
        }

        std::cout << "Parsing the notificationAction received" << std::endl;
        std::vector<qcc::String> languages = notficationAction->getLanguageSet().getLanguages();
        for (size_t i = 0; i < languages.size(); i++) {

            std::vector<Action*> actionsToExecute;
            std::vector<Property*> propertiesToChange;
            std::vector<Dialog*> dialogsToExecute;

            std::cout << "Now parsing language: " << languages[i].c_str() << std::endl;
            RootWidget* rootWidget = notficationAction->getRootWidget(languages[i]);

            std::cout << "Finished loading widget: " << rootWidget->getWidgetName().c_str() << std::endl;
            ControllerUtil::printRootWidget(rootWidget);
        }

        std::cout << "Removing the notificationAction from the device" << std::endl;
        status = device->removeNotificationAction(notficationAction);
        std::cout << "    Removing NotificationAction " << (status == ER_OK ? "succeeded" : "failed") << std::endl;
    }
}

void ControllerNotificationReceiver::Dismiss(const int32_t msgId, const qcc::String appId)
{
    std::cout << "Got ControllerNotificationReceiver::DismissHandler with msgId=" << msgId << " appId=" << appId.c_str() << std::endl;
}
