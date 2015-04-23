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

#include <iostream>
#include <sstream>
#include <cstdio>
#include <signal.h>
#include <alljoyn/PasswordManager.h>
#include <alljoyn/notification/NotificationService.h>
#include <alljoyn/notification/NotificationSender.h>
#include <alljoyn/notification/NotificationEnums.h>
#include <alljoyn/notification/NotificationText.h>
#include <alljoyn/notification/RichAudioUrl.h>
#include "CommonSampleUtil.h"
#include <alljoyn/notification/Notification.h>
#include <stdio.h>
#include <alljoyn/services_common/LogModulesNames.h>
#include <alljoyn/services_common/GuidUtil.h>

#define SERVICE_PORT 900

using namespace ajn;
using namespace services;
using namespace qcc;

NotificationService* prodService = 0;
BusAttachment* bus = 0;
CommonBusListener* notificationBusListener = 0;
AboutData* aboutData = NULL;
AboutObj* aboutObj = NULL;
NotificationSender* Sender = 0;
static volatile sig_atomic_t s_interrupt = false;

enum states {
    BeginNewNotification,
    SetDeviceName,
    SetAppName,
    SetMsgType,
    CheckForNewMsg,
    SetMsgLang,
    SetMsgText,
    CheckForCustomAttributes,
    SetCustomKey,
    SetCustomVal,
    CheckForRichIconUrl,
    SetRichIconUrl,
    CheckForNewRichAudioUrl,
    SetRichAudioUrlLang,
    SetRichAudioUrlUrl,
    CheckForRichIconObjectPath,
    SetRichIconObjectPath,
    CheckForRichAudioObjectPath,
    SetRichAudioObjectPath,
    CheckForControlPanelServiceObjectPath,
    SetControlPanelServiceObjectPath,
    SetTTL,
    SetSleepTime,
    EndNotification,
};

