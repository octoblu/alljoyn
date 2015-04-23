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

#import "AJCFGConfigService.h"
#import "AJNSessionOptions.h"
#import "alljoyn/config/ConfigService.h"
#import "alljoyn/services_common/AJSVCGenericLoggerAdapter.h"
#import "alljoyn/services_common/AJSVCGenericLoggerDefaultImpl.h"

@interface AJCFGConfigService ()

@property ajn::services::ConfigService *handle;
@property id <AJSVCGenericLogger> currentLogger;
@property AJSVCGenericLoggerAdapter *AJSVCGenericLoggerAdapter;

@end

@implementation AJCFGConfigService

- (id)initWithBus:(AJNBusAttachment *)bus propertyStore:(AJCFGPropertyStoreImpl *)propertyStore listener:(AJCFGConfigServiceListenerImpl *)listener
{
	self = [super init];
	if (self) {
		self.handle = new ajn::services::ConfigService((ajn::BusAttachment&)(*bus.handle), *propertyStore.getHandle, *[listener handle]);
		// Set a default logger
		self.currentLogger = [[AJSVCGenericLoggerDefaultImpl alloc] init];
		// Create and set a generic logger adapter adapter
		self.AJSVCGenericLoggerAdapter = new AJSVCGenericLoggerAdapter(self.currentLogger);
	}
	return self;
}

- (QStatus)registerService
{
	return self.handle->Register();
}

- (void)unregisterService
{
	//self.handle->Unregister();
}

#pragma mark - Logger methods
- (void)setLogger:(id <AJSVCGenericLogger> )logger
{
	if (logger) {
		// Save the current logger
		self.currentLogger = logger;
		// Call setLoger with the adapter and save the prev Logger
	}
	else {
		[self.currentLogger warnTag:([NSString stringWithFormat:@"%@", [[self class] description]]) text:@"Failed set a logger"];
	}
}

- (id <AJSVCGenericLogger> )logger
{
	return self.currentLogger;
}

- (void)setLogLevel:(QLogLevel)newLogLevel
{
	[self.currentLogger setLogLevel:newLogLevel];
}

- (QLogLevel)logLevel
{
	return [self.currentLogger logLevel];
}

@end
