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

#ifndef NOTIFICATIONTESTERIMPL_H_
#define NOTIFICATIONTESTERIMPL_H_

#include <alljoyn/notification/NotificationService.h>
#include "NotificationReceiverImpl.h"
#include <AboutDataStore.h>

/**
 * Class for testing notification functionality based on runtime configuration.
 * First call Initialize and ten call LoopHandler periodically based on the number of seconds returned.
 */
class NotificationTesterImpl {
  public:

    /**
     * Constructor
     */
    NotificationTesterImpl();

    /**
     * Destructor
     */
    ~NotificationTesterImpl();

    /**
     * Initialize - initializes the testing functionality
     * @param bus
     * @param store
     * @return true if initialization was successful
     */
    bool Initialize(ajn::BusAttachment* bus, AboutDataStore* store);

    /**
     * LoopHandler - function that executes the test functionality
     * @return maximum amount of time in seconds to wait before calling LoopHandler again
     */
    int32_t LoopHandler();

  private:

    enum NotificationRateType {
        RATE_FIXED, RATE_RANDOM
    };
    enum NotificationPriorityType {
        PRIORITY_FIXED, PRIORITY_RANDOM
    };

    void LoadConfiguration();
    void Cleanup();

    uint64_t nextMessageTime;
    uint64_t intervalOffset;
    ajn::services::NotificationService* prodService;
    NotificationReceiverImpl* receiver;
    ajn::services::NotificationSender* sender;

    bool notifProdEnabled;
    uint32_t notifInterval;
    uint32_t notifIntervalOffset;
    NotificationRateType notifRateType;
    uint16_t notifRatePriority;
    NotificationPriorityType notifRatePriorityType;
    qcc::String notifMsgText;
    qcc::String notifMsgLang;
    uint32_t notifTTL;
    bool notifCustEnabled;
};

#endif /* NOTIFICATIONTESTERIMPL_H_ */
