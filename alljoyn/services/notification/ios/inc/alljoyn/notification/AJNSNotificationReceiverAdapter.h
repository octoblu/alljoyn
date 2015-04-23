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

#import <vector>
#import "alljoyn/notification/NotificationReceiver.h"
#import "alljoyn/notification/Notification.h"
#import "AJNSNotificationReceiver.h"

/**
 NotificationReceiverAdapter class
 */

class AJNSNotificationReceiverAdapter : public ajn::services::NotificationReceiver {
public:
	/**
     NotificationReceiverAdapter
     @param notificationReceiverHandler a AJNSNotificationReceiver handler
	 */
	AJNSNotificationReceiverAdapter(id <AJNSNotificationReceiver> notificationReceiverHandler);
    
	/**
     receive a notification
     @param notification the notification will be populated inside this param
	 */
	void Receive(ajn::services::Notification const& notification);
    
    /**
     Dismiss handler
     @param msgId message ID to dismiss
     @param appId app ID to use
     */
    void Dismiss(const int32_t msgId, const qcc::String appId);

    
	/**
     The handler of the receiver
	 */
	id <AJNSNotificationReceiver> ajnsNotificationReceiverHandler;
};
