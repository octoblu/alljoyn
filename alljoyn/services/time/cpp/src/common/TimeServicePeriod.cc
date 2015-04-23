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

#include <alljoyn/time/TimeServicePeriod.h>

using namespace ajn;
using namespace services;

//Constructor
TimeServicePeriod::TimeServicePeriod() : m_Hour(0), m_Minute(0), m_Second(0), m_Millisecond(0)
{
}

//Destructor
TimeServicePeriod::~TimeServicePeriod()
{
}

//Initialize the object with its data.
QStatus TimeServicePeriod::init(uint32_t hour, uint8_t minute, uint8_t second, uint16_t millisecond)
{

    QStatus status;
    if (!isValid(hour, minute, second, millisecond, status)) {

        return status;
    }

    m_Hour        = hour;
    m_Minute      = minute;
    m_Second      = second;
    m_Millisecond = millisecond;

    return ER_OK;
}

//Checks whether data of the object is valid, the object variables have a correct values.
bool TimeServicePeriod::isValid() const
{
    QStatus status;
    return isValid(m_Hour, m_Minute, m_Second, m_Millisecond, status);
}

//Checks whether data of the object is valid, the object variables have a correct values.
bool TimeServicePeriod::isValid(uint32_t hour, uint8_t minute, uint8_t second, uint16_t millisecond, QStatus& status) const
{
    status = ER_OK;

    if (minute > 59) {

        status = ER_BAD_ARG_2;
        QCC_LogError(ER_BAD_ARG_2, ("Minute is not in the expected range"));
        return false;
    }

    if (second > 59) {

        status = ER_BAD_ARG_3;
        QCC_LogError(ER_BAD_ARG_3, ("Day is not in the correct range"));
        return false;
    }

    if (millisecond > 999) {

        status = ER_BAD_ARG_4;
        QCC_LogError(ER_BAD_ARG_4, ("Millisecond is not in the correct range"));
        return false;
    }

    return true;
}

//Returns hour
const uint32_t TimeServicePeriod::getHour() const
{

    return m_Hour;
}


//Returns minute
const uint8_t TimeServicePeriod::getMinute() const
{

    return m_Minute;
}

//Returns second
const uint8_t TimeServicePeriod::getSecond() const
{

    return m_Second;
}

//Returns millisecond
const uint16_t TimeServicePeriod::getMillisecond() const
{

    return m_Millisecond;
}
