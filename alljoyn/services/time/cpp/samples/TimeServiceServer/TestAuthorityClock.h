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

#ifndef TESTAUTHORITYCLOCK_H_
#define TESTAUTHORITYCLOCK_H_

#include "TestClock.h"
#include <alljoyn/time/TimeServiceServerClock.h>
#include <alljoyn/time/TimeServiceServerAuthorityClock.h>
#include <alljoyn/time/TimeServiceDateTime.h>

namespace ajn {
namespace services {

class TestAuthorityClock : public TimeServiceServerAuthorityClock {

  public:

    /**
     * Constructor
     */
    TestAuthorityClock();

    /**
     * Destructor
     */
    ~TestAuthorityClock();

    /**
     * Returns DateTime object
     *
     * @return DateTime object
     */
    const TimeServiceDateTime& getDateTime() const;

    /**
     * Set the DateTime object
     *
     * @param dateTime DateTime
     */
    void setDateTime(TimeServiceDateTime const& dateTime);

    /**
     * Whether or not this clock has ever been set.
     *
     * @return Returns TRUE if the Clock has been set since the last reboot
     */
    bool isSet() const;

  private:

    TestClock m_Clock;
};

} /* namespace services */
} /* namespace ajn */

#endif /* TESTAUTHORITYCLOCK_H_ */
