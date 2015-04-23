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

#import "CommonBusListener.h"
#import "AJNSessionOptions.h"
#import "alljoyn/config/AJCFGConfigLogger.h"

@interface CommonBusListener ()

@property AJNSessionPort servicePort;

@end

@implementation CommonBusListener

- (id)initWithServicePort:(AJNSessionPort)servicePort
{
	self = [super init];
	if (self) {
		self.servicePort = servicePort;
	}
	return self;
}

- (void)setSessionPort:(AJNSessionPort)sessionPort
{
	self.servicePort = sessionPort;
}

- (AJNSessionPort)sessionPort
{
	return self.servicePort;
}

#pragma mark - AJNSessionPortListener protocol method
- (BOOL)shouldAcceptSessionJoinerNamed:(NSString *)joiner onSessionPort:(AJNSessionPort)sessionPort withSessionOptions:(AJNSessionOptions *)options
{
	if (sessionPort != self.servicePort) {
		[[[AJCFGConfigLogger sharedInstance] logger] errorTag:[[self class] description] text:[NSString stringWithFormat:@"Rejecting join attempt on unexpected session port %hu.", sessionPort]];
		return false;
	}
	else {
		[[[AJCFGConfigLogger sharedInstance] logger] debugTag:[[self class] description] text:[NSString stringWithFormat:@"Accepting join session request from %@ (proximity=%c, traffic=%u, transports=%hu).\n", joiner, options.proximity, options.trafficType, options.transports]];
		return true;
	}
}

@end
