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
package org.allseen.timeservice;

import org.allseen.timeservice.ajinterfaces.Timer;

/**
 * Period is used to indicate time interval of the {@link Timer}.
 */
public class Period {

    private final int HOUR;

    private final byte MINUTE;

    private final byte SECOND;

    private final short MILLISECOND;

    /**
     * Constructor
     * @param hour A positive number
     * @param minute Expected range: 0-59
     * @param second Expected range: 0-59
     * @param millisecond Expected range: 0-999
     * @throws IllegalArgumentException Is thrown if received arguments are not in the correct range
     */
    public Period(int hour, byte minute, byte second, short millisecond) {

        checkValidity(hour, minute, second, millisecond);

        HOUR         = hour;
        MINUTE       = minute;
        SECOND       = second;
        MILLISECOND  = millisecond;
    }

    public int getHour() {
        return HOUR;
    }

    public byte getMinute() {
        return MINUTE;
    }

    public byte getSeconds() {
        return SECOND ;
    }

    public short getMilliseconds() {
        return MILLISECOND;
    }

    /**
     * Check arguments validity.
     * @throws IllegalArgumentException Is thrown if received arguments are not in the correct range
     */
    private void checkValidity(int hour, byte minute, byte second, short millisecond) {

        if ( hour < 0 ) {

            throw new IllegalArgumentException("Hour is not a positive number");
        }

        if ( minute < 0 || minute > 59 ) {

            throw new IllegalArgumentException("Minute is not in the correct range");
        }

        if ( second < 0 || second > 59 ) {

            throw new IllegalArgumentException("Second is not in the correct range");
        }

        if ( millisecond < 0 || millisecond > 999 ) {

            throw new IllegalArgumentException("Millisecond is not in the correct range");
        }
    }

    /**
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {

        return String.format("{Period %s:%s:%s.%s}", HOUR, MINUTE, SECOND, MILLISECOND);
    }
}
