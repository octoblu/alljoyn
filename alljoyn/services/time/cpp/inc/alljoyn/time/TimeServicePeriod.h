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

#ifndef PERIOD_H_
#define PERIOD_H_

#include <alljoyn/Status.h>
#include <alljoyn/time/LogModule.h>

namespace ajn {
namespace services {

/**
 * Period is used to indicate time interval of the Timer.
 */
class TimeServicePeriod {

  public:

    /**
     * Constructor
     */
    TimeServicePeriod();

    /**
     * Destructor
     */
    ~TimeServicePeriod();

    /**
     * Initialize the object with its data.
     *
     * @param hour A positive number
     * @param minute Expected range: 0-59
     * @param second Expected range: 0-59
     * @param millisecond Expected range: 0-999
     * @return ER_OK if the object was initialized successfully and valid arguments have been passed to this method,
     * otherwise ER_BAD_ARGUMENT status of the appropriate argument is returned
     */
    QStatus init(uint32_t hour, uint8_t minute, uint8_t second, uint16_t millisecond);

    /**
     * Checks whether data of the object is valid, the object variables have a correct values.
     *
     * @return TRUE of the object is valid
     */
    bool isValid() const;

    /**
     * Returns Hour
     *
     * @return Returns hour
     */
    const uint32_t getHour() const;

    /**
     * Returns Minute
     *
     * @return Returns minute
     */
    const uint8_t getMinute() const;

    /**
     * Returns second
     *
     * @return Returns second
     */
    const uint8_t getSecond() const;

    /**
     * Returns millisecond
     *
     * @return Returns millisecond
     */
    const uint16_t getMillisecond() const;

  private:

    /**
     * hour
     */
    uint32_t m_Hour;

    /**
     * minute Expected range: 0-59
     */
    uint8_t m_Minute;

    /**
     * second Expected range: 0-59
     */
    uint8_t m_Second;

    /**
     * millisecond Expected range: 0-999
     */
    uint16_t m_Millisecond;

    /**
     * Checks data validity
     *
     * @param status ER_OK if all the arguments are valid, or BAD_ARGUMENT status for the bad argument
     */
    bool isValid(uint32_t hour, uint8_t minute, uint8_t second, uint16_t millisecond, QStatus& status) const;
};

} /* namespace services */
} /* namespace ajn */

#endif /* PERIOD_H_ */
