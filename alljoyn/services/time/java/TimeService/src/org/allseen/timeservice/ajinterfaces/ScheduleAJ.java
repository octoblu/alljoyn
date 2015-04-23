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

package org.allseen.timeservice.ajinterfaces;

import org.alljoyn.bus.annotation.Position;
import org.allseen.timeservice.Schedule;
import org.allseen.timeservice.Schedule.WeekDay;
import org.allseen.timeservice.ajinterfaces.DateTimeAJ.TimeAJ;

/**
 * Schedule structure for communicating with {@link Alarm}
 */
public class ScheduleAJ {

    /**
     * Time
     */
    @Position(0)
    public TimeAJ time;

    /**
     * Weekdays bitmap
     */
    @Position(1)
    public byte weekdays;

    /**
     * Constructor
     */
    public ScheduleAJ() {
    }

    /**
     * Constructor
     * @param schedule
     */
    public ScheduleAJ(Schedule schedule) {

         time       = new TimeAJ(schedule.getTime());
         weekdays   = WeekDay.getBitMap( schedule.getWeekDays() );
    }

    /**
     * Creates {@link Schedule} from {@link ScheduleAJ}
     * @return {@link ScheduleAJ}
     * @throws IllegalArgumentException Is thrown if failed to create the {@link Schedule}
     * @see Schedule
     */
    public Schedule toSchedule() {

        return new Schedule(time.toTime(), WeekDay.getWeekDays(weekdays));
    }

    /**
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {

        return String.format("{ScheduleAJ Time: '%s' WeekDays: '%s'}", time, weekdays);
    }
}
