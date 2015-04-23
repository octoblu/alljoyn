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

/**
 * This class is experimental, and as such has not been tested.
 * Please help make it more robust by contributing fixes if you find issues
 **/

#import "SampleTimer.h"

@interface SampleTimer ()
@property (strong, nonatomic) AJTMTimeServicePeriod* timerInterval;
@property (strong, nonatomic) AJTMTimeServicePeriod* timerLeft;
@property (strong, nonatomic) NSString* timerTitle;
@property (nonatomic) bool isRunning;
@property (nonatomic) uint16_t timerRepeat;
@end

@implementation SampleTimer

-(id)init
{
    self = [super init];
    if (self) {
        _timerInterval = [[AJTMTimeServicePeriod alloc]init];
        _timerLeft = [[AJTMTimeServicePeriod alloc]init];
        _timerTitle = @"";
    }
    return self;
}

-(AJTMTimeServicePeriod*)interval
{
    return _timerInterval;
}

-(QStatus)setInterval:(AJTMTimeServicePeriod*) interval
{
    if (!interval) {
        NSLog(@"AJTMTimeServicePeriod object is nil");
        return ER_FAIL;
    }
    _timerInterval = interval;
    return ER_OK;
}

-(AJTMTimeServicePeriod*)timeLeft
{
    return _timerLeft;
}

-(NSString*)title
{
    return _timerTitle;
}

-(void)setTitle:(NSString*) title
{
    _timerTitle = title;
}

-(bool)isRunning
{
    return _isRunning;
}

-(uint16_t)repeat
{
    return _timerRepeat;
}

-(void)setRepeat:(uint16_t) repeat
{
    _timerRepeat = repeat;
}

-(void)start
{
    NSLog(@"%@", NSStringFromSelector(_cmd));
}

-(void)reset
{
    NSLog(@"%@", NSStringFromSelector(_cmd));
}

-(void)pause
{
    NSLog(@"%@", NSStringFromSelector(_cmd));
}

-(void)releaseObject
{
    NSLog(@"%@", NSStringFromSelector(_cmd));
}

@end

