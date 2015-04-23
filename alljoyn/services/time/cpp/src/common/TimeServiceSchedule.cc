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

#include <alljoyn/time/TimeServiceSchedule.h>

using namespace ajn;
using namespace services;

//Constructor
TimeServiceSchedule::TimeServiceSchedule() : m_WeekDays(0)
{
}

//Destructor
TimeServiceSchedule::~TimeServiceSchedule() {
}

// Initialize the object
QStatus TimeServiceSchedule::init(const TimeServiceTime& time, uint8_t weekDays)
{

    if (!time.isValid()) {

        QCC_LogError(ER_BAD_ARG_1, ("Time is not a valid object"));
        return ER_BAD_ARG_1;
    }

    m_Time      = time;
    m_WeekDays  = weekDays;

    return ER_OK;
}

//Is valid
bool TimeServiceSchedule::isValid() const
{

    return m_Time.isValid();
}

//Returns Time
const TimeServiceTime& TimeServiceSchedule::getTime() const
{

    return m_Time;
}

//Returns Bitmap of WeekDay(s) when the Alarm should fire.
const uint8_t TimeServiceSchedule::getWeekDays() const
{

    return m_WeekDays;
}
