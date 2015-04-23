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

#import "AJTMTimeServiceServer.h"
#import "alljoyn/time/TimeServiceServer.h"
#import "alljoyn/BusAttachment.h"
#import "AJNTranslatorImpl.h"
#import "AJTMTimeServiceServerAlarmAdapter.h"
#import "AJTMTimeServiceAlarmBusObjAdapter.h"
#import "AJTMTimeServiceTimerBusObjAdapter.h"
#import "AJTMTimeServiceServerTimeAuthorityAdapter.h"
#import "AJTMTimeServiceServerTimerFactoryAdapter.h"
#import "AJTMTimeServiceServerAlarmFactoryAdapter.h"

@interface AJTMTimeServiceServer ()

@end

@implementation AJTMTimeServiceServer

+ (id)sharedInstance
{
	static AJTMTimeServiceServer* timeServiceServer;
	static dispatch_once_t donce;
	dispatch_once(&donce, ^{
	    timeServiceServer = [[self alloc] init];
	});
	return timeServiceServer;
}

-(QStatus)initWithBus:(AJNBusAttachment*) busAttachment
{
    return ajn::services::TimeServiceServer::getInstance()->init((ajn::BusAttachment*)busAttachment.handle);
}

-(void)shutdown
{
    ajn::services::TimeServiceServer::getInstance()->shutdown();
}

-(AJNBusAttachment*)busAttachment
{
    AJNBusAttachment* bus = [[AJNBusAttachment alloc] initWithHandle:(ajn::BusAttachment*)(ajn::services::TimeServiceServer::getInstance()->getBusAttachment())];
    return bus;
}

-(bool)isStarted
{
    return ajn::services::TimeServiceServer::getInstance()->isStarted();
}

-(QStatus)createClock:(AJTMTimeServiceServerClockBase*) clock
{
    if ([clock isKindOfClass:[AJTMTimeServiceServerClockBase class]] && [clock conformsToProtocol:@protocol(AJTMTimeServiceServerClock)])
    {
        AJTMTimeServiceServerClockAdapter* clockAdapter;
        //create an adapter using the clock handle to receive callbacks
        clockAdapter = new AJTMTimeServiceServerClockAdapter((id<AJTMTimeServiceServerClock>)clock);

        [clock registerAdapter:clockAdapter];

        return ajn::services::TimeServiceServer::getInstance()->activateClock(clockAdapter);
    } else {
        NSLog(@"Clock object type is wrong");
        return ER_FAIL;
    }
}

-(QStatus)createTimeAuthorityClock:(AJTMTimeServiceServerAuthorityClock*) clock authorityType:(AJTMClockAuthorityType) authorityType
{
    if ([clock isKindOfClass:[AJTMTimeServiceServerTimeAuthorityBase class]] && [clock conformsToProtocol:@protocol(AJTMTimeServiceServerClock) ])
    {
        AJTMTimeServiceServerTimeAuthorityAdapter* timeAuthorityAdapter;
        //create an adapter using the clock handle to receive callbacks
        timeAuthorityAdapter = new AJTMTimeServiceServerTimeAuthorityAdapter((id<AJTMTimeServiceServerClock>)clock);

        [clock registerAdapter:timeAuthorityAdapter];

        return ajn::services::TimeServiceServer::getInstance()->activateTimeAuthorityClock(timeAuthorityAdapter, (ajn::services::tsConsts::ClockAuthorityType)authorityType);
    } else {
        NSLog(@"Time Authority object type is wrong");
        return ER_FAIL;
    }

    return ER_OK;
}

-(QStatus)createAlarm:(AJTMTimeServiceServerAlarmBase*) alarm
{
    if ([alarm isKindOfClass:[AJTMTimeServiceServerAlarmBase class]] && [alarm conformsToProtocol:@protocol(AJTMTimeServiceServerAlarm) ])
    {
        AJTMTimeServiceServerAlarmAdapter* alarmAdapter;
        //create an adapter using the clock handle to receive callbacks
        alarmAdapter = new AJTMTimeServiceServerAlarmAdapter((id<AJTMTimeServiceServerAlarm>)alarm);

        [alarm registerAdapter:alarmAdapter];

        return ajn::services::TimeServiceServer::getInstance()->activateAlarm(alarmAdapter);
    } else {
        NSLog(@"Alarm object type is wrong");
        return ER_FAIL;
    }
}

