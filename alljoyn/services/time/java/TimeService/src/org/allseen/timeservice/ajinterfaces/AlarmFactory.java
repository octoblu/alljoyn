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
import org.alljoyn.bus.annotation.BusMethod;
import org.alljoyn.bus.annotation.BusProperty;
import org.alljoyn.bus.annotation.Secure;
import org.allseen.timeservice.TimeServiceConst;

/**
 * Time Service Alarm Factory interface
 */
@BusInterface(name = AlarmFactory.IFNAME, announced = "true")
@Secure
public interface AlarmFactory extends BusObject {

    /**
     * The name of the AllJoyn interface
     */
    public static final String IFNAME = TimeServiceConst.IFNAME_PREFIX + ".AlarmFactory";

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
     * Creates an {@link Alarm} under this {@link AlarmFactory}. The newly
     * created {@link Alarm} implements {@link Alarm#IFNAME}.
     * 
     * @return Returns the object path of the newly created {@link Alarm}.
     * @throws BusException
     */
    @BusMethod(name = "NewAlarm", replySignature = "o")
    String newAlarm() throws BusException;

    /**
     * Removes {@link Alarm} created by this {@link AlarmFactory}.
     * 
     * @param objectPath
     *            The object path of the {@link Alarm} to remove
     * @throws BusException
     */
    @BusMethod(name = "DeleteAlarm", signature = "o")
    void deleteAlarm(String objectPath) throws BusException;
}
