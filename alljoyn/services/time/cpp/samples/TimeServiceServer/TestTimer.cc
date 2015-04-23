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

#include "TestTimer.h"

using namespace ajn;
using namespace services;

//Constructor
TestTimer::TestTimer() : TimeServiceServerTimer(), m_Interval(), m_TimeLeft(), m_IsRunning(false), m_Repeat(0)
{
}

//Destructor
TestTimer::~TestTimer() {
}


const TimeServicePeriod& TestTimer::getInterval()
{
    printf("get interval:");
    printPeriod(m_Interval);
    return m_Interval;
}

QStatus TestTimer::setInterval(const TimeServicePeriod& interval)
{
    printf("set interval:");
    printPeriod(interval);

    if (interval.isValid()) {
        m_Interval = interval;
        m_TimeLeft = interval;
        return ER_OK;
    }

    return ER_BAD_ARG_1;
}


const TimeServicePeriod& TestTimer::getTimeLeft()
{
    printf("get time left:");
    printPeriod(m_TimeLeft);

    return m_TimeLeft;
}

const qcc::String& TestTimer::getTitle()
{
    return m_Title;

}


void TestTimer::setTitle(const qcc::String& title)
{
    m_Title = title;

}


const bool TestTimer::isRunning()
{
    return m_IsRunning;
}


const uint16_t TestTimer::getRepeat()
{
    return m_Repeat;
}


void TestTimer::setRepeat(uint16_t repeat)
{
    m_Repeat = repeat;
}

/**
 * Start Timer
 */
void TestTimer::start()
{
    setIsRunning(true);
}

/**
 * Reset Timer
 */
void TestTimer::reset()
{
    setIsRunning(false);
}

/**
 * Pause
 */
void TestTimer::pause()
{
    setIsRunning(false);
}

void TestTimer::setIsRunning(bool isRunning)
{
    m_IsRunning = isRunning;

    QStatus status = runStateChanged(isRunning);

    if (status != ER_OK) {

        printf("Failed to send RunStateChanged: '%u'", status);
    }

    if (!isRunning) {

        status = timerEvent();

        if (status != ER_OK) {

            printf("Failed to send TimerEvent: '%u'", status);
        }
    }
}

/**
 * Print out the Period
 */

void TestTimer::printPeriod(const TimeServicePeriod& interval) const
{
    printf("{Period: {Time %d:%d:%d.%d}}\n",  (int)interval.getHour(), (int)interval.getMinute(), interval.getSecond(), interval.getMillisecond());
}