-(QStatus)createAlarm:(AJTMTimeServiceServerAlarmBase*) alarm description:(NSString*) description language:(NSString*) language translator:(id<AJNTranslator>) translator
{
    //create an adapter using the alarm handle
    [self createAlarm:alarm];

    return ajn::services::TimeServiceServer::getInstance()->activateAlarm((__bridge AJTMTimeServiceServerAlarmAdapter*)[alarm getHandle], (qcc::String)[description UTF8String], (qcc::String)[language UTF8String], new AJNTranslatorImpl(translator));
}

-(QStatus)registerCustomAlarm:(id<AJTMTimeServiceAlarmBusObj>) alarmBusObj alarm:(AJTMTimeServiceServerAlarmBase*) alarm notAnnounced:(NSArray*) notAnnounced
{
    [self createAlarm:alarm];
    AJTMTimeServiceAlarmBusObjAdapter* alarmBusObjAdapter;

    alarmBusObjAdapter = new AJTMTimeServiceAlarmBusObjAdapter(alarmBusObj);

    std::vector <qcc::String> notAnnouncedVect;
    for (NSString *str in notAnnounced) {
        notAnnouncedVect.push_back([str cStringUsingEncoding:NSASCIIStringEncoding]);
    }

    return ajn::services::TimeServiceServer::getInstance()->registerCustomAlarm(alarmBusObjAdapter, (__bridge AJTMTimeServiceServerAlarmAdapter*)[alarm getHandle], notAnnouncedVect);
}

-(QStatus)registerCustomAlarm:(id<AJTMTimeServiceAlarmBusObj>) alarmBusObj alarm:(AJTMTimeServiceServerAlarmBase*) alarm notAnnounced:(NSArray*) notAnnounced description:(NSString*) description language:(NSString*) language translator:(id<AJNTranslator>) translator
{

    [self createAlarm:alarm];
    AJTMTimeServiceAlarmBusObjAdapter* alarmBusObjAdapter;

    alarmBusObjAdapter = new AJTMTimeServiceAlarmBusObjAdapter(alarmBusObj);

    std::vector <qcc::String> notAnnouncedVect;
    for (NSString *str in notAnnounced) {
        notAnnouncedVect.push_back([str cStringUsingEncoding:NSASCIIStringEncoding]);
    }

    return ajn::services::TimeServiceServer::getInstance()->registerCustomAlarm(alarmBusObjAdapter, (__bridge AJTMTimeServiceServerAlarmAdapter*)[alarm getHandle], notAnnouncedVect, (qcc::String)[description UTF8String], (qcc::String)[language UTF8String], new AJNTranslatorImpl(translator) );
}

-(QStatus)createTimer:(AJTMTimeServiceServerTimerBase*) timer
{
    if ([timer isKindOfClass:[AJTMTimeServiceServerTimerBase class]] && [timer conformsToProtocol:@protocol(AJTMTimeServiceServerTimer) ])
    {
        AJTMTimeServiceServerTimerAdapter* timerAdapter;
        //create an adapter using the clock handle to receive callbacks
        timerAdapter = new AJTMTimeServiceServerTimerAdapter((id<AJTMTimeServiceServerTimer>)timer);

        [timer registerAdapter:timerAdapter];

        return ajn::services::TimeServiceServer::getInstance()->activateTimer(timerAdapter);
    } else {
        NSLog(@"Timer object type is wrong");
        return ER_FAIL;
    }
}

-(QStatus)createTimer:(AJTMTimeServiceServerTimerBase*) timer description:(NSString*) description language:(NSString*) language translator:(id<AJNTranslator>) translator
{
    //create an adapter using the timer handle
    [self createTimer:timer];

    return ajn::services::TimeServiceServer::getInstance()->activateTimer((__bridge AJTMTimeServiceServerTimerAdapter*)[timer getHandle], (qcc::String)[description UTF8String], (qcc::String)[language UTF8String], new AJNTranslatorImpl(translator));
}

-(QStatus)registerCustomTimer:(id<AJTMTimeServiceTimerBusObj>) timerBusObj timer:(AJTMTimeServiceServerTimerBase*) timer notAnnounced:(NSArray*) notAnnounced
{
    [self createTimer:timer];
    AJTMTimeServiceTimerBusObjAdapter* timerBusObjAdapter;

    timerBusObjAdapter = new AJTMTimeServiceTimerBusObjAdapter(timerBusObj);

    std::vector <qcc::String> notAnnouncedVect;
    for (NSString *str in notAnnounced) {
        notAnnouncedVect.push_back([str cStringUsingEncoding:NSASCIIStringEncoding]);
    }

    return ajn::services::TimeServiceServer::getInstance()->registerCustomTimer(timerBusObjAdapter, (__bridge AJTMTimeServiceServerTimerAdapter*)[timer getHandle], notAnnouncedVect);
}

