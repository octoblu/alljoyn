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

/**
 AJSVCGenericLogger protocol allows a custom logging using:
 </br> - QLogLevel enum which represents the available log levels.
 </br> - A set of methods one per logLevel.
 </br> The methods receive a 'tag' String to represent the current context and a log text String.
 </br> - Log level setter and getter.
 */
@protocol AJSVCGenericLogger <NSObject>

/**
 @enum QLogLevel
 */
typedef NS_ENUM (NSInteger, QLogLevel) {
	/** Fatal Error */
	QLEVEL_FATAL = 1,
	/** Error */
	QLEVEL_ERROR = 2,
	/** Warning */
	QLEVEL_WARN  = 3,
	/** Information */
	QLEVEL_INFO  = 4,
	/** Debug */
	QLEVEL_DEBUG = 5,
};

/**
 Log in Debug Level.
 @param tag identifies area of code.
 @param logText Text to be logged.
 */
- (void)debugTag:(NSString *)tag text:(NSString *)logText;

/**
 Log in Info Level.
 @param tag identifies area of code.
 @param logText Text to be logged.
 */
- (void)infoTag:(NSString *)tag text:(NSString *)logText;

/**
 Log in Warn Level.
 @param tag identifies area of code.
 @param logText Text to be logged.
 */
- (void)warnTag:(NSString *)tag text:(NSString *)logText;

/**
 Log in Error Level.
 @param tag identifies area of code.
 @param logText Text to be logged.
 */
- (void)errorTag:(NSString *)tag text:(NSString *)logText;

/**
 Log in Fatal Level.
 @param tag identifies area of code.
 @param logText Text to be logged.
 */
- (void)fatalTag:(NSString *)tag text:(NSString *)logText;

/**
 Set log level filter value currently in effect.
 @param logLevel log level (QLogLevel).
 */
- (void)setLogLevel:(QLogLevel)logLevel;

/**
 Get log level filter value currently in effect.
 */
- (QLogLevel)logLevel;

@end
