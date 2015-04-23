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

#ifndef TIME_H_
#define TIME_H_

#include <alljoyn/time/LogModule.h>
#include <alljoyn/Status.h>

namespace ajn {
namespace services {

/**
 * Time data type which is used by the Alarm and Clock
 */
class TimeServiceTime {

  public:

    /**
     * Constructor
     */
    TimeServiceTime();

    /**
     * Destructor
     */
    ~TimeServiceTime();

    /**
     * Initialize the object with its data.
     *
     * @param hour Expected range: 0-23
     * @param minute Expected range: 0-59
     * @param second Expected range: 0-59
     * @param millisecond Expected range: 0-999
     *
     * @return ER_OK if the object was initialized successfully and valid arguments have been passed to this method,
     * otherwise ER_BAD_ARGUMENT status of the appropriate argument is returned
     */
    QStatus init(uint8_t hour, uint8_t minute, uint8_t second, uint16_t millisecond);

    /**
     * Checks whether data of the object is valid, the object variables have a correct values.
     *
     *  - hour Expected range: 0-23
     *  - minute Expected range: 0-59
     *  - second Expected range: 0-59
     *  - millisecond Expected range: 0-999
     *
     *  @return TRUE if this object is valid which means that is was initialized with the valid arguments
     */
    bool isValid() const;

    /**
     * Returns hour
     *
     * @return hour
     */
    const uint8_t getHour() const;

    /**
     * Returns minute
     *
     * @return minute
     */
    const uint8_t getMinute() const;

    /**
     * Returns second
     *
     * @return second
     */
    const uint8_t getSecond() const;

    /**
     * Returns milliseconds
     *
     * @return milliseconds
     */
    const uint16_t getMillisecond() const;

  private:

    /**
     * Hour Expected range: 0-23
     */
    uint8_t m_Hour;

    /**
     * Minute Expected range: 0-59
     */
    uint8_t m_Minute;

    /**
     * Second Expected range: 0-59
     */
    uint8_t m_Second;

    /**
     * Millisecond Expected range: 0-999
     */
    uint16_t m_Millisecond;

    /**
     * Checks whether data of the object is valid, the object variables have a correct values.
     *
     * @param hour Expected range: 0-23
     * @param minute Expected range: 0-59
     * @param second Expected range: 0-59
     * @param millisecond Expected range: 0-999
     * @param status ER_OK if all the arguments are valid, or BAD_ARGUMENT status for the bad argument
     */
    bool isValid(uint8_t hour, uint8_t minute, uint8_t second, uint16_t millisecond, QStatus& status) const;
};

} /* namespace services */
} /* namespace ajn */

#endif /* TIME_H_ */
