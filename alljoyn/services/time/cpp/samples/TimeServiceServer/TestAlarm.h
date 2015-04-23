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

#ifndef TestAlarm_H_
#define TestAlarm_H_

#include <alljoyn/time/TimeServiceServerAlarm.h>
#include <alljoyn/time/TimeServiceSchedule.h>

namespace ajn {
namespace services {

/**
 * Test Server Alarm functionality
 */
class TestAlarm : public TimeServiceServerAlarm {

  public:

    /**
     * Constructor
     */
    TestAlarm();

    /**
     * Destructor
     */
    ~TestAlarm();

    /**
     * Returns TimeServiceSchedule of this Alarm
     *
     * @return Alarm
     */

    const TimeServiceSchedule& getSchedule();

    /**
     * Set TimeServiceSchedule to this Alarm object
     *
     * @param schedule TimeServiceSchedule
     * @return Status ER_OK If succeeded to set the Schedule, otherwise, if this Alarm does not support
     * one of the Schedule values, return an appropriate failure status.
     */
    QStatus setSchedule(const TimeServiceSchedule& schedule);

    /**
     * Returns Alarm title.
     *
     * @return Optional textual description of what this Alarm is
     */
    const qcc::String& getTitle();

    /**
     * Set title to this Alarm.
     *
     * @param title Alarm title.
     * Title is an optional textual description of what this Alarm is set for.
     */
    void setTitle(const qcc::String& title);

    /**
     * Returns whether or not this Alarm is enabled
     *
     * @return Is Alarm enabled
     */
    const bool isEnabled();

    /**
     * Set whether or not this Alarm is enabled
     *
     * @param enabled Is Alarm enabled
     */
    void setEnabled(bool enabled);

  private:

    /**
     * Schedule of the Alarm
     */
    TimeServiceSchedule m_Schedule;

    /**
     * Title of the Alarm
     */
    qcc::String m_Title;

    /**
     * Alarm enabled
     */
    bool m_Enabled;


    /**
     * Creates String representation of the DateTime
     */
    void printSchedule() const;

    /**
     * Creates String representation of the week days
     */
    qcc::String getWeekdaysString() const;
};

} /* namespace services */
} /* namespace ajn */

#endif /* TestAlarm_H_ */
