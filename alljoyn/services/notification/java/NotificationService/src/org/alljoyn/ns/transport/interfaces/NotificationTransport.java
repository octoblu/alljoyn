/******************************************************************************
 * Copyright (c) 2013, AllSeen Alliance. All rights reserved.
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

import java.util.Map;

import org.alljoyn.bus.BusObject;
import org.alljoyn.bus.Variant;
import org.alljoyn.bus.annotation.BusInterface;
import org.alljoyn.bus.annotation.BusSignal;
import org.alljoyn.ns.transport.TransportNotificationText;

/** 
 * The interface used to send and receive "session less" notification signals  
 */
@BusInterface (name = NotificationTransport.IF_NAME)
public interface NotificationTransport extends BusObject {
	
	/**
	 * AllJoyn interface name
	 */
	public static final String IF_NAME = "org.alljoyn.Notification"; 

	/**
	 * The interface version
	 */
	public static final short VERSION  = 1;
	
	/**
	 * Use the method to send or receive AJ session less signal
	 * @param version The version of the message signature 
	 * @param msgId The notification message id
	 * @param messageType Notification message type id
	 * @param deviceId Device Id
	 * @param deviceName Device Name
	 * @param appId App Id 
	 * @param appName App Name
	 * @param attributes Attributes key-value pair
	 * @param customAttributes customAttributes
	 * @param text Array of NotificationText objects
	 */
	@BusSignal(signature = "qiqssaysa{iv}a{ss}ar")
	public void notify(int version, int msgId, short messageType, String deviceId, String deviceName, byte[] appId, String appName, Map<Integer, Variant> attributes, Map<String, String> customAttributes, TransportNotificationText[] text); 
}
