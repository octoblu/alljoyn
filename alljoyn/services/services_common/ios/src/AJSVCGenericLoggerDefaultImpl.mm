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

#import "AJSVCGenericLoggerDefaultImpl.h"
#import "AJSVCGenericLoggerAdapter.h"
#import "AJSVCGenericLoggerUtil.h"

@interface AJSVCGenericLoggerDefaultImpl ()
@property QLogLevel currentlogLevel;
@end

@implementation AJSVCGenericLoggerDefaultImpl

- (id)init
{
	self = [super init];
	if (self) {
		self.currentlogLevel = QLEVEL_DEBUG;
		[self printTag:[NSString stringWithFormat:@"%@", [[self class] description]]
                  text:@"Logger Started" logLevel:self.currentlogLevel];
        
  		[self printTag:[NSString stringWithFormat:@"%@", [[self class] description]]
                  text:[NSString stringWithFormat:@"App Version:%@", [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleVersion"]]
                                        logLevel:self.currentlogLevel];
	}
	return self;
}

- (void)debugTag:(NSString *)tag text:(NSString *)text
{
	[self printTag:tag text:text logLevel:QLEVEL_DEBUG];
}

- (void)infoTag:(NSString *)tag text:(NSString *)text
{
	[self printTag:tag text:text logLevel:QLEVEL_INFO];
}

- (void)warnTag:(NSString *)tag text:(NSString *)text
{
	[self printTag:tag text:text logLevel:QLEVEL_WARN];
}

- (void)errorTag:(NSString *)tag text:(NSString *)text
{
	[self printTag:tag text:text logLevel:QLEVEL_ERROR];
}

- (void)fatalTag:(NSString *)tag text:(NSString *)text
{
	[self printTag:tag text:text logLevel:QLEVEL_FATAL];
}

- (void)printTag:(NSString *)tag text:(NSString *)logText logLevel:(QLogLevel)functionLogLevel
{
	// Get the logger level and print logs accordingly
	QLogLevel loggerLogLevel = [self logLevel];
    
	if (functionLogLevel <= loggerLogLevel) {
		NSLog(@"[%@][%@] %@", [AJSVCGenericLoggerUtil toStringQLogLevel:functionLogLevel], tag, logText);
	}
}

- (void)setLogLevel:(QLogLevel)logLevel
{
	[self printTag:[NSString stringWithFormat:@"%@", [[self class] description]] text:[NSString stringWithFormat:@"New Log level is %@.", [AJSVCGenericLoggerUtil toStringQLogLevel:logLevel]] logLevel:self.currentlogLevel];
    
	self.currentlogLevel = logLevel;
}

- (QLogLevel)logLevel
{
	return self.currentlogLevel;
}

@end
