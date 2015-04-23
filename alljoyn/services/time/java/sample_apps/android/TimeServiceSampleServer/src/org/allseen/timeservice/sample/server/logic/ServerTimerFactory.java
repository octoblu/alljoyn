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

package org.allseen.timeservice.sample.server.logic;

import java.util.ArrayList;
import java.util.List;

import org.alljoyn.bus.ErrorReplyBusException;
import org.allseen.timeservice.TimeServiceConst;
import org.allseen.timeservice.sample.server.ui.TimeSampleServer;
import org.allseen.timeservice.server.Timer;

import android.content.Context;
import android.util.Log;

/**
 * Represents a TimerFactory {@link org.allseen.timeservice.server.TimerFactory}.
 * Encapsulates access to list of timers.
 * <ul>
 * <li>Enables creation of new Timer {@link org.allseen.timeservice.server.Timer}.
 * <li>Enables delete of a Timer {@link org.allseen.timeservice.server.Timer}.
 * <li>Enables search for a Timer {@link org.allseen.timeservice.server.Timer}.
 * </ul>
 */
public class ServerTimerFactory extends org.allseen.timeservice.server.TimerFactory {
    private static final String TAG = "ServerTimerFactory";

    /**
     * Log Tag
     */
    private final Context context;

    /**
     * Timers created by this Timer Factory
     */
    private final List<Timer> timers;

    /**
     * Constructor
     */
    public ServerTimerFactory(Context context) {
        this.context = context;
        timers = new ArrayList<Timer>();
        TimeSampleServer.sendMessage(context, TimeSampleServer.TIMER_EVENT_ACTION, getObjectPath(), "ServerTimerFactory created");
    }

    /**
     * @see org.allseen.timeservice.server.AlarmFactory#release()
     */
    @Override
    public void release() {
        for (Timer timer : timers) {
            timer.release();
        }
        timers.clear();
        super.release();
        TimeSampleServer.sendMessage(context, TimeSampleServer.TIMER_EVENT_ACTION, getObjectPath(), "ServerTimerFactory released");
    }

    /**
     * @see org.allseen.timeservice.server.TimerFactory#newTimer()
     */
    @Override
    public Timer newTimer() throws ErrorReplyBusException {
        Timer timer = new ServerTimer(context);
        timers.add(timer);
        Log.i(TAG, "Created new Timer by Factory");
        TimeSampleServer.sendMessage(context, TimeSampleServer.TIMER_EVENT_ACTION, getObjectPath(), "ServerTimerFactory newTimer request");
        return timer;
    }

    /**
     * @see org.allseen.timeservice.server.TimerFactory#deleteTimer(java.lang.String)
     */
    @Override
    public void deleteTimer(String objectPath) throws ErrorReplyBusException {

        TimeSampleServer.sendMessage(context, TimeSampleServer.TIMER_EVENT_ACTION, getObjectPath(), "ServerTimerFactory deleteTimer request");
        Timer timer = findTimer(objectPath);
        if (timer != null) {
            Log.i(TAG, "Removing Timer, objectPath: '" + objectPath + "'");
            timer.release();
            timers.remove(timer);
            return;
        }
        throw new ErrorReplyBusException(TimeServiceConst.GENERIC_ERROR, "Timer: '" + objectPath + "' is not found");
    }

    /**
     * Search for a Timer in the Timer list
     *
     * @param Object
     *            path of the Timer to be searched
     * @return The found Timer or NULL if the Timer wasn't found
     */
    public Timer findTimer(String objectPath) {
        for (Timer timer : timers) {
            if (timer.getObjectPath().equals(objectPath)) {
                return timer;
            }
        }
        return null;
    }
}
