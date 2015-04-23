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

package org.allseen.timeservice.test;

import org.allseen.timeservice.Date;
import org.allseen.timeservice.DateTime;
import org.allseen.timeservice.server.Clock;

import android.text.format.Time;

/**
 * Test Server Clock
 */
public class TestServerClock extends Clock {

    private DateTime dt;
    private boolean isSet;

    /**
     * Constructor
     */
    public TestServerClock() {

        super();

        Time t = new Time(Time.getCurrentTimezone());
        t.setToNow();

        byte seconds = (byte) ((t.second > 59) ? 59 :  t.second);

        org.allseen.timeservice.Time ts = new org.allseen.timeservice.Time((byte)t.hour, (byte) t.minute,
                                                seconds, (short)0);

        org.allseen.timeservice.Date ds = new Date((short)t.year, (byte)(t.month + 1), (byte)t.monthDay);

        dt = new DateTime(ds, ts, (short)120);
    }

    @Override
    public DateTime getDateTime() {

        return dt;
    }

    @Override
    public void setDateTime(DateTime dateTime) {

        this.dt = dateTime;
        isSet   = true;
    }

    @Override
    public boolean getIsSet() {

        return isSet;
    }

}
