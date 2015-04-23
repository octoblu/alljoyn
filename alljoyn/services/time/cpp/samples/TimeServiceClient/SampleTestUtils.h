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

#ifndef TESTAPPUTILS_H_
#define TESTAPPUTILS_H_

#include <alljoyn/time/TimeServiceDateTime.h>
#include <alljoyn/time/TimeServiceSchedule.h>
#include <alljoyn/time/TimeServicePeriod.h>
#include <qcc/String.h>
#include <vector>

namespace ajn {
namespace services {

namespace sampleTestUtils {

/**
 * Print DateTime
 *
 * @param dateTime
 */
void printDateTime(TimeServiceDateTime const& dateTime);

/**
 * Generate DateTime
 *
 * @param dateTime
 * @param offsetSeconds to add to the localtime
 */
void dateTimeNow(TimeServiceDateTime* dateTime, uint16_t offsetSeconds = 0);

/**
 * Print Schedule
 *
 * @param schedule to print
 */
void printSchedule(const TimeServiceSchedule& schedule);

/**
 * @param weekdays bitmap
 *
 * @return Weekdays string representation
 */
qcc::String getWeekdaysString(const uint8_t weekDays);

/**
 * Converts from comma separated days of week (sun,mon,tue,...) to a weekDays bit map
 *
 * @param weekDaysStr comma separated days of week (sun,mon,tue,...)
 *
 * @return week days bit map
 */
uint8_t  getWeekdaysNum(std::string weekDaysStr);

/**
 * Split weekDaysStr by "," and populate weekDays vector
 *
 * @param weekDaysStr comma separated days of week (sun,mon,tue,...)
 * @param weekDays vector to be populated by the string tokens
 */
void weekDaysFromString(std::string weekDaysStr, std::vector<std::string>* weekDays);

/**
 * Print Period
 *
 * @param interval period to print
 * @param meaning meaning of this period
 */
void printPeriod(const TimeServicePeriod& interval, const char*meaning);

/**
 * Create period from string
 *
 * @param periodStr period to split. format hh:mm:ss
 * @param period returned period
 * @return false if failed
 */

bool periodFromString(const std::string& periodStr, TimeServicePeriod& period);

} /*sampleTestUtils*/
} /* namespace services */
} /* namespace ajn */

#endif /* TESTAPPUTILS_H_ */
