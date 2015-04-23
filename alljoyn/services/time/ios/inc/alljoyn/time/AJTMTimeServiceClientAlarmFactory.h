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
#import "alljoyn/time/TimeServiceClientAlarmFactory.h"
#import "AJTMTimeServiceObjectIntrospector.h"

@interface AJTMTimeServiceClientAlarmFactory : AJTMTimeServiceObjectIntrospector
/* Constructor
 * @param handle A handle to a cpp object
 */
- (id)initWithHandle:(ajn::services::TimeServiceClientAlarmFactory*)handle;

/**
 * Release object resources
 */
-(void)releaseObject;

/**
 * Creates new Alarm
 *
 * @param Out variable. Object path of the new created Alarm.
 * Use this object path to create TimeServiceClientAlarm.
 *
 * @return status ER_OK, or a failure status if failed to create new Alarm
 */
-(QStatus)newAlarm:(NSString**)objectPath;

/**
 * Delete Alarm identified by the given object path.
 *
 * @param object path of the Alarm to be deleted.
 * Retrieve the object path by the call to TimeServiceClientAlarm.getObjectPath()
 *
 * @return status ER_OK, or a failure status if failed to delete the Alarm
 */
-(QStatus)deleteAlarm:(NSString *)objectPath;

/**
 * Retrieves a list of alarm object paths created by the AlarmFactory.
 * Use this object path to create TimeServiceClientAlarm.
 *
 * @param objectPathList Pointer to the vector to be populated with the object paths of the
 * Alarms created by this AlarmFactory
 *
 * @return status ER_OK, or a failure status if failed to retrieve list of Alarms
 */
//std::vector<qcc::String>* objectPathList
-(QStatus)retrieveAlarmList:(NSArray**)objectPathList;

@end
