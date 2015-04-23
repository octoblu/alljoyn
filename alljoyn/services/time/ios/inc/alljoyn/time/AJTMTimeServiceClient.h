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
#import "alljoyn/time/TimeServiceClient.h"
#import "AJNBusAttachment.h"
#import "AJTMTimeServiceSessionListener.h"

@interface AJTMTimeServiceClient : NSObject

-(id)init;

/* Constructor
 * @param handle A handle to a cpp object
 */
- (id)initWithHandle:(ajn::services::TimeServiceClient*)handle;

    /**
     * Initialize TimeServiceClient
     * @param bus The BusAttachment that is used by the TimeServiceClient
     * to reach the TimeServiceServer
     * @param serverBusName The bus name to reach the TimeServiceServer
     * @param deviceId The id of the device hosting the TimeServiceServer
     * @param appId The name of the application that started the TimeServiceServer
     * @param objDescs AnnounceHandler::ObjectDescriptions received with Announcement
     *
     * @return ER_OK if succeeded to initialize the TimeServiceClient
     */
-(QStatus)populateWithBus:(AJNBusAttachment*) busAttachment serverBusName:(NSString *)serverBusName deviceId:(NSString *)deviceId appId:(NSString *)appId objDescArgs:(AJNMessageArgument *)objDescsArgs;

    /**
     * Cleans the object and releases its resources.
     * It's a programming error to call another method on this object after the release method has been called.
     */
-(void) releaseObject;

    /**
     * BusAttachment that is used by the TimeServiceClient
     *
     * @return BusAttachment that is used by the TimeServiceClient
     */
-(AJNBusAttachment*) getBus;

    /**
     * The bus name that is used to reach TimeServiceServer
     *
     * @return The bus name that is used to reach TimeServiceServer
     */
-(NSString *)getServerBusName;

    /**
     * The id of the device hosting the TimeServiceServer
     *
     * @return Device id
     */
-(NSString *)getDeviceId;

    /**
     * The name of the application that started the TimeServiceServer
     *
     * @return The application name
     */
-(NSString *)getAppId;

    /**
     * Returns list of the TimeServiceClientClock objects found in the received object descriptions
     *
     * @return vector of the TimeServiceClientClock objects
     */
//    const std::vector<TimeServiceClientClock*>&
-(NSArray *)getAnnouncedClockList;

    /**
     * Returns list of the TimeServiceClientAlarm objects found in the received object descriptions
     *
     * @return vector of the TimeServiceClientAlarm objects
     */
    //const std::vector<TimeServiceClientAlarm*>&
-(NSArray *)getAnnouncedAlarmList;

    /**
     * Returns list of the TimeServiceClientAlarmFactory objects found in the received object descriptions
     *
     * @return vector of the TimeServiceClientAlarmFactory objects
     */
//    const std::vector<TimeServiceClientAlarmFactory*>&
-(NSArray *)getAnnouncedAlarmFactoryList;

    /**
     * Returns list of the TimeServiceClientTimer objects found in the received object descriptions
     *
     * @return vector of the TimeServiceClientTimer objects
     */
//    const std::vector<TimeServiceClientTimer*>&
-(NSArray *)getAnnouncedTimerList;

    /**
     * Returns list of the TimeServiceClientTimerFactory objects found in the received object descriptions
     *
     * @return vector of the TimeServiceClientTimerFactory objects
     */
  //  const std::vector<TimeServiceClientTimerFactory*>&
-(NSArray *)getAnnouncedTimerFactoryList;

    /**
     * Creates the session asynchronously with the server.
     *
     * @param sessionListener Session related events will be passed via the TimeServiceSessionListener
     *
     * @return status ER_OK If the join session call succeeded
     */
-(QStatus)joinSessionAsyncWithListener:(id<AJTMTimeServiceSessionListener>)sessionListener; 

    /**
     * Leave previously established session
     *
     * @return status ER_OK if the session was disconnected successfully
     */
-(QStatus)leaveSession;

    /**
     * Return whether a session with the TimeServiceServer has been created
     *
     * @return TRUE if the session has been created
     */
-(bool) isConnected;

    /**
     * Set the given sessionId with id of the session that has been established with the TimeServiceServer.
     *
     * @param sessionId Out variable. Set to be session id that has been established with the server.
     *
     * @return ER_OK if the connection with the server has been established.
     */
-(QStatus)getSessionId:(AJNSessionId *)sessionId;

-(ajn::services::TimeServiceClient*)getHandle;


@end
