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

#import "AJCPSControlPanelService.h"
#import "AJSVCGenericLoggerAdapter.h"
#import "AJCPSControlPanelListenerAdapter.h"

@interface AJCPSControlPanelService ()
@property (nonatomic) ajn::services::ControlPanelService *handle;
@property (nonatomic) AJCPSControlPanelListenerAdapter *listenerAdapter;

@property id <AJSVCGenericLogger> currentLogger;
@property AJSVCGenericLoggerAdapter *genericLoggerAdapter;
@end


@implementation AJCPSControlPanelService

+ (AJCPSControlPanelService *)getInstance
{
	static AJCPSControlPanelService *aboutLogger;
	static dispatch_once_t donce;
	dispatch_once(&donce, ^{
	    aboutLogger = [[self alloc] init];
	});
	return aboutLogger;
}

- (id)init
{
	self = [super init];
	self.handle = ajn::services::ControlPanelService::getInstance();
	return self;
}

- (QStatus)initController:(AJNBusAttachment *)bus controlPanelController:(AJCPSControlPanelController *)controlPanelController
     controlPanelListener:(id <AJCPSControlPanelListener> )controlPanelListener
{
	self.listenerAdapter = new AJCPSControlPanelListenerAdapter(controlPanelListener);
    
	return self.handle->initController((ajn::BusAttachment *)[bus handle], [controlPanelController handle], self.listenerAdapter);
}

/**
 * Remove locally stored controller. Allows a new call to initController to be made
 * @return status
 */
- (QStatus)shutdownController
{
	return self.handle->shutdown();
}

- (AJNBusAttachment *)getBusAttachment
{
	return [[AJNBusAttachment alloc]initWithHandle:self.handle->getBusAttachment()];
}

/**
 * Get the ControlPanelListener
 * @return ControlPanelListener
 */
- (id <AJCPSControlPanelListener> )getControlPanelListener
{
	AJCPSControlPanelListenerAdapter *adapter = (AJCPSControlPanelListenerAdapter *)self.handle->getControlPanelListener();
    
	return adapter->getListener();
}

/**
 * Get the Version of the ControlPanelService
 * @return the ControlPanelService version
 */
- (uint16_t)getVersion
{
	return self.handle->getVersion();
}

#pragma mark - Logger methods
- (void)setLogger:(id <AJSVCGenericLogger> )logger
{
	if (logger) {
		// save current logger
		self.currentLogger = logger;
		// call setLoger with the adapter and save the prev Logger
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