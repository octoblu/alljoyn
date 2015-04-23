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

#import <Foundation/Foundation.h>
#import "AJNBusAttachment.h"
#import "AJNTranslator.h"
#import "AJTMTimeServiceServerClock.h"
#import "AJTMTimeServiceServerAuthorityClock.h"
#import "AJTMTimeServiceServerAlarm.h"
#import "AJTMTimeServiceAlarmBusObj.h"
#import "AJTMTimeServiceTimerBusObj.h"
#import "AJTMTimeServiceConstants.h"
#import "alljoyn/time/TimeServiceConstants.h"
#import "AJTMTimeServiceServerClockBase.h"
#import "AJTMTimeServiceServerAlarmBase.h"
#import "AJTMTimeServiceServerTimerBase.h"
#import "AJTMTimeServiceServerAlarmFactoryBase.h"
#import "AJTMTimeServiceServerTimerFactoryBase.h"


//class TimeServiceServer : public SessionListener, public SessionPortListener {
/**
 * The main class to initialize the Time Service in the server mode.
 * Initialize the class by calling the TimeServiceServer->init() method
 */
@interface AJTMTimeServiceServer : NSObject

/**
 * Get Instance of AJTMTimeServiceServer - singleton implementation
 * @return instance
 */
+ (id)sharedInstance;

/**
 * Initialize the TimeServiceServer
 *
 * @param BusAttachment to be used by the TimeServiceServer
 */
-(QStatus)initWithBus:(AJNBusAttachment*) busAttachment;

/**
 * Stop the TimeServiceServer
 */
-(void)shutdown;

/**
 * Returns BusAttachment that is used by the TimeServiceServer.
 *
 * @return AJNBusAttachment or NULL if the BusAttachment is not initialized.
 */
-(AJNBusAttachment*)busAttachment;

/**
 * TimeServiceServer is started if the method TimeServiceServer->init(BusAttachment)
 * has been invoked
 * @return TRUE if the TimeServiceServer has been started
 */
-(bool)isStarted;

/**
 * Create Clock.
 * @param clock Methods of this Clock are invoked as a response to calls of the remote TimeServiceClient(s)
 * @return status of creating the Clock
 */
-(QStatus)createClock:(AJTMTimeServiceServerClockBase*) clock;

/**
 * Create Time Authority Clock.
 * The clock allows to send TimeServiceServerAuthorityClock.timeSync() signal to suggest synchronizing
 * time with it.
 *
 * @param clock Methods of this Clock are invoked as a response to calls of the remote TimeServiceClient(s)
 * @param type Options where this Time AuthorityClock synchronizes its time
 * @return status of creating the TimeAuthority clock
 */
-(QStatus)createTimeAuthorityClock:(AJTMTimeServiceServerAuthorityClock*) clock authorityType:(AJTMClockAuthorityType) authorityType;

/**
 * Create Alarm.
 *
 * @param alarm Methods of this Alarm are invoked
 * as a response to calls of the remote TimeServiceClient(s)
 * @return status of creating the alarm
 */
-(QStatus)createAlarm:(AJTMTimeServiceServerAlarmBase*) alarm;

/**
 * Create Alarm with description.
 *
 * @param alarm Methods of this TimeServiceServerAlarm are invoked
 * as a response to calls of the remote TimeServiceClient(s)
 * @param description Description to be added to the object when registering TimeServiceServerAlarm.
 * Since description is provided, org.allseen.Introspectable interface is added to the announced interfaces
 * to support Events & Actions feature.
 * @param language The language of the description
 * @param translator If not NULL, TimeServiceServerAlarm is registered with Translator
 * to support the feature of AllSeenIntrospectable.GetDescriptionLanguages()
 * @return status of creating the alarm
 */
-(QStatus)createAlarm:(AJTMTimeServiceServerAlarmBase*) alarm description:(NSString*) description language:(NSString*) language translator:(id<AJNTranslator>) translator;


