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

#include "AJTMTimeServiceServerTimerAdapter.h"
#include "AJTMTimeServicePeriod.h"

AJTMTimeServiceServerTimerAdapter::AJTMTimeServiceServerTimerAdapter(id <AJTMTimeServiceServerTimer> timeServiceServerTimer)
{
    handle = timeServiceServerTimer;
}

AJTMTimeServiceServerTimerAdapter::~AJTMTimeServiceServerTimerAdapter()
{
}

const ajn::services::TimeServicePeriod& AJTMTimeServiceServerTimerAdapter::getInterval()
{
    return [[handle interval] getHandle];
}

QStatus AJTMTimeServiceServerTimerAdapter::setInterval(const ajn::services::TimeServicePeriod& interval)
{
    AJTMTimeServicePeriod* timeServicePeriod;
    ajn::services::TimeServicePeriod *period = new ajn::services::TimeServicePeriod();

    period->init(interval.getHour(), interval.getMinute(), interval.getSecond(), interval.getMillisecond());

    timeServicePeriod = [[AJTMTimeServicePeriod alloc] initWithHandle: period];
    return [handle setInterval: timeServicePeriod];
}

const ajn::services::TimeServicePeriod& AJTMTimeServiceServerTimerAdapter::getTimeLeft()
{
    return [[handle timeLeft] getHandle];
}

const qcc::String& AJTMTimeServiceServerTimerAdapter::getTitle()
{
    NSString* title = [handle title];
    qcc::String *str = new qcc::String([title UTF8String]);
    return *str; //the cpp interface demands this. we may have a memory leak here. maybe obj-c ARC is a solution.
}

void AJTMTimeServiceServerTimerAdapter::setTitle(const qcc::String& title)
{
    [handle setTitle: (@(title.c_str()))];
}

const bool AJTMTimeServiceServerTimerAdapter::isRunning()
{
    return [handle isRunning];
}

const uint16_t AJTMTimeServiceServerTimerAdapter::getRepeat()
{
    return [handle repeat];
}

void AJTMTimeServiceServerTimerAdapter::setRepeat(uint16_t repeat)
{
    [handle setRepeat:repeat];
}

void AJTMTimeServiceServerTimerAdapter::start()
{
    [handle start];
}

void AJTMTimeServiceServerTimerAdapter::reset()
{
    [handle reset];
}

void AJTMTimeServiceServerTimerAdapter::pause()
{
    [handle pause];
}


void AJTMTimeServiceServerTimerAdapter::release()
{
    [handle releaseObject];
}