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

#import "AJNSNotificationReceiverAdapter.h"
#import "AJNSNotificationReceiver.h"
#import "alljoyn/about/AJNConvertUtil.h"

AJNSNotificationReceiverAdapter::AJNSNotificationReceiverAdapter(id <AJNSNotificationReceiver> notificationReceiverHandler)
{
	ajnsNotificationReceiverHandler = notificationReceiverHandler;
}

void AJNSNotificationReceiverAdapter::Receive(ajn::services::Notification const& notification)
{
    AJNSNotification *t_ajnsNotification = [[AJNSNotification alloc] initWithHandle:(new ajn::services::Notification(notification))];

	[t_ajnsNotification createAJNSNotificationTextArray];
	[ajnsNotificationReceiverHandler receive:t_ajnsNotification];
}

void AJNSNotificationReceiverAdapter::Dismiss(const int32_t msgId, const qcc::String appId)
{
    NSLog(@"Got Dissmiss of msgId %d and appId %@",msgId, [AJNConvertUtil convertQCCStringtoNSString:appId]);
    [ajnsNotificationReceiverHandler dismissMsgId:msgId appId:[AJNConvertUtil convertQCCStringtoNSString:appId]];
}