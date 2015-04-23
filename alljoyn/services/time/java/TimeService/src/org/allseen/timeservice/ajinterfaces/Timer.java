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

import org.alljoyn.bus.BusException;
import org.alljoyn.bus.BusObject;
import org.alljoyn.bus.annotation.BusAnnotation;
import org.alljoyn.bus.annotation.BusAnnotations;
import org.alljoyn.bus.annotation.BusInterface;
import org.alljoyn.bus.annotation.BusMethod;
import org.alljoyn.bus.annotation.BusProperty;
import org.alljoyn.bus.annotation.BusSignal;
import org.allseen.timeservice.TimeServiceConst;

/**
 * Time Service Timer interface
 */
@BusInterface(name=Timer.IFNAME, descriptionLanguage="en", description="Timer")
public interface Timer extends BusObject {

    /**
     * The name of the AllJoyn interface
     */
    public static final String IFNAME        = TimeServiceConst.IFNAME_PREFIX + ".Timer";

    /**
     * The version of this interface
     */
    public static final short VERSION        = 1;

    /**
     * If timer Repeat is set to this value, the timer will repeat indefinitely
     */
    public static final short REPEAT_FOREVER = (short)0xffff;

    /**
     * Returns the interface version
     * @return Interface version number
     * @throws BusException
     */
    @BusProperty(signature="q")
    short getVersion() throws BusException;

    /**
     * Returns total time to count down
     * @return Total time to count down
     * @throws BusException
     */
    @BusProperty(signature="(uyyq)")
    PeriodAJ getInterval() throws BusException;

    /**
     * Set total time to count down
     * @throws BusException
     */
    @BusProperty(signature="(uyyq)")
    void setInterval(PeriodAJ periodAJ) throws BusException;

    /**
     * Amount of time left
     * @return Amount of time left
     * @throws BusException
     */
    @BusProperty(signature="(uyyq)")
    PeriodAJ getTimeLeft() throws BusException;

    /**
     * True if the timer is currently running
     * @return True if the timer is currently running
     * @throws BusException
     */
    @BusProperty(signature="b")
    boolean getIsRunning() throws BusException;

    /**
     * How many times this timer should repeat itself.
     * The value of {@link org.allseen.timeservice.ajinterfaces.Timer#REPEAT_FOREVER}
     * means to repeat the timer forever
     * @return How many times this timer should repeat itself.
     * @throws BusException
     */
    @BusProperty(signature="q")
    short getRepeat() throws BusException;

    /**
     * How many times this timer should repeat itself.
     * The value of {@link org.allseen.timeservice.ajinterfaces.Timer#REPEAT_FOREVER}
     * means to repeat the timer forever
     * @throws BusException
     */
    @BusProperty(signature="q")
    void setRepeat(short repeatTimes) throws BusException;

    /**
     * Returns textual description of this Timer
     * @return Textual description of this Timer
     * @throws BusException
     */
    @BusProperty(signature="s")
    String getTitle() throws BusException;

    /**
     * Set textual description of this Timer
     * @param title Textual description of this Timer
     * @throws BusException
     */
    @BusProperty(signature="s")
    void setTitle(String title) throws BusException;

    /**
     * Start the timer
     * @throws BusException
     */
    @BusMethod(name="Start")
    @BusAnnotations({@BusAnnotation(name="org.freedesktop.DBus.Method.NoReply", value="true")})
    void start() throws BusException;

    /**
     * Pause the timer
     * @throws BusException
     */
    @BusMethod(name="Pause")
    @BusAnnotations({@BusAnnotation(name="org.freedesktop.DBus.Method.NoReply", value="true")})
    void pause() throws BusException;

    /**
     * Reset the timer so that TimeLeft is equal to Interval
     * @throws BusException
     */
    @BusMethod(name="Reset")
    void reset() throws BusException;

    /**
     * The signal is sent when the timer fires
     * @throws BusException
     */
    @BusSignal(name="TimerEvent", description="Timer reached signal", sessionless=true)
    void timerEvent() throws BusException;

    /**
     * The signal is sent after {@link Timer#start()} or {@link Timer#pause()} is called
     * @param isRunning TRUE if the timer is running, otherwise FALSE
     * @throws BusException
     */
    @BusSignal(name="RunStateChanged")
    void runStateChanged(boolean isRunning) throws BusException;
}
