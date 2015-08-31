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

#include <algorithm>
#include <iostream>
#include <signal.h>
#include <alljoyn/PasswordManager.h>
#include <alljoyn/notification/NotificationService.h>
#include <alljoyn/notification/NotificationSender.h>
#include <alljoyn/notification/NotificationText.h>
#include <alljoyn/notification/RichAudioUrl.h>
#include <alljoyn/notification/NotificationEnums.h>
#include "CommonSampleUtil.h"
#include <alljoyn/notification/Notification.h>
#include <alljoyn/services_common/GuidUtil.h>
#include <alljoyn/services_common/LogModulesNames.h>

using namespace qcc;
using namespace ajn;

// Set application constants
#define DEVICE_NAME "testdeviceName"
#define APP_NAME "testappName"
#define LANG1  "en"
#define TEXT1 "Hello World"
#define LANG2  "es"
#define TEXT2 "Hola Mundo"
#define KEY1 "On"
#define VAL1 "Hello"
#define KEY2 "Off"
#define VAL2 "Goodbye"
#define URL1 "http://url1.com"
#define URL2 "http://url2.com"
#define RICH_ICON_URL "http://iconurl.com"
#define CONTROL_PANEL_SERVICE_OBJECT_PATH "/ControlPanel/MyDevice/areYouSure"
#define RICH_ICON_OBJECT_PATH "/icon/objectPath"
#define RICH_AUDIO_OBJECT_PATH "/Audio/objectPath"
#define SERVICE_PORT 900

using namespace ajn;
using namespace services;

NotificationService* prodService = 0;
NotificationSender* Sender = 0;
BusAttachment* bus = 0;
AboutData* aboutData = NULL;
AboutObj* aboutObj = NULL;
CommonBusListener*  busListener = 0;
static volatile sig_atomic_t s_interrupt = false;

void cleanup()
{
    // Clean up
    if (prodService) {
        prodService->shutdown();
        prodService = NULL;
    }
    if (bus && busListener) {
        CommonSampleUtil::aboutServiceDestroy(bus, busListener);
    }
    if (busListener) {
        delete busListener;
        busListener = NULL;
    }
    if (aboutData) {
        delete aboutData;
        aboutData = NULL;
    }
    if (aboutObj) {
        delete aboutObj;
        aboutObj = NULL;
    }
    if (bus) {
        delete bus;
        bus = NULL;
    }
    std::cout << "Goodbye!" << std::endl;
}

void signal_callback_handler(int32_t signum)
{
    std::cout << "got signal_callback_handler" << std::endl;
    s_interrupt = true;
}

void WaitForSigInt() {
    while (s_interrupt == false) {
#ifdef _WIN32
        Sleep(100);
#else
        usleep(100 * 1000);
#endif
    }
}

int main()
{
    // Allow CTRL+C to end application
    signal(SIGINT, signal_callback_handler);

    // Initialize Service object and Sender Object
    prodService = NotificationService::getInstance();

//set Daemon password only for bundled app
#ifdef QCC_USING_BD
    PasswordManager::SetCredentials("ALLJOYN_PIN_KEYX", "000000");
#endif
    QStatus status;

    QCC_SetDebugLevel(logModules::NOTIFICATION_MODULE_LOG_NAME, logModules::ALL_LOG_LEVELS);

    bus = CommonSampleUtil::prepareBusAttachment();
    if (bus == NULL) {
        std::cout << "Could not initialize BusAttachment." << std::endl;
        return 1;
    }

    qcc::String deviceid;
    GuidUtil::GetInstance()->GetDeviceIdString(&deviceid);
    qcc::String appid;
    GuidUtil::GetInstance()->GenerateGUID(&appid);

    aboutData = new AboutData("en");
    DeviceNamesType deviceNames;
    deviceNames.insert(std::pair<qcc::String, qcc::String>("en", "ProducerBasicDeviceName"));
    status = CommonSampleUtil::fillAboutData(aboutData, appid, APP_NAME, deviceid, deviceNames);
    if (status != ER_OK) {
        std::cout << "Could not fill About Data." << std::endl;
        cleanup();
        return 1;
    }

    busListener = new CommonBusListener();
    aboutObj = new AboutObj(*bus, BusObject::ANNOUNCED);
    status = CommonSampleUtil::prepareAboutService(bus, aboutData, aboutObj, busListener, SERVICE_PORT);
    if (status != ER_OK) {
        std::cout << "Could not set up the AboutService." << std::endl;
        cleanup();
        return 1;
    }

    Sender = prodService->initSend(bus, aboutData);
    if (!Sender) {
        std::cout << "Could not initialize Sender - exiting application" << std::endl;
        cleanup();
        return 1;
    }

    status = CommonSampleUtil::aboutServiceAnnounce();
    if (status != ER_OK) {
        std::cout << "Could not announce." << std::endl;
        cleanup();
        return 1;
    }
    // Prepare message type
    NotificationMessageType messageType = INFO;

    // Prepare text object, set language and text, add notification to vector
    NotificationText textToSend1(LANG1, TEXT1);
    NotificationText textToSend2(LANG2, TEXT2);

    std::vector<NotificationText> vecMessages;
    vecMessages.push_back(textToSend1);
    vecMessages.push_back(textToSend2);

    // Add variable parameters
    std::map<qcc::String, qcc::String> customAttributes;
    customAttributes[KEY1] = VAL1;
    customAttributes[KEY2] = VAL2;

    //Prepare Rich Notification Content
    RichAudioUrl audio1(LANG1, URL1);
    RichAudioUrl audio2(LANG2, URL2);

    std::vector<RichAudioUrl> richAudioUrl;

    richAudioUrl.push_back(audio1);
    richAudioUrl.push_back(audio2);

    qcc::String richIconUrl = RICH_ICON_URL;
    qcc::String richIconObjectPath = RICH_ICON_OBJECT_PATH;
    qcc::String richAudioObjectPath = RICH_AUDIO_OBJECT_PATH;
    qcc::String controlPanelServiceObjectPath = CONTROL_PANEL_SERVICE_OBJECT_PATH;

    // Send messages
    Notification notification(messageType, vecMessages);
    notification.setCustomAttributes(customAttributes);
    notification.setRichIconUrl(richIconUrl.c_str());
    notification.setRichAudioUrl(richAudioUrl);
    notification.setRichIconObjectPath(richIconObjectPath.c_str());
    notification.setRichAudioObjectPath(richAudioObjectPath.c_str());
    notification.setControlPanelServiceObjectPath(controlPanelServiceObjectPath.c_str());

    status = Sender->send(notification, 7200);
    if (status != ER_OK) {
        std::cout << "Notification was not sent successfully - exiting application" << std::endl;
        cleanup();
        return 1;
    }

    std::cout << "Notification sent! " << std::endl;
    std::cout << "Hit Ctrl+C to exit the application" << std::endl;

    WaitForSigInt();

    std::cout << "Exiting the application and deleting the bus connection." << std::endl;
    cleanup();

    return 0;
}
