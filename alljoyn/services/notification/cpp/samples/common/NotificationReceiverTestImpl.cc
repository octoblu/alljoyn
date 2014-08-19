/******************************************************************************
 * Copyright (c) 2013-2014, AllSeen Alliance. All rights reserved.
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

#include "NotificationReceiverTestImpl.h"
#include <alljoyn/notification/NotificationText.h>
#include <alljoyn/notification/RichAudioUrl.h>
#include <alljoyn/Status.h>
#include <iostream>
#include <algorithm>
#include <sstream>

using namespace ajn;
using namespace services;
using namespace qcc;

NotificationReceiverTestImpl::NotificationReceiverTestImpl(bool waitForExternalNotificationAction) :
    m_NotificationAction(ACTION_NOTHING), m_WaitForExternalNotificationAction(waitForExternalNotificationAction) {

    if (m_WaitForExternalNotificationAction) {
        pthread_mutex_init(&m_Lock, NULL);
        pthread_cond_init(&m_Condition, NULL);
    }
}

NotificationReceiverTestImpl::~NotificationReceiverTestImpl() {

    if (m_WaitForExternalNotificationAction) {

        pthread_mutex_lock(&m_Lock);
        pthread_cond_signal(&m_Condition);
        pthread_mutex_unlock(&m_Lock);

        pthread_cond_destroy(&m_Condition);
        pthread_mutex_destroy(&m_Lock);
    }
}

void NotificationReceiverTestImpl::Receive(Notification const& notification) {

    qcc::String appName = notification.getAppName();
    // If applications list is empty or the name exists in the filter list then print the notification
    if ((m_Applications.size() == 0) || (find(m_Applications.begin(), m_Applications.end(), appName) !=  m_Applications.end())) {
        std::cout << "******************** Begin New Message Received ********************" << std::endl;
        std::cout << "Message Id: " << notification.getMessageId() << std::endl;
        std::cout << "Device Id: " << notification.getDeviceId() << std::endl;
        std::cout << "Device Name: " << notification.getDeviceName() << std::endl;
        std::cout << "App Id: " << notification.getAppId() << std::endl;
        std::cout << "App Name: " << notification.getAppName() << std::endl;
        std::cout << "Sender BusName: " << notification.getSenderBusName() << std::endl;
        std::cout << "Message Type " << notification.getMessageType() << " " << MessageTypeUtil::getMessageTypeString(notification.getMessageType()).c_str() << std::endl;
        std::cout << "Notification version: " << notification.getVersion() << std::endl;


        // get vector of text messages and iterate through it
        std::vector<NotificationText> vecMessages = notification.getText();

        for (std::vector<NotificationText>::const_iterator vecMessage_it = vecMessages.begin(); vecMessage_it != vecMessages.end(); ++vecMessage_it) {
            std::cout << "Language: " << vecMessage_it->getLanguage().c_str() << "  Message: " << vecMessage_it->getText().c_str() << std::endl;
        }

        // Print out any other parameters sent in
        std::cout << "Other parameters included:" << std::endl;
        std::map<qcc::String, qcc::String> customAttributes = notification.getCustomAttributes();

        for (std::map<qcc::String, qcc::String>::const_iterator customAttributes_it = customAttributes.begin(); customAttributes_it != customAttributes.end(); ++customAttributes_it) {
            std::cout << "Custom Attribute Key: " << customAttributes_it->first.c_str() << "  Custom Attribute Value: " << customAttributes_it->second.c_str() << std::endl;
        }

        if (notification.getRichIconUrl()) {
            std::cout << "Rich Content Icon Url: " << notification.getRichIconUrl() << std::endl;
        }

        // get vector of audio content and iterate through it
        std::vector<RichAudioUrl> richAudioUrl = notification.getRichAudioUrl();

        if (!richAudioUrl.empty()) {
            std::cout << "******************** Begin Rich Audio Content ********************" << std::endl;
            for (std::vector<RichAudioUrl>::const_iterator vecAudio_it = richAudioUrl.begin(); vecAudio_it != richAudioUrl.end(); ++vecAudio_it) {
                std::cout << "Language: " << vecAudio_it->getLanguage().c_str() << "  Audio Url: " << vecAudio_it->getUrl().c_str() << std::endl;
            }
            std::cout << "******************** End Rich Audio Content ********************" << std::endl;

        }

        if (notification.getRichIconObjectPath()) {
            std::cout << "Rich Content Icon Object Path: " << notification.getRichIconObjectPath() << std::endl;
        }

        if (notification.getRichAudioObjectPath()) {
            std::cout << "Rich Content Audio Object Path: " << notification.getRichAudioObjectPath() << std::endl;
        }

        if (notification.getControlPanelServiceObjectPath()) {
            std::cout << "ControlPanelService object path: " << notification.getControlPanelServiceObjectPath() << std::endl;
        }

        if (notification.getOriginalSender()) {
            std::cout << "OriginalSender: " << notification.getOriginalSender() << std::endl;
        }


        std::cout << "******************** End New Message Received ********************" << std::endl << std::endl;

        Notification nonConstNotification(notification);

        if (m_WaitForExternalNotificationAction) {
            pthread_mutex_lock(&m_Lock);
            pthread_cond_wait(&m_Condition, &m_Lock);
        } else {
            std::cout << "Notification action (0-Nothing 1-Dismiss):" << std::endl;
            int32_t notificationAction(NotificationAction::ACTION_NOTHING);
            int retScan = scanf("%d", &notificationAction);
            if (retScan != EOF) {
                m_NotificationAction = static_cast<NotificationAction>(notificationAction);
            }
        }
        switch (GetNotificationAction()) {
        case ACTION_NOTHING:
            std::cout << "Nothing planed to do with the notification message id:" << nonConstNotification.getMessageId() << std::endl;
            break;

        case ACTION_DISMISS:
            {
                std::cout << "going to call dismiss for message id:" << nonConstNotification.getMessageId() << std::endl;
                nonConstNotification.dismiss();
            }
            break;

        default:
            std::cout << "Got non valid action to do:" << GetNotificationAction() << std::endl;
            break;
        }
        ;
        if (m_WaitForExternalNotificationAction) {
            pthread_mutex_unlock(&m_Lock);
        }
    }
    std::cout << "End handling notification!!!" << std::endl;
}

void NotificationReceiverTestImpl::setApplications(qcc::String const& listOfApps) {
    std::istringstream iss(listOfApps.c_str());
    std::string singleAppName;
    while (std::getline(iss, singleAppName, ';')) {
        m_Applications.push_back(singleAppName.c_str());
    }
}

void NotificationReceiverTestImpl::Dismiss(const int32_t msgId, const qcc::String appId)
{
    std::cout << "Got NotificationReceiverTestImpl::Dismiss with msgId=" << msgId << " appId=" << appId.c_str() << std::endl;
}

NotificationReceiverTestImpl::NotificationAction NotificationReceiverTestImpl::GetNotificationAction()
{
    return m_NotificationAction;
}

void NotificationReceiverTestImpl::SetNotificationAction(NotificationReceiverTestImpl::NotificationAction notificationAction)
{
    if (m_WaitForExternalNotificationAction) {
        pthread_mutex_lock(&m_Lock);
        m_NotificationAction = notificationAction;
        pthread_cond_signal(&m_Condition);
        pthread_mutex_unlock(&m_Lock);
    }
}
