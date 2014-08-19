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

package org.alljoyn.ns.transport.producer;

import java.util.EnumMap;
import java.util.Map;

import org.alljoyn.bus.Variant;
import org.alljoyn.ns.Notification;
import org.alljoyn.ns.NotificationMessageType;
import org.alljoyn.ns.transport.TransportNotificationText;
import org.alljoyn.ns.transport.interfaces.NotificationTransport;

/** 
 * The class is used to send {@link Notification} session less signals
 */
class NotificationTransportProducer implements NotificationTransport {

	/**
	 * Stores AJ BusObject identifiers
	 * Maps from Message type to ServicePath string
	 */
	private static Map<NotificationMessageType, String> servicePath;


	static {
		servicePath = new EnumMap<NotificationMessageType, String>(NotificationMessageType.class);
		servicePath.put(NotificationMessageType.EMERGENCY, "/emergency");
		servicePath.put(NotificationMessageType.WARNING, "/warning");
		servicePath.put(NotificationMessageType.INFO, "/info");
	}
	
	/**
	 * Returns the servicePath for each of the transport objects
	 * @return
	 */
	public static Map<NotificationMessageType, String> getServicePath() {
		return servicePath;
	}
	
	/**
	 * Use this method to send session less signals
	 * @see org.alljoyn.ns.transport.interfaces.NotificationTransport#notify(int, int, short, String, String, byte[], String, Map, Map, TransportNotificationText[])
	 */
	@Override
	public void notify(int version, int msgId,
			short messageType, String deviceId, String deviceName, byte[] appId, String appName,
			Map<Integer, Variant> attributes,
			Map<String, String> customAttributes,
			TransportNotificationText[] text) {}
}
