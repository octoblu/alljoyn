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

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <alljoyn/notification/NotificationText.h>
#include <alljoyn/notification/RichAudioUrl.h>
#include <alljoyn/Status.h>
#include <iostream>
#include <algorithm>
#include <sstream>
#include "NotificationTesterImpl.h"

using namespace ajn;
using namespace services;
using namespace qcc;

#include <qcc/Debug.h>
#include <qcc/time.h>
#include <qcc/Util.h>

static char const* const QCC_MODULE = "ServerSample";

NotificationTesterImpl::NotificationTesterImpl() {
    notifProdEnabled = true;
    notifInterval = 30000;
    notifIntervalOffset = 2000;
    notifRateType = RATE_RANDOM;
    notifRatePriority = 0;
    notifRatePriorityType = PRIORITY_FIXED;
    notifTTL = 30;
    notifCustEnabled = false;
    notifMsgText.assign("Notification Text");
    notifMsgLang.assign("en");

    nextMessageTime = 0;
    intervalOffset = 0;
    prodService = NULL;
    receiver = NULL;
    sender = NULL;
}

NotificationTesterImpl::~NotificationTesterImpl() {
    Cleanup();
}

void NotificationTesterImpl::Cleanup() {
    if (prodService) {
        prodService->shutdown();
        prodService = NULL;
    }
    if (sender) {
        delete sender;
        sender = NULL;
    }
    if (receiver) {
        delete receiver;
        receiver = NULL;
    }
}

void NotificationTesterImpl::LoadConfiguration() {
    const char* value = getenv("AJNS_PRODUCER_ENABLED");
    if (value != NULL) {
        notifProdEnabled = (0 == strcmp(value, "true"));
        std::cout << "Read AJNS_PRODUCER_ENABLED from environment: " << (notifProdEnabled ? "true" : "false") << std::endl;
    }
    value = getenv("AJNS_MSG_INTERVAL");
    if (value != NULL) {
        notifInterval = atoi(value);
        std::cout << "Read AJNS_MSG_INTERVAL from environment: " << notifInterval << "ms" << std::endl;
    }
    value = getenv("AJNS_MSG_INTERVAL_OFFSET");
    if (value != NULL) {
        notifIntervalOffset = (uint32_t)atoi(value);
        std::cout << "Read AJNS_MSG_INTERVAL_OFFSET from environment: " << notifIntervalOffset << "ms" << std::endl;
    }
    value = getenv("AJNS_MSG_INTERVAL_TYPE");
    if (value != NULL) {
        notifRateType = (0 == strcmp("random", value)) ? RATE_RANDOM : RATE_FIXED;
        std::cout << "Read AJNS_MSG_INTERVAL_TYPE from environment: " << (notifRateType == RATE_RANDOM ? "random" : "fixed") << std::endl;
    }
    value = getenv("AJNS_MSG_PRIORITY");
    if (value != NULL) {
        notifRatePriority = atoi(value);
        std::cout << "Read AJNS_MSG_PRIORITY from environment: " << notifRatePriority << std::endl;
    }
    value = getenv("AJNS_MSG_PRIORITY_TYPE");
    if (value != NULL) {
        notifRatePriorityType = (0 == strcmp("random", value)) ? PRIORITY_RANDOM : PRIORITY_FIXED;
        std::cout << "Read AJNS_MSG_PRIORITY_TYPE from environment: " << (notifRatePriorityType == PRIORITY_RANDOM ? "random" : "fixed") << std::endl;
    }
    value = getenv("AJNS_MSG_TEXT");
    if (value != NULL) {
        notifMsgText = value;
        std::cout << "Read AJNS_MSG_TEXT from environment: " << notifMsgText << std::endl;
    }
    value = getenv("AJNS_MSG_LANG");
    if (value != NULL) {
        notifMsgLang = value;
        std::cout << "Read AJNS_MSG_LANG from environment: " << notifMsgLang << std::endl;
    }
    value = getenv("AJNS_MSG_TTL");
    if (value != NULL) {
        notifTTL = atoi(value);
        std::cout << "Read AJNS_MSG_TTL from environment: " << notifTTL << " secs" << std::endl;
    }
    value = getenv("AJNS_CONSUMER_ENABLED");
    if (value != NULL) {
        notifCustEnabled = (0 == strcmp(value, "true"));
        std::cout << "Read AJNS_CONSUMER_ENABLED from environment: " << (notifCustEnabled ? "true" : "false") << std::endl;
    }
}

