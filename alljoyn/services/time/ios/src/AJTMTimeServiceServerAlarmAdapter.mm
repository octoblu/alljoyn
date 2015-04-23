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

#include "AJTMTimeServiceServerAlarmAdapter.h"


AJTMTimeServiceServerAlarmAdapter::AJTMTimeServiceServerAlarmAdapter(id<AJTMTimeServiceServerAlarm> timeServiceServerAlarm)
{
    handle = timeServiceServerAlarm;
}

AJTMTimeServiceServerAlarmAdapter::~AJTMTimeServiceServerAlarmAdapter()
{

}

const ajn::services::TimeServiceSchedule& AJTMTimeServiceServerAlarmAdapter::getSchedule()
{
   return [[handle schedule] getHandle];
}

QStatus AJTMTimeServiceServerAlarmAdapter::setSchedule(const ajn::services::TimeServiceSchedule& schedule)
{
    AJTMTimeServiceSchedule *a = [[AJTMTimeServiceSchedule alloc]init];

    ajn::services::TimeServiceTime *time = new ajn::services::TimeServiceTime();
    time->init(schedule.getTime().getHour(),schedule.getTime().getMinute(), schedule.getTime().getSecond(), schedule.getTime().getMillisecond());
    AJTMTimeServiceTime *objtime = [[AJTMTimeServiceTime alloc]initWithHandle:time];
    [a populateWithTime:objtime weekDays:schedule.getWeekDays()];

    return [handle setSchedule:a];
}

const qcc::String& AJTMTimeServiceServerAlarmAdapter::getTitle()
{
    NSString* title = [handle title];
    qcc::String *str = new qcc::String([title UTF8String]);
    return *str; //the cpp interface demands this. we may have a memory leak here. maybe obj-c ARC is a solution.
}

void AJTMTimeServiceServerAlarmAdapter::setTitle(const qcc::String& title)
{
    [handle setTitle:(@(title.c_str()))];
}

const bool AJTMTimeServiceServerAlarmAdapter::isEnabled()
{
    return [handle isEnabled];
}

void AJTMTimeServiceServerAlarmAdapter::setEnabled(bool enabled)
{
    [handle setEnabled:enabled];
}