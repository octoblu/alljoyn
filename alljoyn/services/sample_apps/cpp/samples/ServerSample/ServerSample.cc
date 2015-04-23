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

#include <signal.h>
#include <SrpKeyXListener.h>
#include <CommonSampleUtil.h>
#include <AboutDataStore.h>
#include <alljoyn/AboutObj.h>
#include <alljoyn/AboutData.h>
#include <OptParser.h>
#include <alljoyn/services_common/LogModulesNames.h>
#include <alljoyn/AboutIconObj.h>
#include <AboutDataStore.h>
#include <qcc/StringUtil.h>

#ifdef _CONFIG_
#include <alljoyn/config/ConfigService.h>
#include "ConfigServiceListenerImpl.h"
#endif

#ifdef _NOTIFICATION_
#include <alljoyn/notification/NotificationService.h>
#include <alljoyn/notification/NotificationText.h>
#include <alljoyn/notification/Notification.h>
#endif

#if defined (_TESTMODE_) && defined (_NOTIFICATION_)
#include "NotificationTesterImpl.h"
#endif

#ifdef _CONTROLPANEL_
#include <ControlPanelGenerated.h>
#include <alljoyn/controlpanel/ControlPanelService.h>
#include <alljoyn/controlpanel/ControlPanelControllee.h>
#include <alljoyn/controlpanel/LanguageSets.h>
#endif

using namespace ajn;
using namespace services;

#define SERVICE_EXIT_OK       0
#define SERVICE_OPTION_ERROR  1
#define SERVICE_CONFIG_ERROR  2

static char const* const QCC_MODULE = "ServerSample";

/** static variables need for sample */
static BusAttachment* msgBus = NULL;
static SrpKeyXListener* keyListener = NULL;
static CommonBusListener* busListener = NULL;
static AboutIcon* icon = NULL;
static AboutIconObj* aboutIconObj = NULL;
static AboutDataStore* aboutDataStore = NULL;
static AboutObj* aboutObj = NULL;

static SessionPort servicePort = 900;
static qcc::String configFile;
static volatile sig_atomic_t s_interrupt = false;
static volatile sig_atomic_t s_restart = false;

#ifdef _CONFIG_
static ConfigService* configService = NULL;
static ConfigServiceListenerImpl* configServiceListenerImpl = NULL;
#endif

#ifdef _NOTIFICATION_
NotificationService* prodService = NULL;
NotificationSender* sender = NULL;
#ifdef _TESTMODE_
NotificationTesterImpl* notifTester = new NotificationTesterImpl();
#endif
#endif

#ifdef _CONTROLPANEL_
ControlPanelService* controlPanelService = NULL;
ControlPanelControllee* controlPanelControllee = NULL;
#endif

static void SigIntHandler(int sig) {
    s_interrupt = true;
}

static void daemonDisconnectCB()
{
    QCC_DbgHLPrintf(("Received daemonDisconnectCB"));
    s_restart = true;
}

static void cleanup() {

    if (AboutObjApi::getInstance()) {
        AboutObjApi::DestroyInstance();
    }

    if (keyListener) {
        delete keyListener;
        keyListener = NULL;
    }

    if (aboutIconObj) {
        delete aboutIconObj;
        aboutIconObj = NULL;
    }

    if (icon) {
        delete icon;
        icon = NULL;
    }

    if (aboutDataStore) {
        delete aboutDataStore;
        aboutDataStore = NULL;
    }

    if (aboutObj) {
        delete aboutObj;
        aboutObj = NULL;
    }

    if (busListener) {
        if (msgBus) {
            msgBus->UnregisterBusListener(*busListener);
        }
        delete busListener;
        busListener = NULL;
    }

#ifdef _CONFIG_
    if (configService) {
        delete configService;
        configService = NULL;
    }

    if (configServiceListenerImpl) {
        delete configServiceListenerImpl;
        configServiceListenerImpl = NULL;
    }
#endif

#ifdef _NOTIFICATION_

#ifdef _TESTMODE_
    if (notifTester) {
        delete notifTester;
        notifTester = NULL;
    }
#endif

    if (prodService) {
        prodService->shutdown();
        prodService = NULL;
    }
    if (sender) {
        delete sender;
        sender = NULL;
    }
#endif

#ifdef _CONTROLPANEL_
    if (controlPanelService) {
        controlPanelService->shutdownControllee();
    }
    ControlPanelGenerated::Shutdown();
    if (controlPanelControllee) {
        delete controlPanelControllee;
        controlPanelControllee = NULL;
    }
    if (controlPanelService) {
        delete controlPanelService;
        controlPanelService = NULL;
    }
#endif

    /* Clean up msg bus */
    if (msgBus) {
        delete msgBus;
        msgBus = NULL;
    }
}

