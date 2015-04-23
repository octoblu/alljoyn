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

#include <stdio.h>
#include <signal.h>
#include <fstream>
#include <map>
#include <stdint.h>

#include <SrpKeyXListener.h>
#include <CommonBusListener.h>
#include <CommonSampleUtil.h>
#include <OptParser.h>
#include "ConfigServiceListenerImpl.h"

#include <alljoyn/version.h>
#include <qcc/platform.h>
#include <qcc/Log.h>
#include <qcc/String.h>

#include <alljoyn/BusObject.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/Status.h>

#include <alljoyn/config/ConfigService.h>

#include <alljoyn/onboarding/Onboarding.h>
#include <alljoyn/onboarding/OnboardingService.h>
#include <OnboardingControllerImpl.h>
#include <alljoyn/services_common/LogModulesNames.h>
#include <alljoyn/AboutIconObj.h>
#include <AboutDataStore.h>
#include <alljoyn/AboutObj.h>

using namespace ajn;
using namespace services;

#define SERVICE_EXIT_OK       0
#define SERVICE_OPTION_ERROR  1
#define SERVICE_CONFIG_ERROR  2

/** static variables need for sample */
static BusAttachment* msgBus = NULL;

static SrpKeyXListener* keyListener = NULL;

static ConfigService* configService = NULL;

static AboutIcon* icon = NULL;
static AboutIconObj* aboutIconObj = NULL;
static AboutDataStore* aboutDataStore = NULL;
static AboutObj* aboutObj = NULL;

static ConfigServiceListenerImpl* configServiceListener = NULL;

static OnboardingControllerImpl* obController = NULL;

OnboardingService* onboardingService = NULL;

static CommonBusListener* busListener = NULL;

static SessionPort servicePort = 900;

static volatile sig_atomic_t s_interrupt = false;

static volatile sig_atomic_t s_restart = false;

static void SigIntHandler(int sig) {
    s_interrupt = true;
}

static void daemonDisconnectCB()
{
    s_restart = true;
}

