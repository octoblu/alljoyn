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
#import "alljoyn/time/TimeServiceClientTimerFactory.h"
#import "AJTMTimeServiceObjectIntrospector.h"

@interface AJTMTimeServiceClientTimerFactory : AJTMTimeServiceObjectIntrospector
/* Constructor
 * @param handle A handle to a cpp object
 */
- (id)initWithHandle:(ajn::services::TimeServiceClientTimerFactory*)handle;

/**
 * Release object resources
 */
-(void)releaseObject;

/**
 * Creates new Timer
 *
 * @param Out variable. Object path of the new created Timer.
 * Use this object path to create TimeServiceClientTimer.
 *
 * @return status ER_OK, or a failure status if failed to create new Timer
 */
-(QStatus)newTimer:(NSString**)objectPath;

/**
 * Delete Timer identified by the given object path.
 *
 * @param object path of the Timer to be deleted.
 * Retrieve the object path by the call to TimeServiceClientTimer.getObjectPath()
 *
 * @return status ER_OK, or a failure status if failed to delete the Timer
 */
-(QStatus)deleteTimer:(NSString *)objectPath;

/**
 * Retrieves a list of timer object paths created by the TimerFactory.
 * Use this object path to create TimeServiceClientTimer.
 *
 * @param objectPathList Pointer to the vector to be populated with the object paths of the
 * Timers created by this TimerFactory
 *
 * @return status ER_OK, or a failure status if failed to retrieve list of Timers
 */
//std::vector<qcc::String>* objectPathList
-(QStatus)retrieveTimerList:(NSArray**)objectPathList;

@end
