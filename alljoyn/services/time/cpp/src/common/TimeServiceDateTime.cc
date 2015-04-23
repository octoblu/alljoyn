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

#include <alljoyn/time/TimeServiceDateTime.h>

using namespace ajn;
using namespace services;

/**
 * Constructor
 */
TimeServiceDateTime::TimeServiceDateTime() : m_OffsetMinutes(0)
{
}

/**
 * Destructor
 */
TimeServiceDateTime::~TimeServiceDateTime()
{
}

//Initialize the DateTime object
QStatus TimeServiceDateTime::init(const TimeServiceDate& date, const TimeServiceTime& time, int16_t offsetMinutes)
{

    if (!date.isValid()) {

        QCC_LogError(ER_BAD_ARG_1, ("Date is not a valid object"));
        return ER_BAD_ARG_1;
    }

    if (!time.isValid()) {

        QCC_LogError(ER_BAD_ARG_2, ("Time is not a valid object"));
        return ER_BAD_ARG_2;
    }

    m_Date          = date;
    m_Time          = time;
    m_OffsetMinutes = offsetMinutes;

    return ER_OK;
}

//Checks whether data of the object is valid, the object variables have a correct values, date and time are valid
bool TimeServiceDateTime::isValid() const
{

    return m_Date.isValid() && m_Time.isValid();
}

//Get Date
const TimeServiceDate& TimeServiceDateTime::getDate() const
{

    return m_Date;
}

//Get Time
const TimeServiceTime& TimeServiceDateTime::getTime() const
{

    return m_Time;
}

//Get offset
const int16_t TimeServiceDateTime::getOffsetMinutes() const
{

    return m_OffsetMinutes;
}