bool NotificationTesterImpl::Initialize(ajn::BusAttachment* bus, AboutDataStore* store)
{
    LoadConfiguration();

    std::cout << "---------------------" << std::endl;
    std::cout << "Notification parameters" << std::endl;
    if (notifProdEnabled) {
        std::cout << "Producer:     enabled" << std::endl;
        std::cout << "Interval:     " << notifInterval << " ms" << std::endl;
        std::cout << "Offset:       " << notifIntervalOffset << " ms" << std::endl;
        std::cout << "IntervalType: " << (notifRateType == RATE_RANDOM ? "random" : "fixed") << std::endl;
        std::cout << "Priority:     " << notifRatePriority << std::endl;
        std::cout << "PriorityType: " << ((notifRatePriorityType == PRIORITY_RANDOM) ? "random" : "fixed") << std::endl;
        std::cout << "TTL:          " << notifTTL << std::endl;
        std::cout << "MsgLang:      " << notifMsgLang << std::endl;
        std::cout << "MsgText:      " << notifMsgText << std::endl;
    } else {
        std::cout << "Producer:     disabled" << std::endl;
    }
    if (notifCustEnabled) {
        std::cout << "Consumer:     enabled" << std::endl;
    } else {
        std::cout << "Consumer:     disabled" << std::endl;
    }
    std::cout << "---------------------" << std::endl;

    prodService = NotificationService::getInstance();

    if (notifProdEnabled) {
        sender = prodService->initSend(bus, store);
        if (!sender) {
            std::cout << "Could not initialize Sender - exiting application" << std::endl;
            Cleanup();
            return false;
        }
    }

    if (notifCustEnabled) {
        receiver = new NotificationReceiverImpl();
        if (prodService->initReceive(bus, receiver) != ER_OK) {
            std::cout << "Could not initialize receiver." << std::endl;
            Cleanup();
            return false;
        }
    }
    return true;
}

int32_t NotificationTesterImpl::LoopHandler() {
    int32_t sleepTime = 5;

    if (notifProdEnabled) {
        uint64_t currentTimestamp = qcc::GetTimestamp64();
        if ((nextMessageTime == 0) || (currentTimestamp > nextMessageTime)) {
            if (nextMessageTime == 0) {
                if (notifIntervalOffset > 0) {
                    // add the initial offset to the notification time
                    intervalOffset = notifIntervalOffset;
                    QCC_DbgHLPrintf(("First notification will be offset by: %dms", intervalOffset));
                }
            } else {
                // send a message
                NotificationMessageType messageType = NotificationMessageType(INFO);
                std::vector<NotificationText> vecMessages;

                // setup for the next interval
                if (notifRatePriorityType == PRIORITY_RANDOM) {
                    // random priority type
                    uint32_t randomNum = qcc::Rand32();
                    messageType = NotificationMessageType(
                        (randomNum % NotificationMessageType::MESSAGE_TYPE_CNT));
                } else {
                    messageType = NotificationMessageType(notifRatePriority);
                }

                NotificationText textToSend(notifMsgLang, notifMsgText);
                vecMessages.push_back(textToSend);

                Notification notification(messageType, vecMessages);
                QStatus status = sender->send(notification, notifTTL);
                if (status != ER_OK) {
                    QCC_LogError(status, ("Could not send the message successfully."));
                } else {
                    QCC_DbgHLPrintf(("Sent Message with Type: %d; TTL: %d sec", messageType, notifTTL));
                }
            }

            // determine next message time
            int intervalInMsecs = notifInterval;
            if (intervalInMsecs <= 0) {
                std::cout << "Invalid Notification Interval, using 10 seconds instead" << std::endl;
                intervalInMsecs = 10000;
            }
            if (notifRateType == RATE_RANDOM) {
                // random rate type
                uint32_t randomNum = qcc::Rand32();
                uint64_t nextMessageInterval = (randomNum % intervalInMsecs);
                nextMessageTime = currentTimestamp + (nextMessageInterval) + intervalOffset;
                intervalOffset = intervalInMsecs - nextMessageInterval;
            } else {
                nextMessageTime = currentTimestamp + intervalInMsecs + intervalOffset;
                intervalOffset = 0;
            }

            sleepTime = ((nextMessageTime - currentTimestamp) + 999) / 1000;
            QCC_DbgHLPrintf(("Next message will be sent in %d sec", sleepTime));
        }
    }
    return sleepTime;
}
