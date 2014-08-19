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

#ifndef AJNSNOTIFICATIONRECEIVER_H_
#define AJNSNOTIFICATIONRECEIVER_H_
#import <Foundation/Foundation.h>
#import "alljoyn/notification/NotificationReceiver.h"
#import "AJNSNotification.h"

/**
 AJNSNotificationReceiver class
 */
@protocol AJNSNotificationReceiver <NSObject>

/**
 * Pure abstract function that receives a notification
 * Consumer Application must override this method
 * @param notification the notification that is received
 */

- (void)receive:(AJNSNotification *)notification;

/**
 *   Dismiss handler
 * @param msgId message ID to dismiss
 * @param appId app ID to use
 */
- (void)dismissMsgId:(const int32_t)msgId appId:(NSString*) appId;

@end
#endif
