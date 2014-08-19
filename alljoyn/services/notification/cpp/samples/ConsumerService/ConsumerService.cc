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

#include <iostream>
#include <signal.h>
#include <alljoyn/PasswordManager.h>
#include <alljoyn/notification/NotificationService.h>
#include "../common/NotificationReceiverTestImpl.h"
#include "CommonSampleUtil.h"
#include <alljoyn/about/AnnouncementRegistrar.h>
#include <alljoyn/services_common/LogModulesNames.h>

using namespace ajn;
using namespace services;

NotificationService* conService = 0;
NotificationReceiverTestImpl* Receiver = 0;
ajn::BusAttachment* busAttachment = 0;
static volatile sig_atomic_t s_interrupt = false;

void cleanup()
{
    std::cout << "cleanup() - start" << std::endl;
    if (conService) {
        std::cout << "cleanup() - conService" << std::endl;
        conService->shutdown();
        conService = NULL;
    }
    if (Receiver) {
        std::cout << "cleanup() - Receiver" << std::endl;
        delete Receiver;
        Receiver = NULL;
    }
    if (busAttachment) {
        std::cout << "cleanup() - busAttachment" << std::endl;
        delete busAttachment;
        busAttachment = NULL;
    }
    std::cout << "cleanup() - end" << std::endl;
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
    std::cout << "Getting out from WaitForSigInt()" << std::endl;
}

int main()
{
    std::string listOfApps;

    // Allow CTRL+C to end application
    signal(SIGINT, signal_callback_handler);

    std::cout << "Begin Consumer Application. (Press CTRL+C to end application)" << std::endl;
    std::cout << "Enter in a list of app names (separated by ';') you would like to receive notifications from." << std::endl;
    std::cout << "Empty list means all app names." << std::endl;
    std::getline(std::cin, listOfApps);

    // Initialize Service object and send it Notification Receiver object
    conService = NotificationService::getInstance();
//set Daemon passowrd only for bundled app
#ifdef QCC_USING_BD
    PasswordManager::SetCredentials("ALLJOYN_PIN_KEYX", "000000");
#endif

    // change loglevel to debug:
    QCC_SetLogLevels("ALLJOYN_ABOUT_CLIENT=7");
    QCC_SetLogLevels("ALLJOYN_ABOUT_ICON_CLIENT=7");
    QCC_SetLogLevels("ALLJOYN_ABOUT_ANNOUNCE_HANDLER=7");
    QCC_SetLogLevels("ALLJOYN_ABOUT_ANNOUNCEMENT_REGISTRAR=7");

    QCC_SetDebugLevel(logModules::NOTIFICATION_MODULE_LOG_NAME, logModules::ALL_LOG_LEVELS);

    Receiver = new NotificationReceiverTestImpl(false);

    // Set the list of applications this receiver should receive notifications from
    Receiver->setApplications(listOfApps.c_str());

    busAttachment = CommonSampleUtil::prepareBusAttachment();
    if (busAttachment == NULL) {
        std::cout << "Could not initialize BusAttachment." << std::endl;
        cleanup();
        return 1;
    }
    QStatus status;

    status = conService->initReceive(busAttachment, Receiver);
    if (status != ER_OK) {
        std::cout << "Could not initialize receiver." << std::endl;
        cleanup();
        return 1;
    }

    std::cout << "Waiting for notifications." << std::endl;

    WaitForSigInt();

    cleanup();
    return 0;
}