bool getInput(qcc::String& device_name, qcc::String& app_name, NotificationMessageType& messageType,
              std::vector<NotificationText>& vecMessages, std::map<qcc::String, qcc::String>& customAttributes, qcc::String& richIconUrl,
              std::vector<RichAudioUrl>& richAudioUrl, qcc::String& richIconObjectPath, qcc::String& richAudioObjectPath, qcc::String& controlPanelServiceObjectPath,
              uint16_t& ttl, int32_t& sleepTime)
{
    int32_t state = BeginNewNotification;
    std::string input;
    qcc::String tempLang = "";
    qcc::String tempText = "";
    qcc::String tempUrl = "";
    qcc::String tempKey;
    qcc::String defaultDeviceName = "defaultDeviceName";
    qcc::String defaultAppName = "defaultAppName";
    qcc::String defaultLang = "en";
    qcc::String defaultText = "Using the default text.";
    qcc::String defaultRichAudioUrl = "http://myRichContentAudioUrl.wv";
    qcc::String defaultRichIconUrl = "http://myRichContentIconUrl.jpg";
    uint16_t defaultTTL = 30;
    qcc::String defaultRichIconObjectPath = "/Icon/ObjectPath";
    qcc::String defaultRichAudioObjectPath = "/Audio/ObjectPath";
    qcc::String defaultControlPanelServiceObjectPath = "/ControlPanel/MyDevice/areYouSure";

    std::cout << "Enter in new notification info:" << std::endl;
    std::cout << "Enter in device name or press 'enter' to use default:" << std::endl;
    state = SetDeviceName;

    getline(std::cin, input);

    while (!std::cin.eof()) {
        switch (state) {

        case SetDeviceName:
            device_name = input.length() ? input.c_str() : defaultDeviceName;
            std::cout << "Enter in application name or press 'enter' to use default:" << std::endl;
            state = SetAppName;
            break;

        case SetAppName:
            app_name = input.length() ? input.c_str() : defaultAppName;
            std::cout << "Enter in message type (0,1,2) or press 'enter' to use default:" << std::endl;
            state = SetMsgType;
            break;

        case SetMsgType:
            // If atoi fails message type will hold a 0 which is fine in this case
            messageType = input.length() ? (NotificationMessageType)atoi(input.c_str()) : NotificationMessageType(INFO);
            // We require to send at least one message
            std::cout << "Enter in message language (ex: en_US) or press 'enter' to use default and move to message text:" << std::endl;
            state = SetMsgLang;
            break;

        case CheckForNewMsg:
            if (input.compare("y") == 0) {
                std::cout << "Enter in message language (ex: en_US) or press 'enter' to use default and move to message text:" << std::endl;
                state = SetMsgLang;
            } else {
                std::cout << "Do you want to enter a set of custom Attributes? (press 'y' for yes or anything else for no)" << std::endl;
                state = CheckForCustomAttributes;
            }
            break;

        case SetMsgLang:
            // Currently allowing an empty value for the language so no need to check the value
            tempLang = input.length() ? input.c_str() : defaultLang;
            std::cout << "Enter in message text or press 'enter' to use default and move on to custom Attributes:" << std::endl;
            state = SetMsgText;
            break;

        case SetMsgText:
            {
                tempText = input.length() ? input.c_str() : defaultText;
                // It is possible for both the language and text to be empty
                NotificationText textToSend(tempLang.c_str(), tempText.c_str());
                vecMessages.push_back(textToSend);
                tempLang = "";
                tempText = "";
                std::cout << "Do you want to enter a new message? (press 'y' for yes or anything else for no)" << std::endl;
                state = CheckForNewMsg;
                break;
            }

        case CheckForCustomAttributes:
            if (input.compare("y") == 0) {
                std::cout << "Enter in custom attribute key or press 'enter' to continue:" << std::endl;
                state = SetCustomKey;
            } else {
                std::cout << "Do you want to enter a rich content icon url? (press 'y' for yes or anything else for no)" << std::endl;
                state = CheckForRichIconUrl;
            }
            break;

        case SetCustomKey:
            tempKey = input.c_str();
            std::cout << "Enter in custom attribute value or press 'enter' to continue:" << std::endl;
            state = SetCustomVal;
            break;

        case SetCustomVal:
            customAttributes[tempKey] = input.c_str();
            std::cout << "Do you want to enter another set of custom attributes? (press 'y' for yes or anything else for no)" << std::endl;
            state = CheckForCustomAttributes;
            break;

        case CheckForRichIconUrl:
            if (input.compare("y") == 0) {
                std::cout << "Enter in a url for rich icon or press 'enter' to use default:" << std::endl;
                state = SetRichIconUrl;
            } else {
                std::cout << "Do you want to enter a rich content audio url? (press 'y' for yes or anything else for no)" << std::endl;
                state = CheckForNewRichAudioUrl;
            }
            break;

        case SetRichIconUrl:
            richIconUrl = input.length() ? input.c_str() : defaultRichIconUrl;
            std::cout << "Do you want to enter a rich content audio url? (press 'y' for yes or anything else for no)" << std::endl;
            state = CheckForNewRichAudioUrl;
            break;


        case CheckForNewRichAudioUrl:
            if (input.compare("y") == 0) {
                std::cout << "Enter in a language (ex: en_US) for the audio content or press 'enter' to use default and move to url text:" << std::endl;
                state = SetRichAudioUrlLang;
            } else {
                std::cout << "Do you want to enter a rich content icon object path? (press 'y' for yes or anything else for no)" << std::endl;
                state = CheckForRichIconObjectPath;
            }
            break;

        case SetRichAudioUrlLang:
            // Currently allowing an empty value for the language so no need to check the value
            tempLang = input.length() ? input.c_str() : defaultLang;
            std::cout << "Enter in a url for audio content or press 'enter' to use default and move on to set ttl:" << std::endl;
            state = SetRichAudioUrlUrl;
            break;

        case SetRichAudioUrlUrl:
            {
                tempUrl = input.length() ? input.c_str() : defaultRichAudioUrl;
                // It is possible for both the language and text to be empty
                RichAudioUrl audioContent(tempLang.c_str(), tempUrl.c_str());
                richAudioUrl.push_back(audioContent);
                tempLang = "";
                tempText = "";
                std::cout << "Do you want to enter another rich audio content? (press 'y' for yes or anything else for no)" << std::endl;
                state = CheckForNewRichAudioUrl;
                break;
            }

        case CheckForRichIconObjectPath:
            if (input.compare("y") == 0) {
                std::cout << "Enter in a object path for rich icon or press 'enter' to use default:" << std::endl;
                state = SetRichIconObjectPath;
            } else {
                std::cout << "Do you want to enter a rich content audio object path? (press 'y' for yes or anything else for no)" << std::endl;
                state = CheckForRichAudioObjectPath;
            }
            break;

        case SetRichIconObjectPath:
            richIconObjectPath = input.length() ? input.c_str() : defaultRichIconObjectPath;
            std::cout << "Do you want to enter a rich content audio object path? (press 'y' for yes or anything else for no)" << std::endl;
            state = CheckForRichAudioObjectPath;
            break;


        case CheckForRichAudioObjectPath:
            if (input.compare("y") == 0) {
                std::cout << "Enter in a object path for rich audio or press 'enter' to use default:" << std::endl;
                state = SetRichAudioObjectPath;
            } else {
                std::cout << "Do you want to enter a control panel service object path? (press 'y' for yes or anything else for no)" << std::endl;
                state = CheckForControlPanelServiceObjectPath;
            }
            break;

        case SetRichAudioObjectPath:
            richAudioObjectPath = input.length() ? input.c_str() : defaultRichAudioObjectPath;
            std::cout << "Do you want to enter a control panel service object path? (press 'y' for yes or anything else for no)" << std::endl;
            state = CheckForControlPanelServiceObjectPath;
            break;


        case CheckForControlPanelServiceObjectPath:
            if (input.compare("y") == 0) {
                std::cout << "Enter in the ControlPanelService object path or press 'enter' to use default:" << std::endl;
                state = SetControlPanelServiceObjectPath;
            } else {
                std::cout << "Enter in TTL of message in seconds or press 'enter' to use default:" << std::endl;
                state = SetTTL;
            }
            break;

        case SetControlPanelServiceObjectPath:
            controlPanelServiceObjectPath = input.length() ? input.c_str() : defaultControlPanelServiceObjectPath;
            std::cout << "Enter in TTL of message in seconds or press 'enter' to use default:" << std::endl;
            state = SetTTL;
            break;

        case SetTTL:
            if (input.length()) {
                ttl = atoi(input.c_str());
            } else {
                ttl = defaultTTL;
            }
            std::cout << "Enter in sleep time between messages (in milliseconds) or press 'enter' to end notification:" << std::endl;
            state = SetSleepTime;
            break;

        case SetSleepTime:
            if (input.size() > 0) {
                sleepTime = (atoi(input.c_str()) / 1000);
            } else {
                sleepTime = 0;
            }
            return true;
        }
        getline(std::cin, input);
    }
    return false;
}

