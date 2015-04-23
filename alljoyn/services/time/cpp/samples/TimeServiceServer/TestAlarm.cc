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

#include "TestAlarm.h"

using namespace ajn;
using namespace services;

//Constructor
TestAlarm::TestAlarm() : TimeServiceServerAlarm(), m_Enabled(false)
{
}

//Destructor
TestAlarm::~TestAlarm() {
}


const TimeServiceSchedule& TestAlarm::getSchedule()
{

    return m_Schedule;
}

/**
 * Set TimeServiceSchedule to this Alarm object
 */
QStatus TestAlarm::setSchedule(const TimeServiceSchedule& schedule)
{

    m_Schedule = schedule;

    printSchedule();

    return ER_OK;
}

/**
 * Returns Alarm title.
 *
 * @return Optional textual description of what this Alarm is
 */
const qcc::String& TestAlarm::getTitle()
{

    return m_Title;
}

/**
 * Set title to this Alarm.
 *
 * @param title Alarm title.
 * Title is an optional textual description of what this Alarm is set for.
 */
void TestAlarm::setTitle(const qcc::String& title)
{
    m_Title = title;
}

/**
 * Returns whether or not this Alarm is enabled
 *
 * @return Is Alarm enabled
 */
const bool TestAlarm::isEnabled()
{

    return m_Enabled;
}

/**
 * Set whether or not this Alarm is enabled
 *
 * @param enabled Is Alarm enabled
 */
void TestAlarm::setEnabled(bool enabled)
{

    m_Enabled = enabled;
}

/**
 * Print out the Schedule
 */
void TestAlarm::printSchedule() const
{
    const TimeServiceTime& time = m_Schedule.getTime();

    printf("{Schedule: {Time %d:%d:%d.%d} weekdays: '%s'}\n",  (int)time.getHour(), (int)time.getMinute(), time.getSecond(), time.getMillisecond()
           , getWeekdaysString().c_str());
}

const char*days[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

/**
 * Creates String representation of the week days
 */
qcc::String TestAlarm::getWeekdaysString() const
{
    int x;
    uint8_t weekdays = m_Schedule.getWeekDays();
    qcc::String str;

    for (x = 0; x != 7; x++) {

        if (weekdays & (1 << x)) {
            str = str + days[x] + " ";
        }
    }

    return str;
}

