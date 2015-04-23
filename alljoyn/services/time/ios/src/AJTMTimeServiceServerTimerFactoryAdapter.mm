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

#include "AJTMTimeServiceServerTimerFactoryAdapter.h"

AJTMTimeServiceServerTimerFactoryAdapter::AJTMTimeServiceServerTimerFactoryAdapter(id<AJTMTimeServiceServerTimerFactory> timeServiceServerTimerFactory)
{
    handle = timeServiceServerTimerFactory;
}

AJTMTimeServiceServerTimerFactoryAdapter::~AJTMTimeServiceServerTimerFactoryAdapter()
{

}

qcc::String const& AJTMTimeServiceServerTimerFactoryAdapter::getObjectPath() const
{
	return this->getObjectPath();
}

QStatus AJTMTimeServiceServerTimerFactoryAdapter::newTimer(ajn::services::TimeServiceServerTimer** ptrTimer, ajn::services::tsConsts::ErrorInfo* errorInfo)
{
    AJTMTimeServiceServerTimerBase *timer;
    AJTMErrorInfo *error = [[AJTMErrorInfo alloc]init];
    QStatus status = [handle newTimer:&timer errorInfo:error];

    if (status == ER_OK) {
        *ptrTimer = (__bridge ajn::services::TimeServiceServerTimer*)[timer getHandle];
    }
    else
    {
        errorInfo->errorDescription = [error errorDescription].UTF8String;
        errorInfo->errorName = [error errorName].UTF8String;
    }

    return status;
}


QStatus AJTMTimeServiceServerTimerFactoryAdapter::deleteTimer(const qcc::String& objectPath, ajn::services::tsConsts::ErrorInfo* errorInfo)
{
    AJTMErrorInfo *error = [[AJTMErrorInfo alloc]init];

    QStatus status = [handle deleteTimer:[NSString stringWithUTF8String:objectPath.c_str()] errorInfo:error];

    if (status != ER_OK) {
        errorInfo->errorDescription = [error errorDescription].UTF8String;
        errorInfo->errorName = [error errorName].UTF8String;
    }

    return status;
}


/**
 * The method is invoked by the TimeService, when there was a failure to create NewTimer,
 * or to construct a response to the client that has invoked the NewTimer method.
 * Application that implementing this method is expected to invoke TimeServiceServerTimer.release()
 * to release object resources.
 *
 * @param Timer TimeServiceServerTimer that failed to be created
 */
void AJTMTimeServiceServerTimerFactoryAdapter::handleError(ajn::services::TimeServiceServerTimer* timer)
{
    [handle handleError:timer];

    timer->release();
}