void cleanup()
{
    if (prodService) {
        prodService->shutdown();
        prodService = NULL;
    }
    if (Sender) {
        delete Sender;
        Sender = NULL;
    }
    if (bus && notificationBusListener) {
        CommonSampleUtil::aboutServiceDestroy(bus, notificationBusListener);
    }
    if (notificationBusListener) {
        delete notificationBusListener;
        notificationBusListener = NULL;
    }
    if (aboutData) {
        delete aboutData;
        aboutData = NULL;
    }
    if (aboutObj) {
        delete aboutObj;
        bus = NULL;
    }
    if (bus) {
        delete bus;
        bus = NULL;
    }
    std::cout << "Goodbye!" << std::endl;
}

void signal_callback_handler(int32_t signum)
{
    s_interrupt = true;
}

int main()
{
    notificationBusListener = new CommonBusListener();
    aboutData = new AboutData("en");

    // Allow CTRL+C to end application
    signal(SIGINT, signal_callback_handler);

    // Initialize Service object
    prodService = NotificationService::getInstance();
//set Daemon passowrd only for bundled app
#ifdef QCC_USING_BD
    PasswordManager::SetCredentials("ALLJOYN_PIN_KEYX", "000000");
#endif

    QCC_SetDebugLevel(logModules::NOTIFICATION_MODULE_LOG_NAME, logModules::ALL_LOG_LEVELS);

    std::cout << "Begin Producer Application. (Press CTRL+D to end application)" << std::endl;

    bus = CommonSampleUtil::prepareBusAttachment();
    if (bus == NULL) {
        std::cout << "Could not initialize BusAttachment." << std::endl;
        cleanup();
        return 1;
    }

    aboutObj = new AboutObj(*bus, AboutObj::ANNOUNCED);

    qcc::String device_id;
    GuidUtil::GetInstance()->GetDeviceIdString(&device_id);
    qcc::String app_id;
    GuidUtil::GetInstance()->GenerateGUID(&app_id);

    //Run in loop unless ctrl+c has been hit
    while (s_interrupt == false) {
        qcc::String device_name, app_name;
        qcc::String richIconUrl = "";
        qcc::String richIconObjectPath = "";
        qcc::String richAudioObjectPath = "";
        qcc::String controlPanelServiceObjectPath = "";
        NotificationMessageType messageType = NotificationMessageType(INFO);
        std::vector<NotificationText> vecMessages;
        std::map<qcc::String, qcc::String> customAttributes;
        std::vector<RichAudioUrl> richAudioUrl;

        uint16_t ttl;
        int32_t sleepTime;
        QStatus status;

        if (!getInput(device_name, app_name, messageType, vecMessages, customAttributes,
                      richIconUrl, richAudioUrl, richIconObjectPath, richAudioObjectPath, controlPanelServiceObjectPath, ttl, sleepTime)) {
            break;
        }

        DeviceNamesType deviceNames;
        deviceNames.insert(std::pair<qcc::String, qcc::String>("en", device_name));
        status = CommonSampleUtil::fillAboutData(aboutData, app_id, app_name, device_id, deviceNames);
        if (status != ER_OK) {
            std::cout << "Could not fill About Data." << std::endl;
            cleanup();
            return 1;
        }

        status = CommonSampleUtil::prepareAboutService(bus, aboutData, aboutObj,
                                                       notificationBusListener, SERVICE_PORT);
        if (status != ER_OK) {
            std::cout << "Could not set up the AboutService." << std::endl;
            cleanup();
            return 1;
        }
        Sender = prodService->initSend(bus, aboutData);
        if (!Sender) {
            std::cout << "Could not initialize the sender" << std::endl;
            CommonSampleUtil::aboutServiceDestroy(bus, notificationBusListener);
            continue;
        }

        status = CommonSampleUtil::aboutServiceAnnounce();
        if (status != ER_OK) {
            std::cout << "Could not announce." << std::endl;
            cleanup();
            return 1;
        }

        Notification notification(messageType, vecMessages);
        notification.setCustomAttributes(customAttributes);
        notification.setRichAudioUrl(richAudioUrl);
        if (richIconUrl.length()) {
            notification.setRichIconUrl(richIconUrl.c_str());
        }
        if (richIconObjectPath.length()) {
            notification.setRichIconObjectPath(richIconObjectPath.c_str());
        }
        if (richAudioObjectPath.length()) {
            notification.setRichAudioObjectPath(richAudioObjectPath.c_str());
        }
        if (controlPanelServiceObjectPath.length()) {
            notification.setControlPanelServiceObjectPath(controlPanelServiceObjectPath.c_str());
        }

        if (Sender->send(notification, ttl) != ER_OK) {
            std::cout << "Could not send the message successfully" << std::endl;
        } else {
            std::cout << "Notification sent with ttl of " << ttl << std::endl;
        }

        std::string input;
        do {
            std::cout << "To delete the bus connection and start again please push 'c' character:" << std::endl;
            getline(std::cin, input);
        } while ((std::cin) && (input != "c"));

        prodService->shutdownSender();
        if (Sender) {
            std::cout << "Going to delete Sender" << std::endl;
            delete Sender;
            Sender = 0;
        }

        std::cout << "Going to call aboutServiceDestroy" << std::endl;
        CommonSampleUtil::aboutServiceDestroy(bus, notificationBusListener);
        std::cout << "Going to sleep:" << sleepTime << std::endl;
#ifdef _WIN32
        Sleep(sleepTime * 1000);
#else
        sleep(sleepTime);
#endif
    }

    std::cout << "Exiting the application deletes the bus connection." << std::endl;

    cleanup();
    return 0;
}

