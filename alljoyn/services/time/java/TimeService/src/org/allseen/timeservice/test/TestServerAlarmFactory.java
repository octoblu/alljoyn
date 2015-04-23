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

import java.util.ArrayList;
import java.util.List;

import org.alljoyn.bus.ErrorReplyBusException;
import org.allseen.timeservice.TimeServiceConst;
import org.allseen.timeservice.server.Alarm;
import org.allseen.timeservice.server.AlarmFactory;

import android.util.Log;

/**
 * Tests Alarm Factory
 */
public class TestServerAlarmFactory extends AlarmFactory {
    private static final String TAG = "ajts" + TestServerAlarmFactory.class.getSimpleName();

    /**
     * Alarms created by this Alarm Factory
     */
    private final List<Alarm> alarms;

    /**
     * Constructor
     */
    public TestServerAlarmFactory() {

        alarms = new ArrayList<Alarm>();
    }

    /**
     * @see org.allseen.timeservice.server.AlarmFactory#release()
     */
    @Override
    public void release() {

        for ( Alarm alarm : alarms ) {

            alarm.release();
        }
        alarms.clear();

        super.release();
    }

    /**
     * @see org.allseen.timeservice.server.AlarmFactory#newAlarm()
     */
    @Override
    public Alarm newAlarm() throws ErrorReplyBusException {

        Alarm alarm = new TestServerAlarm();
        alarms.add(alarm);

        Log.i(TAG, "Created new Alarm by Factory");
        return alarm;
    }

    /**
     * @see org.allseen.timeservice.server.AlarmFactory#deleteAlarm(java.lang.String)
     */
    @Override
    public void deleteAlarm(String objectPath) throws ErrorReplyBusException {

        Alarm alarm = findAlarm(objectPath);

        if ( alarm != null ) {

            Log.i(TAG, "Removing Alarm, objectPath: '" + objectPath + "'");
            alarm.release();

            alarms.remove(alarm);

            return;
        }

        throw new ErrorReplyBusException(TimeServiceConst.GENERIC_ERROR, "Alarm: '" + objectPath + "' is not found");
    }

    /**
     * Looks for Alarm
     */
    public Alarm findAlarm(String objectPath) {

        for ( Alarm alarm : alarms ) {

            if ( alarm.getObjectPath().equals(objectPath) ) {

                return alarm;
            }
        }

        return null;
    }
}
