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

#include <alljoyn/controlpanel/CPSTime.h>

namespace ajn {
namespace services {

CPSTime::CPSTime(uint16_t hour, uint16_t minute, uint16_t second) :
    m_Hour(hour), m_Minute(minute), m_Second(second)
{
}

CPSTime::~CPSTime()
{
}

uint16_t CPSTime::getHour() const
{
    return m_Hour;
}

void CPSTime::setHour(uint16_t hour)
{
    m_Hour = hour;
}

uint16_t CPSTime::getMinute() const
{
    return m_Minute;
}

void CPSTime::setMinute(uint16_t minute)
{
    m_Minute = minute;
}

uint16_t CPSTime::getSecond() const
{
    return m_Second;
}

void CPSTime::setSecond(uint16_t second)
{
    m_Second = second;
}

} /* namespace services */
} /* namespace ajn */
