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
#import "AJTMTimeServiceClientBase.h"
#import "alljoyn/time/TimeServiceClientClock.h"
#import "AJTMTimeServiceDateTime.h"

@class AJTMTimeServiceClientClock;


@protocol AJTMTimeAuthorityHandler <NSObject>

/**
 * Handle Time Sync event, which was sent from the given TimeServiceClientClock
 * @param clock The TimeServiceClientClock that received the TimeSync event.
 */
-(void)handleTimeSync:(AJTMTimeServiceClientClock*) clock;

@end

@interface AJTMTimeServiceClientClock : AJTMTimeServiceClientBase

/* Constructor
 * @param handle A handle to a cpp object
 */
- (id)initWithHandle:(ajn::services::TimeServiceClientClock*)handle;

/**
 * Release object resources
 */
-(void)releaseObject;

/**
 * This method to be called by the TimeServiceClient after finding out
 * whether this Clock is a reliable source of time.
 */
-(void)setAuthority:(bool) isAuthority;

/**
 * Returns whether the server side Clock is a reliable source of time.
 *
 * @return TRUE if server side Clock is a reliable source of time.
 */
-(bool)isAuthority;

/**
 * Retrieve DateTime from the server Clock object.
 *
 * @param dateTime Out variable. Is populated with the retrieved DateTime.
 *
 * @return status ER_OK, or a failure status if failed to retrieve DateTime
 */
-(QStatus)retrieveDateTime:(AJTMTimeServiceDateTime**) dateTime;

/**
 * Set DateTime to the server Clock object
 *
 * @param dateTime TimeServiceDateTime to set
 *
 * @return status
 */
-(QStatus)setDateTime:(AJTMTimeServiceDateTime *)dateTime;

/**
 * Retrieve IsSet from the server Clock object.
 *
 * @param isSet Out variable.
 *
 * @return status ER_OK, or a failure status if failed to retrieve IsSet
 */
-(QStatus)retrieveIsSet:(bool*) isSet;

/**
 * Retrieves Authority Type of the server clock.
 * This method is only valid for the Time Authority clock object.
 *
 * @param authType Out variable.
 *
 * @return status ER_OK, or a failure status if failed to retrieve authority type
 */
-(QStatus)retrieveAuthorityType:(ajn::services::tsConsts::ClockAuthorityType*) authType;

/*
 * Register TimeAuthorityHandler to receive TimeSync
 * events. This method is only valid for the Time Authority clocks.
 *
 * @param timeAuthorityHandler TimeAuthorityHandler
 *
 * @return status
 */
-(QStatus)registerTimeAuthorityHandler:(id<AJTMTimeAuthorityHandler>) handler;

/**
 * Unregister TimeAuthorityHandler to stop receiving TimeSync
 * events. This method is only valid for objects for the Time Authority clocks.
 */
-(void)unRegisterTimeAuthorityHandler;

/**
 * @return TimeAuthorityHandler that has been registered or NULL
 */
-(id<AJTMTimeAuthorityHandler>) getTimeAuthorityHandler;

@end

class TimeAuthorityHandlerAdapter : public ajn::services::TimeAuthorityHandler
{
public:
    id<AJTMTimeAuthorityHandler> handle;

    /**
     * Constructor
     */
    TimeAuthorityHandlerAdapter(id<AJTMTimeAuthorityHandler> timeAuthorityHandler):handle(timeAuthorityHandler){}

    void handleTimeSync(ajn::services::TimeServiceClientClock* clock) {[handle handleTimeSync:[[AJTMTimeServiceClientClock alloc]initWithHandle:clock]];}
};

