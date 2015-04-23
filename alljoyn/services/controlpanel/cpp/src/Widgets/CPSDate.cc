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

#include <alljoyn/controlpanel/CPSDate.h>

namespace ajn {
namespace services {

CPSDate::CPSDate(uint16_t day, uint16_t month, uint16_t year) :
    m_Day(day), m_Month(month), m_Year(year)
{
}

CPSDate::~CPSDate()
{
}

uint16_t CPSDate::getDay() const
{
    return m_Day;
}

void CPSDate::setDay(uint16_t day)
{
    m_Day = day;
}

uint16_t CPSDate::getMonth() const
{
    return m_Month;
}

void CPSDate::setMonth(uint16_t month)
{
    m_Month = month;
}

uint16_t CPSDate::getYear() const
{
    return m_Year;
}

void CPSDate::setYear(uint16_t year)
{
    m_Year = year;
}

} /* namespace services */
} /* namespace ajn */
