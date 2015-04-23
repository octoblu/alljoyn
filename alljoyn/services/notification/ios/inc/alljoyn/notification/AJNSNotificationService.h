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
#import "alljoyn/notification/NotificationService.h"
#import "AJNSNotificationReceiverAdapter.h"
#import "AJNSNotificationReceiver.h"
#import "AJNSNotificationSender.h"
#import "AJNBusAttachment.h"
#import "alljoyn/services_common/AJSVCGenericLoggerAdapter.h"

/**
 AJNSNotificationService class
 */
@interface AJNSNotificationService : NSObject

///---------------------
/// @name Properties
///---------------------

/**
 * Get Instance of AJNSNotificationServiceImpl
 * @return instance
 */
+ (id)sharedInstance;

/**
 *  Initialize Producer side via Transport. Create and
 *  return NotificationSender.
 *  @param bus ajn bus
 *  @param store property store
 *  @return NotificationSender instance
 */

- (AJNSNotificationSender *)startSendWithBus:(AJNBusAttachment *)bus andPropertyStore:(AJNAboutPropertyStoreImpl *)store;
/**
 * Initialize Consumer side via Transport.
 * Set NotificationReceiver to given receiver
 * @param bus ajn bus
 * @param ajnsNotificationReceiver notification receiver
 * @return status
 */
- (QStatus)startReceive:(AJNBusAttachment *)bus withReceiver:(id <AJNSNotificationReceiver> )ajnsNotificationReceiver;

/**
 * Stops sender but leaves bus and other objects alive
 */
- (void)shutdownSender;

/**
 * Stops receiving but leaves bus and other objects alive
 */
- (void)shutdownReceiver;

/**
 * Cleanup and get ready for shutdown
 */
- (void)shutdown;

/**
 * Disabling superagent mode. Needs to be called before
 * starting receiver
 * @return status
 */
- (QStatus)disableSuperAgent;

/**
 * Get the currently-configured logger implementation
 * @return logger Implementation of GenericLogger
 */
- (id <AJSVCGenericLogger> )logger;



/**
 * Set log level filter for subsequent logging messages
 * @param newLogLevel enum value
 * @return logLevel enum value that was in effect prior to this change
 */
- (void)setLogLevel:(QLogLevel)newLogLevel;

/**
 * Get log level filter value currently in effect
 * @return logLevel enum value currently in effect
 */
- (QLogLevel)logLevel;

/**
 * Virtual method to get the busAttachment used in the service.
 */
- (AJNBusAttachment *)busAttachment;

/**
 * Get the Version of the NotificationService
 * @return the NotificationService version
 */
- (uint16_t)version;

@end
