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

package org.alljoyn.ns.transport.interfaces;


import org.alljoyn.bus.BusException;
import org.alljoyn.bus.BusObject;
import org.alljoyn.bus.annotation.BusInterface;
import org.alljoyn.bus.annotation.BusMethod;
import org.alljoyn.bus.annotation.BusProperty;
import org.alljoyn.ns.Notification;

/**
 * The interface provides to a notification receiver the functionality to dismiss 
 * the received {@link Notification}.  
 */
@BusInterface(name = NotificationProducer.IFNAME)
public interface NotificationProducer extends BusObject {
	
	/**
	 * The AllJoyn interface name
	 */
	public static final String IFNAME = "org.alljoyn.Notification.Producer";
	
	/**
	 * Notification producer object
	 */
	public static final String OBJ_PATH = "/notificationProducer";
	
	/**
	 * The interface version
	 */
	public static final short VERSION = 1;
		
	/**
	 * When the notification message is dismissed, it's first of all deleted and then a dismiss signal is sent
	 * to all notification consumers to update them that the {@link Notification} has been dismissed   
	 * @param msgId The notification message identifier
	 * @throws BusException
	 */
	@BusMethod(name="Dismiss", signature="i")
	public void dismiss(int msgId) throws BusException;
	
	/**
	 * @return The interface version
	 * @throws BusException
	 */
	@BusProperty(signature="q")
	public short getVersion() throws BusException;
	
}
