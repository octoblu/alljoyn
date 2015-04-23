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

#import <Foundation/Foundation.h>
#import "alljoyn/notification/NotificationText.h"

/**
 AJNSNotificationText class.
 */
@interface AJNSNotificationText : NSObject

///---------------------
/// @name Properties
///---------------------

/**
 Initialize notification text with language and text
 @param language The language of this notification text
 @param text The text of the notification text
 @return pointer to the noficiation text object
 */
- (AJNSNotificationText *)initWithLang:(NSString *)language andText:(NSString *)text;

/**
 * Set Language for Notification
 * @param language set the language of the notification text
 */
- (void)setLanguage:(NSString *)language;

/**
 * Get Language for Notification
 * @return language of this notification text
 */
- (NSString *)getLanguage;

/**
 * Set Text for Notification
 * @param text set the text for this notification text
 */
- (void)setText:(NSString *)text;

/**
 * Get Text for Notification
 * @return text
 */
- (NSString *)getText;

@property (nonatomic, readonly)ajn::services::NotificationText * handle;

@end
