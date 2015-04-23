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

#ifndef DATETIME_H_
#define DATETIME_H_

#include <alljoyn/time/LogModule.h>
#include <alljoyn/time/TimeServiceDate.h>
#include <alljoyn/time/TimeServiceTime.h>
#include <alljoyn/Status.h>

namespace ajn {
namespace services {

/**
 * Date & Time data type
 */
class TimeServiceDateTime {

  public:

    /**
     * Constructor
     */
    TimeServiceDateTime();

    /**
     * Destructor
     */
    ~TimeServiceDateTime();

    /**
     * Initialize the object with data.
     *
     * @param date
     * @param time
     * @param offsetMinutes
     * @return ER_OK if the object was initialized successfully and valid arguments have been passed to this method,
     * otherwise ER_BAD_ARGUMENT of the appropriate argument is returned
     */
    QStatus init(const TimeServiceDate& date, const TimeServiceTime& time, int16_t offsetMinutes);

    /**
     * Checks whether data of the object is valid, the object variables have a correct values, date and time are valid
     */
    bool isValid() const;

    /**
     * Returns Date
     *
     * @return Date
     */
    const TimeServiceDate& getDate() const;


    /**
     * Returns Time
     *
     * @return Time
     */
    const TimeServiceTime& getTime() const;

    /**
     * Returns Offset minutes
     *
     * @return Offset minutes
     */
    const int16_t getOffsetMinutes() const;

  private:

    /**
     * Date
     */
    TimeServiceDate m_Date;

    /**
     * Time
     */
    TimeServiceTime m_Time;

    /**
     * Offset minutes
     */
    int16_t m_OffsetMinutes;
};

} /* namespace services */
} /* namespace ajn */

#endif /* DATETIME_H_ */
