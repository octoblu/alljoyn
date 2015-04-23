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

#include "AJTMTimeServiceServerAlarmFactoryAdapter.h"
#include "AJTMTimeServiceServerAlarmBase.h"

AJTMTimeServiceServerAlarmFactoryAdapter::AJTMTimeServiceServerAlarmFactoryAdapter(id<AJTMTimeServiceServerAlarmFactory> timeServiceServerAlarmFactory)
{
    handle = timeServiceServerAlarmFactory;
}


AJTMTimeServiceServerAlarmFactoryAdapter::~AJTMTimeServiceServerAlarmFactoryAdapter()
{

}

qcc::String const& AJTMTimeServiceServerAlarmFactoryAdapter::getObjectPath() const
{
    return this->getObjectPath();
}


QStatus AJTMTimeServiceServerAlarmFactoryAdapter::newAlarm(ajn::services::TimeServiceServerAlarm** ptrAlarm, ajn::services::tsConsts::ErrorInfo* errorInfo)
{
    AJTMTimeServiceServerAlarmBase *alarm;
    AJTMErrorInfo *error = [[AJTMErrorInfo alloc]init];
    QStatus status = [handle newAlarm:&alarm errorInfo:error];

    if (status == ER_OK) {
        *ptrAlarm = (__bridge ajn::services::TimeServiceServerAlarm*)[alarm getHandle];
    }
    else
    {
        errorInfo->errorDescription = [error errorDescription].UTF8String;
        errorInfo->errorName = [error errorName].UTF8String;
    }

    return status;
}


QStatus AJTMTimeServiceServerAlarmFactoryAdapter::deleteAlarm(const qcc::String& objectPath, ajn::services::tsConsts::ErrorInfo* errorInfo)
{
    AJTMErrorInfo* t_errorInfo = [[AJTMErrorInfo alloc] initWithErrorName:@((errorInfo->errorName).c_str()) errorDescription:@((errorInfo->errorDescription).c_str())];

    return [handle deleteAlarm:@(objectPath.c_str()) errorInfo:t_errorInfo];

}

void AJTMTimeServiceServerAlarmFactoryAdapter::handleError(ajn::services::TimeServiceServerAlarm* alarm)
{
    [handle handleError:alarm];


    alarm->release();    
}
