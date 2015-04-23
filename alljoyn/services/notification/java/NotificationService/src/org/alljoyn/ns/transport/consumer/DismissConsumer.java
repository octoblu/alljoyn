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

package org.alljoyn.ns.transport.consumer;

import java.util.UUID;

import org.alljoyn.bus.BusException;
import org.alljoyn.ns.NotificationServiceException;
import org.alljoyn.ns.PayloadAdapter;
import org.alljoyn.ns.commons.GenericLogger;
import org.alljoyn.ns.transport.Transport;
import org.alljoyn.ns.transport.interfaces.NotificationDismisser;

/**
 * The class provides a functionality of receiving AllJoyn Dismiss session-less-signals 
 */
class DismissConsumer implements NotificationDismisser {
	private static final String TAG = "ioe" + DismissConsumer.class.getSimpleName();
	
	public static final String OBJ_PATH = "/dismissReceiver";
	
	/**
	 * @see org.alljoyn.ns.transport.interfaces.NotificationDismisser#dismiss(int, byte[])
	 */
	@Override
	public void dismiss(int msgId, byte[] bAppId) throws BusException {
		
		Transport transport = Transport.getInstance();
		transport.getBusAttachment().enableConcurrentCallbacks();
		
		GenericLogger logger;
		
		try {
			logger = transport.getLogger();
		}
		catch(NotificationServiceException nse) {
			System.out.println(TAG + ": Unexpected error occured: " + nse.getMessage());
			return;
		}
		
		UUID appId = PayloadAdapter.byteArrayToUUID(bAppId);
		if ( appId == null ) {
			logger.error(TAG, "Received the Dismiss signal for the notifId: '" + msgId + "' with an invalid ApplicationId");
			return;
		}
		
		logger.debug(TAG, "Received a dismiss signal for notifId: '" + msgId + "', from appId: '" + appId + "', handling...");
		transport.onReceivedNotificationDismiss(msgId, appId);
	}//dismiss

	/**
	 * @see org.alljoyn.ns.transport.interfaces.NotificationDismisser#getVersion()
	 */
	@Override
	public short getVersion() throws BusException {
		return VERSION;
	}//getVersion

}
