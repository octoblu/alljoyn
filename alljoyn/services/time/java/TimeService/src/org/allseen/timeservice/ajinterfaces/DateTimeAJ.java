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
import org.allseen.timeservice.Date;
import org.allseen.timeservice.DateTime;
import org.allseen.timeservice.Time;

/**
 * Date Time structure for communicating with {@link Clock}
 */
public class DateTimeAJ {

    /**
     * Date structure
     */
    public static class DateAJ {

        @Position(0)
        public short year;

        @Position(1)
        public byte month;

        @Position(2)
        public byte day;

        /**
         * Default Constructor
         */
        public DateAJ(){
        }

        /**
         * Constructor
         * @param date
         */
        public DateAJ(Date date) {

            year   = date.getYear();
            month  = date.getMonth();
            day    = date.getDay();
        }

        /**
         * Converts {@link DateAJ} into {@link Date}
         * @return {@link Date}
         * @throws IllegalArgumentException
         */
        public Date toDate() {

            return new Date(year, month, day);
        }

        /**
         * @see java.lang.Object#toString()
         */
        @Override
        public String toString() {

            return String.format("{DateAJ %s-%s-%s}", day, month, year);
        }
    }

    /**
     * Time structure
     */
    public static class TimeAJ {

        //0-23
        @Position(0)
        public byte hour;

        //0-59
        @Position(1)
        public byte minute;

        //0-59
        @Position(2)
        public byte second;

        //0-999
        @Position(3)
        public short millisecond;

        /**
         * Default Constructor
         */
        public TimeAJ() {
        }

        /**
         * Constructor
         * @param time
         */
        public TimeAJ(Time time) {

            hour          = time.getHour();
            minute        = time.getMinute();
            second        = time.getSeconds();
            millisecond   = time.getMilliseconds();
        }

        /**
         * Converts {@link TimeAJ} into {@link Time}
         * @return {@link Time}
         * @throws IllegalArgumentException
         */
        public Time toTime() {

            return new Time(hour, minute, second, millisecond);
        }

        /**
         * @see java.lang.Object#toString()
         */
        @Override
        public String toString() {

            return String.format("{TimeAJ %s:%s:%s.%s}", hour, minute, second, millisecond);
        }
    }

    @Position(0)
    public DateAJ date;

    @Position(1)
    public TimeAJ time;

    @Position(2)
    public short offsetMinutes;

    /**
     * Default Constructor
     */
    public DateTimeAJ() {
    }

    /**
     * Constructor
     * @param dateTime
     */
    public DateTimeAJ(DateTime dateTime) {

        date          = new DateAJ(dateTime.getDate());
        time          = new TimeAJ(dateTime.getTime());
        offsetMinutes = dateTime.getOffsetMinutes();
    }

    /**
     * Converts from AllJoyn structure {@link DateTimeAJ} to {@link DateTime}
     * @return {@link DateTime}
     * @throws IllegalArgumentException Is thrown if received arguments are not in the correct range
     * @see Date
     * @see Time
     */
    public DateTime toDateTime()  {

        return new DateTime(date.toDate(), time.toTime(), offsetMinutes);
    }

    /**
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {

        return "{DateTimeAJ " + date + " " + time + " offset: '" + offsetMinutes + "'}";
    }
}
