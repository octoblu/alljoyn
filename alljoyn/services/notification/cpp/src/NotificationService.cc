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

#include <algorithm>
#include <alljoyn/notification/NotificationSender.h>
#include <alljoyn/notification/NotificationService.h>
#include "NotificationConstants.h"
#include "Transport.h"
#include "NotificationProducerReceiver.h"
#include <alljoyn/notification/LogModule.h>

using namespace ajn;
using namespace services;
using namespace qcc;
using namespace nsConsts;

NotificationService* NotificationService::s_Instance(NULL);
uint16_t const NotificationService::NOTIFICATION_SERVICE_VERSION = 2;

NotificationService* NotificationService::getInstance()
{
    if (!s_Instance) {
        s_Instance = new NotificationService();
    }
    return s_Instance;
}

NotificationService::NotificationService()
{
}

NotificationService::~NotificationService()
{

}

uint16_t NotificationService::getVersion()
{
    return NOTIFICATION_SERVICE_VERSION;
}

NotificationSender* NotificationService::initSend(BusAttachment* bus, PropertyStore* store)
{
    QCC_DbgTrace(("NotificationService::initSend"));

    if (!bus) {
        QCC_DbgHLPrintf(("BusAttachment cannot be NULL"));
        return NULL;
    }

    if (!store) {
        QCC_DbgHLPrintf(("PropertyStore cannot be NULL"));
        return NULL;
    }

    Transport* transport = Transport::getInstance();
    if (transport->startSenderTransport(bus) != ER_OK) {
        return NULL;
    }

    MsgArg configArgs[1];
    MsgArg* configEntries;
    size_t configNum;
    QStatus status;

    if ((status = store->ReadAll(0, PropertyStore::READ, configArgs[0]))) {
        QCC_LogError(status, ("Error reading all in configuration data"));
    }

    if ((status = configArgs[0].Get(AJPARAM_ARR_DICT_STR_VAR.c_str(), &configNum, &configEntries))) {
        QCC_LogError(status, ("Error reading in configuration data"));
    }

    MsgArg appIdArg;
    for (size_t i = 0; i < configNum; i++) {
        char* keyChar;
        String key;
        MsgArg* variant;

        status = configEntries[i].Get(AJPARAM_DICT_STR_VAR.c_str(), &keyChar, &variant);
        if (status != ER_OK) {
            QCC_LogError(status, ("Error reading in configuration data"));
        }

        key = keyChar;

        if (key.compare("AppId") == 0) {
            appIdArg = *variant;
        }
    }

    if (status != ER_OK) {
        QCC_LogError(status, ("Something went wrong unmarshalling the propertystore."));
        return NULL;
    }

    if (appIdArg.typeId != ALLJOYN_BYTE_ARRAY) {
        QCC_DbgHLPrintf(("ApplicationId argument is not correct type."));
        return NULL;
    }

    transport->getNotificationProducerReceiver()->SetAppIdArg(appIdArg);

    return new NotificationSender(store);
}

QStatus NotificationService::initReceive(ajn::BusAttachment* bus, NotificationReceiver* notificationReceiver)
{
    if (!bus) {
        QCC_LogError(ER_BAD_ARG_1, ("BusAttachment cannot be NULL."));
        return ER_BAD_ARG_1;
    }

    if (!notificationReceiver) {
        QCC_LogError(ER_BAD_ARG_2, ("Could not set NotificationReceiver to null pointer"));
        return ER_BAD_ARG_2;
    }

    QCC_DbgPrintf(("Init receive"));

    Transport* transport = Transport::getInstance();
    transport->setNotificationReceiver(notificationReceiver);

    QStatus status;
    if ((status = transport->startReceiverTransport(bus)) != ER_OK) {
        transport->setNotificationReceiver(0);
    }

    return status;
}

void NotificationService::shutdownSender()
{
    QCC_DbgTrace(("Stop Sender"));
    Transport* transport = Transport::getInstance();
    transport->cleanupSenderTransport();
}

void NotificationService::shutdownReceiver()
{
    QCC_DbgTrace(("Stop Receiver"));
    Transport* transport = Transport::getInstance();
    transport->cleanupReceiverTransport();
}

void NotificationService::shutdown()
{
    QCC_DbgTrace(("Shutdown"));
    Transport* transport = Transport::getInstance();
    transport->cleanup();

    s_Instance = 0;
    delete this;
}

QStatus NotificationService::disableSuperAgent()
{
    QCC_DbgTrace(("Disabling SuperAgent"));
    Transport* transport = Transport::getInstance();
    return transport->disableSuperAgent();
}


BusAttachment* NotificationService::getBusAttachment()
{
    QCC_DbgTrace(("In Get BusAttachment"));
    Transport* transport = Transport::getInstance();
    return transport->getBusAttachment();
}