void readPassword(qcc::String& passCode) {

    ajn::MsgArg*argPasscode;
    char*tmp;
    aboutDataStore->GetField("Passcode", argPasscode);
    argPasscode->Get("s", &tmp);
    passCode = tmp;
    return;
}

bool WaitForSigInt(int32_t sleepTime) {
    if (s_interrupt == false && s_restart == false) {
#ifdef _WIN32
        Sleep(sleepTime * 1000);
#else
        sleep(sleepTime);
#endif
        return false;
    }
    return true;
}

#ifdef _NOTIFICATION_
void FillNotification(NotificationMessageType& messageType, std::vector<NotificationText>& vecMessages, uint16_t& ttl, int32_t& sleepTime)
{
    messageType =  NotificationMessageType(INFO);
    NotificationText textToSend("en", "Using the default text.");
    vecMessages.push_back(textToSend);
    ttl =  30;
    sleepTime = 5;
}
#endif

#define CHECK_RETURN(x) if ((status = x) != ER_OK) { return status; }
QStatus fillAboutData(AboutData* aboutdata)
{
    if (!aboutdata) {
        return ER_BAD_ARG_1;
    }

    QStatus status = ER_OK;

#ifdef _CONTROLPANEL_
    std::vector<qcc::String> languagesVec;
    LanguageSet* langSet = LanguageSets::get("myDeviceMyLanguages");
    if (langSet) {
        const std::vector<qcc::String> languages(langSet->getLanguages());
        languagesVec.resize(languages.size());
        printf("AllJoyn Library version: %s\n", ajn::GetVersion());
        for (size_t i = 0; i < languagesVec.size(); i++) {
            for (size_t j = 0; j < languages[i].size(); j++)
                languagesVec[i].append((languages[i][j] == '_' ? '-' : languages[i][j]));
            printf("AllJoyn languagesVec[%lu]: %s\n", static_cast<long unsigned int>(i), languagesVec[i].c_str());
        }
    } else {
        languagesVec.reserve(3);
        languagesVec.push_back("en");
        languagesVec.push_back("de-AT");
        languagesVec.push_back("zh-Hans-CN");
    }
#else
    std::vector<qcc::String> languagesVec(3);
    languagesVec[0] = "en";
    languagesVec[1] = "de-AT";
    languagesVec[2] = "zh-Hans-CN";
#endif
    for (size_t i = 0; i < languagesVec.size(); i++) {
        CHECK_RETURN(aboutdata->SetSupportedLanguage(languagesVec[i].c_str()))
    }

    CHECK_RETURN(aboutdata->SetDeviceName("My device name", languagesVec[0].c_str()));
    CHECK_RETURN(aboutdata->SetDeviceName("Mein Gerätname", languagesVec[1].c_str()));
    CHECK_RETURN(aboutdata->SetDeviceName("我的設備名稱", languagesVec[2].c_str()));

    CHECK_RETURN(aboutdata->SetModelNumber("Wxfy388i"))
    CHECK_RETURN(aboutdata->SetDateOfManufacture("10/1/2199"))
    CHECK_RETURN(aboutdata->SetSoftwareVersion("12.20.44 build 44454"))
    CHECK_RETURN(aboutdata->SetHardwareVersion("355.499. b"))
    CHECK_RETURN(aboutdata->SetDescription("This is an Alljoyn Application", languagesVec[0].c_str()))
    CHECK_RETURN(aboutdata->SetDescription("This is an Alljoyn Application", languagesVec[1].c_str()))
    CHECK_RETURN(aboutdata->SetDescription("This is an Alljoyn Application", languagesVec[2].c_str()))
    CHECK_RETURN(aboutdata->SetManufacturer("Company", languagesVec[0].c_str()))
    CHECK_RETURN(aboutdata->SetManufacturer("Company", languagesVec[1].c_str()))
    CHECK_RETURN(aboutdata->SetManufacturer("Company", languagesVec[2].c_str()))
    CHECK_RETURN(aboutdata->SetSupportUrl("http://www.alljoyn.org"))

    if (!aboutdata->IsValid()) {
        printf("failed to setup about data.\n");
        return ER_FAIL;
    }
    return status;
}

