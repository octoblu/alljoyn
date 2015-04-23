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

#ifndef AJNSNOTIFICATIONENUMS_H_
#define AJNSNOTIFICATIONENUMS_H_

/**
 AJNSNotificationEnums class
 */
@interface AJNSNotificationEnums : NSObject

/**
 @enum AJNSNotificationMessageType enum
 @abstract AJNSNotificationMessageType DBus request status return values.
 */
typedef NS_ENUM (NSInteger, AJNSNotificationMessageType) {
	/** EMERGENCY - Urgent Message */
	EMERGENCY = 0,
	/** WARNING  - Warning Message */
	WARNING = 1,
	/** INFO - Informational Message */
	INFO = 2,
	/** MESSAGE_TYPE_CNT  - Number of Message Types Defined */
	MESSAGE_TYPE_CNT = 3,
	UNSET = 4
};

/**
 Convet AJNSNotificationMessageType to an NSString format
 @param msgType DBus request status
 @return message type in an NSString format(Capital letters)
 */
+ (NSString *)AJNSMessageTypeToString:(AJNSNotificationMessageType)msgType;

@end
#endif