static void cleanup() {

    if (AboutObjApi::getInstance()) {
        AboutObjApi::DestroyInstance();
    }

    if (configService) {
        delete configService;
        configService = NULL;
    }

    if (configServiceListener) {
        delete configServiceListener;
        configServiceListener = NULL;
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

    if (onboardingService) {
        delete onboardingService;
        onboardingService = NULL;
    }

    if (obController) {
        delete obController;
        obController = NULL;
    }

    if (busListener) {
        if (msgBus) {
            msgBus->UnregisterBusListener(*busListener);
        }
        delete busListener;
        busListener = NULL;
    }

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

/** Advertise the service name, report the result to stdout, and return the status code. */
QStatus AdvertiseName(TransportMask mask) {
    QStatus status = ER_BUS_ESTABLISH_FAILED;
    if (msgBus->IsConnected() && msgBus->GetUniqueName().size() > 0) {
        status = msgBus->AdvertiseName(msgBus->GetUniqueName().c_str(), mask);
        std::cout << "AdvertiseName for " << msgBus->GetUniqueName().c_str() << " = " << QCC_StatusText(status) << std::endl;
    }
    return status;
}

void WaitForSigInt(void) {
    while (s_interrupt == false && s_restart == false) {
#ifdef _WIN32
        Sleep(100);
#else
        usleep(100 * 1000);
#endif
    }
}

int main(int argc, char**argv, char**envArg) {
    QStatus status = ER_OK;
    std::cout << "AllJoyn Library version: " << ajn::GetVersion() << std::endl;
    std::cout << "AllJoyn Library build info: " << ajn::GetBuildInfo() << std::endl;
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

    std::cout << "using port " << servicePort << std::endl;

    if (!opts.GetConfigFile().empty()) {
        std::cout << "using Config-file " << opts.GetConfigFile().c_str() << std::endl;
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

    /* Connect to the daemon */
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

    aboutDataStore = new AboutDataStore(opts.GetFactoryConfigFile().c_str(), opts.GetConfigFile().c_str());
    aboutDataStore->SetOBCFG();
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
    status = CommonSampleUtil::prepareAboutService(msgBus, static_cast<AboutData*>(aboutDataStore), aboutObj, busListener, servicePort);
    if (status != ER_OK) {
        std::cout << "Could not set up the AboutService." << std::endl;
        cleanup();
        return 1;
    }

    AboutObjApi* aboutObjApi = AboutObjApi::getInstance();
    if (!aboutObjApi) {
        std::cout << "Could not set up the AboutObjApi." << std::endl;
        cleanup();
        return 1;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //aboutIconService
    uint8_t aboutIconContent[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x0A,
                                   0x00, 0x00, 0x00, 0x0A, 0x08, 0x02, 0x00, 0x00, 0x00, 0x02, 0x50, 0x58, 0xEA, 0x00, 0x00, 0x00, 0x04, 0x67, 0x41, 0x4D, 0x41, 0x00, 0x00, 0xAF,
                                   0xC8, 0x37, 0x05, 0x8A, 0xE9, 0x00, 0x00, 0x00, 0x19, 0x74, 0x45, 0x58, 0x74, 0x53, 0x6F, 0x66, 0x74, 0x77, 0x61, 0x72, 0x65, 0x00, 0x41, 0x64,
                                   0x6F, 0x62, 0x65, 0x20, 0x49, 0x6D, 0x61, 0x67, 0x65, 0x52, 0x65, 0x61, 0x64, 0x79, 0x71, 0xC9, 0x65, 0x3C, 0x00, 0x00, 0x00, 0x18, 0x49, 0x44,
                                   0x41, 0x54, 0x78, 0xDA, 0x62, 0xFC, 0x3F, 0x95, 0x9F, 0x01, 0x37, 0x60, 0x62, 0xC0, 0x0B, 0x46, 0xAA, 0x34, 0x40, 0x80, 0x01, 0x00, 0x06, 0x7C,
                                   0x01, 0xB7, 0xED, 0x4B, 0x53, 0x2C, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82 };

    qcc::String mimeType("image/png");
    icon = new ajn::AboutIcon();
    status = icon->SetContent(mimeType.c_str(), aboutIconContent, sizeof(aboutIconContent) / sizeof(*aboutIconContent));
    if (ER_OK != status) {
        printf("Failed to setup the AboutIcon.\n");
    }

    aboutIconObj = new ajn::AboutIconObj(*msgBus, *icon);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //OnboardingService

    ajn::MsgArg*arg;
    char*ScanFile, *StateFile, *ErrorFile, *ConfigureCmd, *ConnectCmd, *OffboardCmd, *ScanCmd;
    aboutDataStore->GetField("scan_file", arg);
    arg->Get("s", &ScanFile);
    aboutDataStore->GetField("state_file", arg);
    arg->Get("s", &StateFile);
    aboutDataStore->GetField("error_file", arg);
    arg->Get("s", &ErrorFile);
    aboutDataStore->GetField("connect_cmd", arg);
    arg->Get("s", &ConnectCmd);
    aboutDataStore->GetField("offboard_cmd", arg);
    arg->Get("s", &OffboardCmd);
    aboutDataStore->GetField("configure_cmd", arg);
    arg->Get("s", &ConfigureCmd);
    aboutDataStore->GetField("scan_cmd", arg);
    arg->Get("s", &ScanCmd);

    obController = new OnboardingControllerImpl(ScanFile,
                                                StateFile,
                                                ErrorFile,
                                                ConfigureCmd,
                                                ConnectCmd,
                                                OffboardCmd,
                                                ScanCmd,
                                                (OBConcurrency)0,
                                                *msgBus);
    onboardingService = new OnboardingService(*msgBus, *obController);

    status = onboardingService->Register();
    if (status != ER_OK) {
        std::cout << "Could not register the OnboardingService." << std::endl;
        cleanup();
        return 1;
    }

    status = msgBus->RegisterBusObject(*onboardingService);
    if (status != ER_OK) {
        std::cout << "Could not register the OnboardingService BusObject." << std::endl;
        cleanup();
        return 1;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //ConfigService

    configServiceListener = new ConfigServiceListenerImpl(*aboutDataStore, *msgBus, *busListener, *obController);
    configService = new ConfigService(*msgBus, *aboutDataStore, *configServiceListener);
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
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    const TransportMask SERVICE_TRANSPORT_TYPE = TRANSPORT_ANY;

    if (ER_OK == status) {
        status = AdvertiseName(SERVICE_TRANSPORT_TYPE);
    }

    if (ER_OK == status) {
        status = aboutObjApi->Announce();
    }

    /* Perform the service asynchronously until the user signals for an exit. */
    if (ER_OK == status) {
        WaitForSigInt();
    }

    cleanup();

    if (s_restart) {
        s_restart = false;
        goto start;
    }

    return 0;
} /* main() */

