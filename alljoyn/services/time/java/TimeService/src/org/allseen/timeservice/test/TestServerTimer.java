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

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

import org.alljoyn.bus.ErrorReplyBusException;
import org.allseen.timeservice.Period;
import org.allseen.timeservice.TimeServiceException;
import org.allseen.timeservice.server.Timer;

import android.util.Log;

public class TestServerTimer extends Timer {
    private static final String TAG = "ajts" + TestServerTimer.class.getSimpleName();

    /**
     * The task that is executed when the Timer fires
     */
    private class TimerTask implements Runnable {

        @Override
        public void run() {

            fireTimerEvent();

            Log.d(TAG, "Stopping Timer: '" + getObjectPath() + "'");
            pause();
        }

        /**
         * Call to send {@link TestServerTimer#timerEvent()}
         */
        protected void fireTimerEvent() {

            Log.i(TAG, "Timer: '" + getObjectPath() + "' has been fired");

            try {

                timerEvent();
            } catch (TimeServiceException tse) {

                Log.e(TAG, "Failed to emit TimerEvent", tse);
            }
        }
    }

    //===============================================//

    /**
     * The task that is executed when the Timer fires repeatedly
     */
    private class RepeatedTimerTask extends TimerTask {

        @Override
        public void run() {

            fireTimerEvent();

            if ( toRepeat() ) {

                startedAtMilliSeconds = System.currentTimeMillis();
            }
            else {

                Log.d(TAG, "Stopping repeated Timer: '" + getObjectPath() + "'");
                pause();
            }
        }
    }

    //===============================================//

    /**
     * Timer title
     */
    private String title;

    /**
     * Is timer running
     */
    private volatile boolean isRunning;

    /**
     * Timer period
     */
    private Period interval;

    /**
     * Timer interval in seconds
     */
    private long intervalSeconds;

    /**
     * System time when the Timer was started, to calculated TimeLeft
     */
    private long startedAtMilliSeconds;

    /**
     * Times for timer to repeat
     */
    private short repeat;

    /**
     * Calculate times to repeat until repeat counter doesn't expire
     */
    private short timesToRepeat;

    /**
     * Timer service
     */
    private ScheduledExecutorService timerService;

    @Override
    public Period getInterval() {

        if ( interval == null ) {

            return new Period(0, (byte)0, (byte)0, (short)0);
        }

        return interval;
    }

    @Override
    public void setInterval(Period period) {

        Log.d(TAG, "Timer: '" + getObjectPath() + "', setting interval: '" + period + "'");

        reset();
        this.interval        = period;
        this.intervalSeconds = intervalToSeconds(period);
    }

    @Override
    public Period getTimeLeft() {

        if ( !isRunning ) {

            return toPeriod(intervalSeconds);
        }

        long timeSinceStarted = getTimeSinceStarted();
        long timeLeft         = intervalSeconds - timeSinceStarted;

        if ( timeLeft < 0) {

            timeLeft = 0;
        }

        return toPeriod(timeLeft);
    }

    @Override
    public boolean isRunning() {

        return isRunning;
    }

    @Override
    public short getRepeat() {

        return repeat;
    }

    @Override
    public void setRepeat(short repeat) throws ErrorReplyBusException {

        if ( isRunning )  {

            reset();
        }

        this.repeat        = repeat;
        this.timesToRepeat = repeat;
    }

    @Override
    public String getTitle() {

        if ( title == null ) {

            return "";
        }

        return title;
    }

    @Override
    public void setTitle(String title) {

        this.title = title;
    }

    @Override
    public void start() {

        if ( isRunning ) {

            Log.w(TAG, "The timer is already running");
            return;
        }

        if ( interval == null ) {

            Log.w(TAG, "Timer interval wasn't set");
            return;
        }

        Log.i(TAG, "Starting the Timer: '" + getObjectPath() + "'");

        isRunning = true;
        startedAtMilliSeconds = System.currentTimeMillis();

        try {

            startTimerService();
        }
        catch (Exception e) {

            Log.e(TAG, "Failed to start Timer service", e);
            isRunning = false;

            return;
        }

        try {

            runStateChanged(isRunning);
        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to emit RunStateChanged");
            startedAtMilliSeconds = 0;
        }
    }

