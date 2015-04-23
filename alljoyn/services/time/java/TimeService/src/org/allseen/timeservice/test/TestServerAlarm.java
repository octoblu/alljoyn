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

import org.alljoyn.bus.ErrorReplyBusException;
import org.allseen.timeservice.Schedule;
import org.allseen.timeservice.TimeServiceException;
import org.allseen.timeservice.server.Alarm;

import android.text.format.Time;

public class TestServerAlarm extends Alarm {

    /**
     * {@link Schedule}
     */
    private Schedule schedule;

    /**
     * Alarm title
     */
    private String title;

    /**
     * Is Alarm enabled
     */
    private boolean isEnabled;

    /**
     * Constructor
     */
    public TestServerAlarm() {
        super();

        Time t = new Time(Time.getCurrentTimezone());
        t.setToNow();

        org.allseen.timeservice.Time ts = new org.allseen.timeservice.Time((byte)0, (byte) 0, (byte)0, (short)0);
        schedule = new Schedule(ts);

        title = "";
    }

    @Override
    public Schedule getSchedule() {

        return schedule;
    }

    @Override
    public void setSchedule(Schedule schedule) throws ErrorReplyBusException {

        this.schedule = schedule;
    }

    @Override
    public String getTitle() {

        return title;
    }

    @Override
    public void setTitle(String title) {

        this.title = title;
    }

    @Override
    public boolean isEnabled() {

        return isEnabled;
    }

    @Override
    public void setEnabled(boolean enabled) {

        this.isEnabled = enabled;
    }

    @Override
    public void alarmReached() throws TimeServiceException {

        super.alarmReached();
    }

}
