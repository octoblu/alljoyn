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
import org.allseen.timeservice.Period;

/**
 * Period structure
 */
public class PeriodAJ {

    //Positive
    @Position(0)
    public int hour;

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
    public PeriodAJ() {
    }

    /**
     * Constructor
     * @param period
     */
    public PeriodAJ(Period period) {

        hour          = period.getHour();
        minute        = period.getMinute();
        second        = period.getSeconds();
        millisecond   = period.getMilliseconds();
    }

    /**
     * Converts {@link PeriodAJ} into {@link Period}
     * @return {@link Period}
     * @throws IllegalArgumentException
     */
    public Period toPeriod() {

        return new Period(hour, minute, second, millisecond);
    }

    /**
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {

        return String.format("{TimeAJ %s:%s:%s.%s}", hour, minute, second, millisecond);
    }
}
