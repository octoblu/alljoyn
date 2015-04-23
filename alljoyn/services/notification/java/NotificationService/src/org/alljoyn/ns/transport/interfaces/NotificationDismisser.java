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
import org.alljoyn.bus.annotation.BusProperty;
import org.alljoyn.bus.annotation.BusSignal;

/**
 * The interface provides the functionality to send and receive Dismiss session-less-signals
 */
@BusInterface(name = NotificationDismisser.IF_NAME)
public interface NotificationDismisser extends BusObject {
	
	/**
	 * The AllJoyn interface name
	 */
	public static final String IF_NAME   = "org.alljoyn.Notification.Dismisser";
	
	/**
	 * The interface version
	 */
	public static final short  VERSION   = 1;
	
	/**
	 * The Dismiss signal to be sent 
	 * @param msgId The notification id the Dismiss signal belongs
	 * @param appId The application id of the Notification Sender
	 */
	@BusSignal(signature="iay", name="Dismiss")
	public void dismiss(int msgId, byte[] appId) throws BusException;
	
	
	/**
	 * @return The interface version
	 * @throws BusException
	 */
	@BusProperty(signature="q")
	public short getVersion() throws BusException;
}