-(QStatus)registerCustomTimer:(id<AJTMTimeServiceTimerBusObj>) timerBusObj timer:(AJTMTimeServiceServerTimerBase*) timer notAnnounced:(NSArray*) notAnnounced description:(NSString*) description language:(NSString*) language translator:(id<AJNTranslator>) translator
{

    [self createTimer:timer];
    AJTMTimeServiceTimerBusObjAdapter* timerBusObjAdapter;

    timerBusObjAdapter = new AJTMTimeServiceTimerBusObjAdapter(timerBusObj);

    std::vector <qcc::String> notAnnouncedVect;
    for (NSString *str in notAnnounced) {
        notAnnouncedVect.push_back([str cStringUsingEncoding:NSASCIIStringEncoding]);
    }

    return ajn::services::TimeServiceServer::getInstance()->registerCustomTimer(timerBusObjAdapter, (__bridge AJTMTimeServiceServerTimerAdapter*)[timer getHandle], notAnnouncedVect, (qcc::String)[description UTF8String], (qcc::String)[language UTF8String], new AJNTranslatorImpl(translator) );
}



-(QStatus) createAlarmFactory:(AJTMTimeServiceServerAlarmFactoryBase*) alarmFactory
{
    if ([alarmFactory isKindOfClass:[AJTMTimeServiceServerAlarmFactoryBase class]] && [alarmFactory conformsToProtocol:@protocol(AJTMTimeServiceServerAlarmFactory) ])
    {

        //create an adapter using the clock handle to receive callbacks
        AJTMTimeServiceServerAlarmFactoryAdapter *alarmFactoryAdapter = new AJTMTimeServiceServerAlarmFactoryAdapter((id<AJTMTimeServiceServerAlarmFactory>)alarmFactory);

        [alarmFactory registerAdapter:alarmFactoryAdapter];

        return ajn::services::TimeServiceServer::getInstance()->activateAlarmFactory(alarmFactoryAdapter);
    } else {
        NSLog(@"Alarm Factory object type is wrong");
        return ER_FAIL;
    }

}

-(QStatus) createAlarmFactory:(AJTMTimeServiceServerAlarmFactoryBase*) alarmFactory description:(NSString*) description language:(NSString*) language translator:(id<AJNTranslator>) translator
{
    //create an adapter using the alarm handle
    [self createAlarmFactory:alarmFactory];

    return ajn::services::TimeServiceServer::getInstance()->activateAlarmFactory((__bridge AJTMTimeServiceServerAlarmFactoryAdapter*)[alarmFactory getHandle], (qcc::String)[description UTF8String], (qcc::String)[language UTF8String], new AJNTranslatorImpl(translator));
}

-(QStatus) createTimerFactory:(AJTMTimeServiceServerTimerFactoryBase*) timerFactory
{
    if ([timerFactory isKindOfClass:[AJTMTimeServiceServerTimerFactoryBase class]] && [timerFactory conformsToProtocol:@protocol(AJTMTimeServiceServerTimerFactory) ])
    {

        //create an adapter using the clock handle to receive callbacks
        AJTMTimeServiceServerTimerFactoryAdapter *timerFactoryAdapter = new AJTMTimeServiceServerTimerFactoryAdapter((id<AJTMTimeServiceServerTimerFactory>)timerFactory);

        [timerFactory registerAdapter:timerFactoryAdapter];

        return ajn::services::TimeServiceServer::getInstance()->activateTimerFactory(timerFactoryAdapter);
    } else {
        NSLog(@"Timer Factory object type is wrong");
        return ER_FAIL;
    }

}

-(QStatus) createTimerFactory:(AJTMTimeServiceServerTimerFactoryBase*) timerFactory description:(NSString*) description language:(NSString*) language translator:(id<AJNTranslator>) translator
{
    //create an adapter using the timer handle
    [self createTimerFactory:timerFactory];

    return ajn::services::TimeServiceServer::getInstance()->activateTimerFactory((__bridge AJTMTimeServiceServerTimerFactoryAdapter*)[timerFactory getHandle], (qcc::String)[description UTF8String], (qcc::String)[language UTF8String], new AJNTranslatorImpl(translator));
}

-(QStatus)generateObjectPath:(NSString*) objectPath
{
     return ajn::services::TimeServiceServer::getInstance()->generateObjectPath((qcc::String*)[objectPath UTF8String]);
}

@end
