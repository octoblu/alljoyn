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

#import "alljoyn/time/AJTMTimeServiceClient.h"
#import "AJNAboutObjectDescription.h"
#import "AJTMTimeServiceClientClock.h"
#import "AJTMTimeServiceClientAlarm.h"
#import "AJTMTimeServiceClientTimer.h"
#import "AJTMTimeServiceClientAlarmFactory.h"
#import "AJTMTimeServiceClientTimerFactory.h"
#import "AJTMTimeServiceSessionListenerAdapter.h"

@interface AJTMTimeServiceClient()

@property ajn::services::TimeServiceClient* handle;

@end

@implementation AJTMTimeServiceClient

- (id)init
{
    self = [super init];
    if (self) {
        self.handle = new ajn::services::TimeServiceClient();
    }
    return self;
}

- (id)initWithHandle:(ajn::services::TimeServiceClient*)handle
{
    self = [super init];
    if (self) {
        self.handle = (ajn::services::TimeServiceClient *)handle;
    }
    return self;
}

-(QStatus)populateWithBus:(AJNBusAttachment*) busAttachment serverBusName:(NSString *)serverBusName deviceId:(NSString *)deviceId appId:(NSString *)appId objDescArgs:(AJNMessageArgument *)objDescsArgs
{
	AJNAboutObjectDescription* nativeObjDescs = [[AJNAboutObjectDescription alloc] initWithMsgArg:objDescsArgs];

	QStatus status = self.handle->init((ajn::BusAttachment*)busAttachment.handle, [serverBusName UTF8String], [deviceId UTF8String], [appId UTF8String], *(ajn::AboutObjectDescription*)nativeObjDescs.handle);

    return status;
}

-(void) releaseObject
{
    self.handle->release();
}

-(AJNBusAttachment*) getBus
{
    return [[AJNBusAttachment alloc]initWithHandle:self.handle->getBus()];
}

-(NSString *)getServerBusName
{
    return [NSString stringWithUTF8String:self.handle->getServerBusName().c_str()];
}

-(NSString *)getDeviceId
{
    return [NSString stringWithUTF8String:self.handle->getDeviceId().c_str()];

}

-(NSString *)getAppId
{
    return [NSString stringWithUTF8String:self.handle->getAppId().c_str()];

}


//    const std::vector<TimeServiceClientClock*>&
-(NSArray *)getAnnouncedClockList
{
    std::vector<ajn::services::TimeServiceClientClock*> list = self.handle->getAnnouncedClockList();
    NSMutableArray *tmpList = [[NSMutableArray alloc]init];

    std::vector<ajn::services::TimeServiceClientClock*>::iterator itr;

    for (itr=list.begin(); itr!=list.end(); itr++) {
        [tmpList addObject:[[AJTMTimeServiceClientClock alloc]initWithHandle:*itr]];
    }

    return tmpList;
}

//const std::vector<TimeServiceClientAlarm*>&
-(NSArray *)getAnnouncedAlarmList
{
    std::vector<ajn::services::TimeServiceClientAlarm*> list = self.handle->getAnnouncedAlarmList();
    NSMutableArray *tmpList = [[NSMutableArray alloc]init];

    std::vector<ajn::services::TimeServiceClientAlarm*>::iterator itr;

    for (itr=list.begin(); itr!=list.end(); itr++) {
        [tmpList addObject:[[AJTMTimeServiceClientAlarm alloc]initWithHandle:*itr]];
    }

    return tmpList;
}

//    const std::vector<TimeServiceClientAlarmFactory*>&
-(NSArray *)getAnnouncedAlarmFactoryList
{
    std::vector<ajn::services::TimeServiceClientAlarmFactory*> list = self.handle->getAnnouncedAlarmFactoryList();
    NSMutableArray *tmpList = [[NSMutableArray alloc]init];

    std::vector<ajn::services::TimeServiceClientAlarmFactory*>::iterator itr;

    for (itr=list.begin(); itr!=list.end(); itr++) {
        [tmpList addObject:[[AJTMTimeServiceClientAlarmFactory alloc]initWithHandle:*itr]];
    }

    return tmpList;
}

//    const std::vector<TimeServiceClientTimer*>&
-(NSArray *)getAnnouncedTimerList
{
    std::vector<ajn::services::TimeServiceClientTimer*> list = self.handle->getAnnouncedTimerList();
    NSMutableArray *tmpList = [[NSMutableArray alloc]init];

    std::vector<ajn::services::TimeServiceClientTimer*>::iterator itr;

    for (itr=list.begin(); itr!=list.end(); itr++) {
        [tmpList addObject:[[AJTMTimeServiceClientTimer alloc]initWithHandle:*itr]];
    }

    return tmpList;
}

//  const std::vector<TimeServiceClientTimerFactory*>&
-(NSArray *)getAnnouncedTimerFactoryList
{
    std::vector<ajn::services::TimeServiceClientTimerFactory*> list = self.handle->getAnnouncedTimerFactoryList();
    NSMutableArray *tmpList = [[NSMutableArray alloc]init];

    std::vector<ajn::services::TimeServiceClientTimerFactory*>::iterator itr;

    for (itr=list.begin(); itr!=list.end(); itr++) {
        [tmpList addObject:[[AJTMTimeServiceClientTimerFactory alloc]initWithHandle:*itr]];
    }

    return tmpList;
}

-(QStatus) joinSessionAsyncWithListener:(id<AJTMTimeServiceSessionListener>) sessionListener
{
    AJTMTimeServiceSessionListenerAdapter *adapter = new AJTMTimeServiceSessionListenerAdapter(sessionListener);

    return self.handle->joinSessionAsync(adapter);
}

-(QStatus)  leaveSession
{
    return self.handle->leaveSession();
}

-(bool) isConnected
{
    return self.handle->isConnected();
}

-(QStatus)getSessionId:(AJNSessionId *)sessionId
{
    ajn::SessionId nativeSessionId;
    QStatus status = self.handle->getSessionId(nativeSessionId);

    if (status == ER_OK) {
        nativeSessionId = *sessionId;
    }

    return status;
}

-(ajn::services::TimeServiceClient*)getHandle
{
    return self.handle;
}

@end
