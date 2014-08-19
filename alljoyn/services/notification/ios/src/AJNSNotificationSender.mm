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

#import "AJNSNotificationSender.h"

@interface AJNSNotificationSender ()
@property (nonatomic) ajn::services::AboutPropertyStoreImpl *propertyStore;

@end

@implementation AJNSNotificationSender

- (AJNSNotificationSender *)initWithPropertyStore:(AJNAboutPropertyStoreImpl *)propertyStore
{
	self = [super init];
	if (self) {
		self.propertyStore = [propertyStore getHandle];
		if (self.propertyStore) {
			self.senderHandle = new ajn::services::NotificationSender(self.propertyStore);
			return self;
		}
	}
	return nil;
}

/**
 * Send notification
 * @param notification
 * @param ttl message ttl
 * @return status
 */
- (QStatus)send:(AJNSNotification *)ajnsNotification ttl:(uint16_t)ttl
{
	return (self.senderHandle->send(*ajnsNotification.handle, ttl));
}

/**
 * Delete last message that was sent with given MessageType
 * @param messageType MessageType of message to be deleted
 * @return status
 */
- (QStatus)deleteLastMsg:(AJNSNotificationMessageType)messageType
{
	return(self.senderHandle->deleteLastMsg((ajn::services::NotificationMessageType)messageType));
}

- (ajn::services::AboutPropertyStoreImpl *)getPropertyStore
{
	return self.propertyStore;
}

@end