    @Override
    public void pause() {

        if ( !isRunning ) {

            Log.w(TAG, "The timer is already paused");
            return;
        }

        Log.i(TAG, "Pausing the Timer: '" + getObjectPath() + "'");

        isRunning = false;

        if ( timerService != null ) {

            timerService.shutdownNow();
        }

        try {
            runStateChanged(isRunning);
        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to call RunStateChanged", tse);
        }

        long timeSinceStarted = getTimeSinceStarted();

        if ( intervalSeconds > timeSinceStarted ) {

            intervalSeconds -= timeSinceStarted;
        }
        else {

            intervalSeconds = 0;
        }

        startedAtMilliSeconds = 0;
    }

    @Override
    public void reset() {

        Log.d(TAG, "Resetting the Timer: '" + getObjectPath() + "'");

        if ( isRunning ) {

            Log.d(TAG, "The Timer: '" + getObjectPath() + "' is currently running, pausing");
            pause();
        }

        intervalSeconds       = intervalToSeconds(interval);
        startedAtMilliSeconds = 0;
        repeat                = 0;
        timesToRepeat         = 0;
    }

    @Override
    public void release() {

        Log.d(TAG, "Releasing Test Timer");

        reset();
        interval        = null;
        intervalSeconds = 0;

        super.release();
    }

    /**
     * Calculates time left in seconds since startedAtMilliseconds
     * @return
     */
    private long getTimeSinceStarted() {

        if ( startedAtMilliSeconds == 0 ) {

            return 0;
        }

        return ( System.currentTimeMillis() - startedAtMilliSeconds ) / 1000;
    }

    /**
     * Calculates the given time in seconds into period of time
     * @param time in seconds
     * @return
     */
    private Period toPeriod(long time) {

        int hours    = (int) time / 3600;
        byte minutes = (byte) ((time % 3600) / 60);
        byte seconds = (byte) (time % 60);

        return new Period(hours, minutes, seconds, (short)0);
    }

    /**
     * Calculates {@link Period} into seconds
     * @return seconds
     */
    private long intervalToSeconds(Period period) {

        if ( period == null ) {

            return 0;
        }

        return period.getHour() * 3600 + period.getMinute() * 60 + period.getSeconds();
    }

    /**
     * Calculates whether the Timer should repeat <br>
     * - If timesToRepeat = 0 => false <br>
     * - If timesToRepeat == {@link org.allseen.timeservice.ajinterfaces.Timer#REPEAT_FOREVER} => true <br>
     * - (--timesToRepeat)
     * @return Whether the Timer should repeat
     */
    private boolean toRepeat() {

        if ( timesToRepeat == 0 ) {

            return false;
        }

        if ( timesToRepeat == org.allseen.timeservice.ajinterfaces.Timer.REPEAT_FOREVER ) {

            return true;
        }

        --timesToRepeat;
        return true;
    }

    /**
     * Starts Timer {@link ExecutorService}
     * @throws Exception
     */
    private void startTimerService() throws Exception {

        if ( intervalSeconds == 0 ) {

            throw new Exception("Interval period is 0, can't start Timer Service");
        }

        timerService = Executors.newSingleThreadScheduledExecutor();

        if ( toRepeat() ) {

            Log.d(TAG, "Starting repeated Timer: '" + getObjectPath() + "'");

            timerService.scheduleAtFixedRate(new RepeatedTimerTask(), intervalSeconds, intervalSeconds,
                                                TimeUnit.SECONDS);
        }
        else {

            Log.d(TAG, "Starting Timer: '" + getObjectPath() + "'");

            timerService.schedule(new TimerTask(), intervalSeconds, TimeUnit.SECONDS);
        }
    }
}