/**
 * Register user defined custom TimeServiceServerAlarm.
 * To register custom Alarm inherit from TimeServiceAlarmBusObj.
 * This method registers the given alarmBusObj and searches for the interfaces to be added to AboutService
 * and later announced.
 * To exclude interfaces from being announced, notAnnounced vector should be provided.
 * @param alarmBusObj Custom Alarm to be registered
 * @param alarm Methods of this Alarm are invoked
 * as a response to calls of the remote TimeServiceClient(s)
 * @param notAnnounced AllJoyn name of the interfaces that should NOT be announced. If this vector is empty,
 * all the found interfaces are added to the AboutService to be announced.
 * @return status of creating the alarm
 */
-(QStatus)registerCustomAlarm:(AJTMTimeServiceServerAlarmBase*) alarmBusObj alarm:(id<AJTMTimeServiceServerAlarm>) alarm notAnnounced:(NSArray*) notAnnounced;

/**
 * Register user defined custom TimeServiceServerAlarm.
 * To register custom Alarm inherit from TimeServiceAlarmBusObj.
 * This method registers the given alarmBusObj and searches for the interfaces to be added to AboutService
 * and later announced.
 * To exclude interfaces from being announced, notAnnounced vector should be provided.
 * @param alarmBusObj Custom Alarm to be registered
 * @param alarm Methods of this TimeServiceServerAlarm are invoked
 * as a response to calls of the remote TimeServiceClient(s)
 * @param notAnnounced AllJoyn name of the interfaces that should NOT be announced. If this vector is empty,
 * all the found interfaces are added to the AboutService to be announced.
 * @param description Description to be added to the TimeServiceAlarmBusObject
 * Since description is provided, org.allseen.Introspectable interface is added to the announced interfaces
 * to support Events & Actions feature.
 * @param language The language of the description
 * @param translator If not NULL, TimeServiceAlarmBusObj is registered with Translator
 * to support the feature of AllSeenIntrospectable.GetDescriptionLanguages()
 * @return status of creating the alarm
 */
-(QStatus)registerCustomAlarm:(id<AJTMTimeServiceAlarmBusObj>) alarmBusObj alarm:(AJTMTimeServiceServerAlarmBase*) alarm notAnnounced:(NSArray*) notAnnounced description:(NSString*) description language:(NSString*) language translator:(id<AJNTranslator>) translator;

/**
 * Create Timer.
 *
 * @param timer Methods of this Timer are invoked
 * as a response to calls of the remote TimeServiceClient(s)
 * @return status of creating the timer
 */
-(QStatus)createTimer:(AJTMTimeServiceServerTimerBase*) timer;

/**
 * Create Timer with description.
 *
 * @param timer Methods of this TimeServiceServerTimer are invoked
 * as a response to calls of the remote TimeServiceClient(s)
 * @param description Description to be added to the object when registering TimeServiceServerTimer.
 * Since description is provided, org.allseen.Introspectable interface is added to the announced interfaces
 * to support Events & Actions feature.
 * @param language The language of the description
 * @param translator If not NULL, TimeServiceServerTimer is registered with Translator
 * to support the feature of AllSeenIntrospectable.GetDescriptionLanguages()
 * @return status of creating the timer
 */
-(QStatus)createTimer:(AJTMTimeServiceServerTimerBase*) timer description:(NSString*) description language:(NSString*) language translator:(id<AJNTranslator>) translator;

/**
 * Register user defined custom TimeServiceServerTimer.
 * To register custom Timer inherit from TimeServiceTimerBusObj.
 * This method registers the given timerBusObj and searches for the interfaces to be added to AboutService
 * and later announced.
 * To exclude interfaces from being announced, notAnnounced vector should be provided.
 * @param timerBusObj Custom Timer to be registered
 * @param timer Methods of this Timer are invoked
 * as a response to calls of the remote TimeServiceClient(s)
 * @param notAnnounced AllJoyn name of the interfaces that should NOT be announced. If this vector is empty,
 * all the found interfaces are added to the AboutService to be announced.
 * @return status of creating the timer
 */
-(QStatus)registerCustomTimer:(AJTMTimeServiceServerTimerBase*) timerBusObj timer:(id<AJTMTimeServiceServerTimer>) timer notAnnounced:(NSArray*) notAnnounced;//const std::vector<qcc::String>&