int main(int argc, char**argv, char**envArg) {

    QStatus status = ER_OK;
    printf("AllJoyn Library version: %s\n", ajn::GetVersion());
    printf("AllJoyn Library build info: %s\n", ajn::GetBuildInfo());
    QCC_SetLogLevels("ALLJOYN_ABOUT_SERVICE=7;");
    QCC_SetLogLevels("ALLJOYN_ABOUT_ICON_SERVICE=7;");

    OptParser opts(argc, argv);
    OptParser::ParseResultCode parseCode(opts.ParseResult());
    switch (parseCode) {
    case OptParser::PR_OK:
        break;

    case OptParser::PR_EXIT_NO_ERROR:
        return SERVICE_EXIT_OK;

    default:
        return SERVICE_OPTION_ERROR;
    }

    printf("using port %d\n", servicePort);

    if (!opts.GetConfigFile().empty()) {
        printf("using Config-file %s\n", opts.GetConfigFile().c_str());
    }

    /* Install SIGINT handler so Ctrl + C deallocates memory properly */
    signal(SIGINT, SigIntHandler);

    //set Daemon password only for bundled app
    #ifdef QCC_USING_BD
    PasswordManager::SetCredentials("ALLJOYN_PIN_KEYX", "000000");
    #endif

start:
    std::cout << "Initializing application." << std::endl;

    /* Create message bus */
    keyListener = new SrpKeyXListener();
    uint16_t retry = 0;
    do {
        msgBus = CommonSampleUtil::prepareBusAttachment(keyListener);
        if (msgBus == NULL) {
            std::cout << "Could not initialize BusAttachment. Retrying" << std::endl;
#ifdef _WIN32
            Sleep(1000);
#else
            sleep(1);
#endif
            retry++;
        }
    } while (msgBus == NULL && retry != 180 && !s_interrupt);
    if (msgBus == NULL) {
        std::cout << "Could not initialize BusAttachment." << std::endl;
        cleanup();
        return 1;
    }

    busListener = new CommonBusListener(msgBus, daemonDisconnectCB);
    busListener->setSessionPort(servicePort);

#ifdef _CONTROLPANEL_
    status = ControlPanelGenerated::PrepareWidgets(controlPanelControllee);
    if (status != ER_OK) {
        std::cout << "Could not prepare Widgets." << std::endl;
        cleanup();
        return 1;
    }
#endif

    aboutDataStore = new AboutDataStore(opts.GetFactoryConfigFile().c_str(), opts.GetConfigFile().c_str());
    fillAboutData(aboutDataStore);
    aboutDataStore->Initialize();
    if (!opts.GetAppId().empty()) {
        std::cout << "using appID " << opts.GetAppId().c_str() << std::endl;
        aboutDataStore->SetAppId(opts.GetAppId().c_str());
    }

    if (status != ER_OK) {
        std::cout << "Could not fill About Data." << std::endl;
        cleanup();
        return 1;
    }

    aboutObj = new ajn::AboutObj(*msgBus, BusObject::ANNOUNCED);
    status = CommonSampleUtil::prepareAboutService(msgBus, aboutDataStore, aboutObj, busListener, servicePort);
    if (status != ER_OK) {
        std::cout << "Could not set up the AboutService." << std::endl;
        cleanup();
        return 1;
    }

    AboutObjApi* aboutObjApi = AboutObjApi::getInstance();
    if (!aboutObjApi) {
        std::cout << "Could not set up the AboutService." << std::endl;
        cleanup();
        return 1;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //aboutIconService
    uint8_t aboutIconContent[] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52 \
                                   , 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x32, 0x02, 0x03, 0x00, 0x00, 0x00, 0x63, 0x51, 0x60 \
                                   , 0x22, 0x00, 0x00, 0x00, 0x0c, 0x50, 0x4c, 0x54, 0x45, 0x65, 0x2d, 0x67, 0xeb, 0x00, 0x88, 0xec \
                                   , 0x06, 0x8d, 0xf1, 0x44, 0xaa, 0x1f, 0x54, 0xd3, 0x5b, 0x00, 0x00, 0x00, 0x01, 0x74, 0x52, 0x4e \
                                   , 0x53, 0x00, 0x40, 0xe6, 0xd8, 0x66, 0x00, 0x00, 0x00, 0xe8, 0x49, 0x44, 0x41, 0x54, 0x28, 0xcf \
                                   , 0x95, 0x92, 0x51, 0x0a, 0xc3, 0x20, 0x0c, 0x86, 0xd3, 0xc0, 0x40, 0x84, 0xdd, 0x61, 0xec, 0x49 \
                                   , 0x72, 0x9f, 0xde, 0x47, 0xfa, 0x34, 0x3c, 0x45, 0x18, 0x85, 0x49, 0x4e, 0xb9, 0x18, 0xab, 0xd6 \
                                   , 0xb1, 0x97, 0x06, 0xb4, 0x7e, 0xa6, 0x26, 0xbf, 0x89, 0x00, 0x97, 0xec, 0xb6, 0x9e, 0xc9, 0x8b \
                                   , 0x0e, 0xee, 0x04, 0x40, 0x92, 0x1b, 0x49, 0x04, 0x7a, 0xcb, 0x01, 0x28, 0x20, 0xc4, 0xd4, 0x7c \
                                   , 0x0f, 0x90, 0x11, 0x04, 0x39, 0xd0, 0x29, 0x24, 0xd3, 0x39, 0x41, 0x0c, 0x53, 0x3e, 0x4c, 0x1b \
                                   , 0x4b, 0x4f, 0x87, 0x29, 0x65, 0x49, 0x7b, 0x89, 0x01, 0x64, 0x91, 0x44, 0xf6, 0x2a, 0xc4, 0x26 \
                                   , 0xf1, 0x1f, 0x5d, 0x10, 0xbb, 0xba, 0xe5, 0x77, 0x93, 0x15, 0x4c, 0x40, 0xb5, 0x64, 0xc1, 0x9a \
                                   , 0x66, 0x37, 0x91, 0x2d, 0x10, 0xda, 0xf5, 0x9e, 0xba, 0xc0, 0xad, 0x39, 0x31, 0xea, 0xc0, 0xfe \
                                   , 0xab, 0x2b, 0x5b, 0x9d, 0x42, 0x11, 0x3e, 0xd0, 0x68, 0x5c, 0x18, 0x13, 0x74, 0xf2, 0x01, 0x4b \
                                   , 0x71, 0xea, 0x95, 0x3d, 0x05, 0x56, 0xcc, 0x5a, 0xb9, 0xb2, 0x19, 0x20, 0xfb, 0xa8, 0x5f, 0x3e \
                                   , 0x0a, 0xcd, 0xc4, 0x07, 0x89, 0xd3, 0x84, 0xcd, 0xb7, 0xa8, 0x8b, 0x4c, 0x4f, 0x39, 0xb7, 0x68 \
                                   , 0xd6, 0x1a, 0xbc, 0xcc, 0xf7, 0x58, 0x7c, 0xad, 0x43, 0x77, 0x8d, 0xf3, 0xd2, 0x72, 0x0c, 0xd2 \
                                   , 0x16, 0x0d, 0x95, 0x34, 0x91, 0xfa, 0x46, 0x67, 0x21, 0x45, 0xcb, 0xd0, 0x1a, 0x56, 0xc7, 0x41 \
                                   , 0x7a, 0xc6, 0xe7, 0x89, 0xe4, 0x3f, 0x81, 0x51, 0xfc, 0x79, 0x3f, 0xc3, 0x96, 0xf5, 0xda, 0x5b \
                                   , 0x84, 0x2f, 0x85, 0x3b, 0x47, 0x0d, 0xe8, 0x0d, 0xca, 0xd3, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45 \
                                   , 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82, 0x82 };

    qcc::String mimeType("image/png");
    icon = new ajn::AboutIcon();
    status = icon->SetContent(mimeType.c_str(), aboutIconContent, sizeof(aboutIconContent) / sizeof(*aboutIconContent));
    if (ER_OK != status) {
        printf("Failed to setup the AboutIcon.\n");
    }

    aboutIconObj = new ajn::AboutIconObj(*msgBus, *icon);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //ConfigService
#ifdef _CONFIG_
    configServiceListenerImpl = new ConfigServiceListenerImpl(*aboutDataStore, *msgBus, *busListener);
    configService = new ConfigService(*msgBus, *aboutDataStore, *configServiceListenerImpl);
    configFile = opts.GetConfigFile().c_str();
    keyListener->setGetPassCode(readPassword);

    status = configService->Register();
    if (status != ER_OK) {
        std::cout << "Could not register the ConfigService." << std::endl;
        cleanup();
        return 1;
    }

    status = msgBus->RegisterBusObject(*configService);
    if (status != ER_OK) {
        std::cout << "Could not register the ConfigService BusObject." << std::endl;
        cleanup();
        return 1;
    }
#endif

    //////////////////////////////////////////////////////////////////////////////////////////////////// controlpanel
#ifdef _CONTROLPANEL_

    controlPanelService = ControlPanelService::getInstance();
    QCC_SetDebugLevel(logModules::CONTROLPANEL_MODULE_LOG_NAME, logModules::ALL_LOG_LEVELS);

    status = controlPanelService->initControllee(msgBus, controlPanelControllee);
    if (status != ER_OK) {
        std::cout << "Could not initialize Controllee." << std::endl;
        cleanup();
        return 1;
    }
#endif

#ifdef _NOTIFICATION_
    QCC_SetDebugLevel(logModules::NOTIFICATION_MODULE_LOG_NAME, logModules::ALL_LOG_LEVELS);

#ifdef _TESTMODE_
    if (!notifTester->Initialize(msgBus, aboutDataStore)) {
        std::cout << "Could not initialize NotificationTester - exiting application" << std::endl;
        cleanup();
        return 1;
    }
#else
    // Initialize Service object and Sender Object
    prodService = NotificationService::getInstance();
    QCC_SetDebugLevel(logModules::NOTIFICATION_MODULE_LOG_NAME, logModules::ALL_LOG_LEVELS);
    sender = prodService->initSend(msgBus, aboutDataStore);

    if (!sender) {
        std::cout << "Could not initialize Sender - exiting application" << std::endl;
        cleanup();
        return 1;
    }
#endif

#endif

    status = CommonSampleUtil::aboutServiceAnnounce();
    if (status != ER_OK) {
        std::cout << "Could not announce." << std::endl;
        cleanup();
        return 1;
    }

    std::cout << "Sent announce, waiting for Remote Devices" << std::endl;

    //Run in loop until press enter
    while (1) {
        int32_t sleepTime = 5;

#ifdef _NOTIFICATION_

#ifdef _TESTMODE_
        sleepTime = notifTester->LoopHandler();
#else
        std::cout << "Press any key to send a notification" << std::endl;
        getchar();

        NotificationMessageType messageType = NotificationMessageType(INFO);
        std::vector<NotificationText> vecMessages;
        uint16_t ttl;

        FillNotification(messageType, vecMessages, ttl, sleepTime);
        Notification notification(messageType, vecMessages);

        if (sender->send(notification, ttl) != ER_OK) {
            std::cout << "Could not send the message successfully. Sleeping 5 seconds" << std::endl;
        } else {
            std::cout << "Notification sent. Sleeping 5 seconds" << std::endl;
        }
#endif
#endif

        if (WaitForSigInt(sleepTime)) {
            break;
        }

    }
    //////////////////////////////////////////////////////////////////////////////////////////////////// controlpanel

    cleanup();

    if (s_restart) {
        QCC_DbgHLPrintf(("restarting..."));
        s_restart = false;
        goto start;
    }

    return 0;
} /* main() */


