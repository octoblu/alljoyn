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
import org.alljoyn.bus.annotation.BusInterface;
import org.alljoyn.bus.annotation.BusProperty;
import org.alljoyn.bus.annotation.BusSignal;
import org.allseen.timeservice.TimeServiceConst;

/**
 * Time Service Alarm interface
 */
@BusInterface(name = Alarm.IFNAME, descriptionLanguage = "en", description = "Alarm")
public interface Alarm extends BusObject {

    /**
     * The name of the AllJoyn interface
     */
    public static final String IFNAME = TimeServiceConst.IFNAME_PREFIX + ".Alarm";

    /**
     * The version of this interface
     */
    public static final short VERSION = 1;

    /**
     * Returns the interface version
     * 
     * @return Interface version number
     * @throws BusException
     */
    @BusProperty(signature = "q")
    short getVersion() throws BusException;

    /**
     * Returns Alarm schedule
     * 
     * @return {@link ScheduleAJ}
     * @throws BusException
     */
    @BusProperty(signature = "((yyyq)y)")
    ScheduleAJ getSchedule() throws BusException;

    /**
     * Set Alarm schedule
     * 
     * @param schedule
     * @throws BusException
     */
    @BusProperty(signature = "((yyyq)y)")
    void setSchedule(ScheduleAJ schedule) throws BusException;

    /**
     * Returns Alarm title
     * 
     * @return title
     * @throws BusException
     */
    @BusProperty(signature = "s")
    String getTitle() throws BusException;

    /**
     * Set Alarm title
     * 
     * @param title
     * @throws BusException
     */
    @BusProperty(signature = "s")
    void setTitle(String title) throws BusException;

    /**
     * Returns whether or not this Alarm is enabled
     * 
     * @return Is Alarm is enabled
     * @throws BusException
     */
    @BusProperty(signature = "b")
    boolean getEnabled() throws BusException;

    /**
     * Set whether or not this Alarm is enabled
     * 
     * @throws BusException
     */
    @BusProperty(signature = "b")
    void setEnabled(boolean enabled) throws BusException;

    /**
     * This signal is emitted when the Alarm has been reached
     * 
     * @throws BusException
     */
    @BusSignal(name = "AlarmReached", description = "Alarm reached signal", sessionless = true)
    void alarmReached() throws BusException;
}