/**
 * Register user defined custom TimeServiceServerTimer.
 * To register custom Timer inherit from TimeServiceTimerBusObj.
 * This method registers the given timerBusObj and searches for the interfaces to be added to AboutService
 * and later announced.
 * To exclude interfaces from being announced, notAnnounced vector should be provided.
 * @param timerBusObj Custom Timer to be registered
 * @param timer Methods of this TimeServiceServerTimer are invoked
 * as a response to calls of the remote TimeServiceClient(s)
 * @param notAnnounced AllJoyn name of the interfaces that should NOT be announced. If this vector is empty,
 * all the found interfaces are added to the AboutService to be announced.
 * @param description Description to be added to the TimeServiceTimerBusObject
 * Since description is provided, org.allseen.Introspectable interface is added to the announced interfaces
 * to support Events & Actions feature.
 * @param language The language of the description
 * @param translator If not NULL, TimeServiceTimerBusObj is registered with Translator
 * to support the feature of AllSeenIntrospectable.GetDescriptionLanguages()
 * @return status of creating the timer
 */
-(QStatus)registerCustomTimer:(id<AJTMTimeServiceTimerBusObj>) timerBusObj timer:(AJTMTimeServiceServerTimerBase*) timer notAnnounced:(NSArray*) notAnnounced description:(NSString*) description language:(NSString*) language translator:(id<AJNTranslator>) translator;

/**
 * Create AlarmFactory.
 *
 * @param alarm Methods of this TimeServiceServerAlarmFactory are invoked
 * as a response to calls of the remote TimeServiceClient(s)
 * @return status of creating the AlarmFactory
 */
-(QStatus) createAlarmFactory:(AJTMTimeServiceServerAlarmFactoryBase*) alarmFactory;

/**
 * Create AlarmFactory with description.
 *
 * @param alarmFactory Methods of this TimeServiceServerAlarmFactory are invoked
 * as a response to calls of the remote TimeServiceClient(s)
 * @param description Description to be added to the object when TimeServiceServerAlarm is created
 * by the AlarmFactory.
 * Since description is provided, org.allseen.Introspectable interface is added to the announced interfaces
 * to support Events & Actions feature.
 * @param language The language of the description
 * @param translator If not NULL, Created TimeServiceServerAlarm is registered with Translator
 * to support the feature of AllSeenIntrospectable.GetDescriptionLanguages()
 *
 * @return status of creating the alarm
 */
-(QStatus) createAlarmFactory:(AJTMTimeServiceServerAlarmFactoryBase*) alarmFactory description:(NSString*) description language:(NSString*) language translator:(id<AJNTranslator>) translator;

/**
 * Create TimerFactory.
 *
 * @param timer Methods of this TimeServiceServerTimerFactory are invoked
 * as a response to calls of the remote TimeServiceClient(s)
 * @return status of creating the TimerFactory
 */
-(QStatus) createTimerFactory:(AJTMTimeServiceServerTimerFactoryBase*) timerFactory;

/**
 * Create TimerFactory with description.
 *
 * @param timerFactory Methods of this TimeServiceServerTimerFactory are invoked
 * as a response to calls of the remote TimeServiceClient(s)
 * @param description Description to be added to the object when TimeServiceServerTimer is created
 * by the TimerFactory.
 * Since description is provided, org.allseen.Introspectable interface is added to the announced interfaces
 * to support Events & Actions feature.
 * @param language The language of the description
 * @param translator If not NULL, Created TimeServiceServerTimer is registered with Translator
 * to support the feature of AllSeenIntrospectable.GetDescriptionLanguages()
 *
 * @return status of creating the timer
 */
-(QStatus) createTimerFactory:(AJTMTimeServiceServerTimerFactoryBase*) timerFactory description:(NSString*) description language:(NSString*) language translator:(id<AJNTranslator>) translator;

/**
 * Generate an object path.
 * To be called when creating a new instance of a custom alarm or timer.
 *
 * @param objectPath should contain the prefix for the object path, if status is ok the full object path is returned here.
 * @return status of creating the object path
 */
-(QStatus)generateObjectPath:(NSString*) objectPath;

@end
