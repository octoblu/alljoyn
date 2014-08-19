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

#import "AJNSNotificationText.h"
#import "alljoyn/about/AJNConvertUtil.h"

@interface AJNSNotificationText ()
/** cpp notificationText Handler*/
@property (nonatomic) ajn::services::NotificationText *handle;
@end

@implementation AJNSNotificationText

- (AJNSNotificationText *)initWithLang:(NSString *)language andText:(NSString *)text
{
	self = [super init];
	if (self) {
		self.handle = new ajn::services::NotificationText([AJNConvertUtil convertNSStringToQCCString:language], [AJNConvertUtil convertNSStringToQCCString:text]);
	}
	return self;
}

/**
 * Set Language for Notification
 * @param language
 */
- (void)setLanguage:(NSString *)language
{
	self.handle->setLanguage([AJNConvertUtil convertNSStringToQCCString:language]);
}

/**
 * Get Language for Notification
 * @return language
 */
- (NSString *)getLanguage
{
	return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getLanguage()];
}

/**
 * Set Text for Notification
 * @param text the text to set
 */
- (void)setText:(NSString *)text
{
	self.handle->setText([AJNConvertUtil convertNSStringToQCCString:text]);
}

/**
 * Get Text for Notification
 * @return text
 */
- (NSString *)getText
{
	return [AJNConvertUtil convertQCCStringtoNSString:(self.handle->getText())];
}

@end
