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

#ifndef CPSDATE_H_
#define CPSDATE_H_

#include <qcc/platform.h>

namespace ajn {
namespace services {

/**
 * A class to allow sending of a Date as a Property
 */
class CPSDate {
  public:

    /**
     * Constructor of a CPSDate.
     * @param day - day of date
     * @param month - month of date
     * @param year - year of date
     */
    CPSDate(uint16_t day, uint16_t month, uint16_t year);

    /**
     * Destructor of CPSDate
     */
    virtual ~CPSDate();

    /**
     * Get the day value of the date
     * @return day value
     */
    uint16_t getDay() const;

    /**
     * Set the day Value of the date
     * @param day value
     */
    void setDay(uint16_t day);

    /**
     * Get the Month value of the date
     * @return month value
     */
    uint16_t getMonth() const;

    /**
     * Set the Month value of the date
     * @param month value
     */
    void setMonth(uint16_t month);

    /**
     * Get the Year value of the date
     * @return year value
     */
    uint16_t getYear() const;

    /**
     * Set the Year value of the date
     * @param year value
     */
    void setYear(uint16_t year);

  private:

    /**
     * Day value of the Date
     */
    uint16_t m_Day;

    /**
     * Month value of the Date
     */
    uint16_t m_Month;

    /**
     * Year value of the Date
     */
    uint16_t m_Year;
};

} /* namespace services */
} /* namespace ajn */
#endif /* CPSDATE_H_ */
