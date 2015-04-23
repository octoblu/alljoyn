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

package org.alljoyn.ns.transport.producer;

import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.Mutable.ShortValue;
import org.alljoyn.bus.SessionOpts;
import org.alljoyn.bus.SessionPortListener;
import org.alljoyn.bus.Status;
import org.alljoyn.ns.NotificationServiceException;
import org.alljoyn.ns.commons.GenericLogger;
import org.alljoyn.ns.commons.NativePlatform;
import org.alljoyn.ns.transport.Transport;


/**
 *  Prepares the Notification sender to receive incoming connections <br>
 *  	- Bind session port <br>
 *  	- Register session port listener <br>
 *  	- Handle incoming connections
 */
public class SenderSessionListener extends SessionPortListener {
	private static final String TAG = "ioe" + SenderSessionListener.class.getSimpleName();
	
	/**
	 * The reference to the platform dependent object
	 */
	private NativePlatform nativePlatform;
	
	/**
	 * The port number to listen for incoming connections
	 */
	public static final short PORT_NUM = 1010;
	
	/**
	 * Constructor
	 * @param nativePlatform The reference to the platform dependent object
	 */
	public SenderSessionListener(NativePlatform nativePlatform) {
		super();
		this.nativePlatform = nativePlatform;
	}

	/**
	 * Initializes the object <br>
	 *   - Bind session port <br>
  	 *   - Register session port listener <br>
	 * @throws NotificationServiceException
	 */
	public void init() throws NotificationServiceException {
		
		BusAttachment bus = Transport.getInstance().getBusAttachment();
		Status status     = bus.bindSessionPort(new ShortValue(PORT_NUM), getSessionOpts(), this);
		
		nativePlatform.getNativeLogger().debug(TAG, "Session port: '" + PORT_NUM + "' was bound on the bus: '" + status + "'");
		
		if ( status != Status.OK ) {
			throw new NotificationServiceException("Failed to bind session port: '" + PORT_NUM + "', Error: '" + status + "'");
		}
	}//init
	
	/**
	 * Cleans the object resources
	 */
	public void clean() {
		
		GenericLogger logger = nativePlatform.getNativeLogger();
				
		logger.debug(TAG, "Cleaning the SenderSessionListener");
		BusAttachment bus = Transport.getInstance().getBusAttachment();
		Status status     = bus.unbindSessionPort(PORT_NUM);
		
		if ( status != Status.OK ) {
			nativePlatform.getNativeLogger().error(TAG, "Failed to unbind the port number: '" + PORT_NUM + "', Error: '" + status + "'");
		}
		
		nativePlatform = null;
	}//clean
	
	/**
	 * @see org.alljoyn.bus.SessionPortListener#acceptSessionJoiner(short, java.lang.String, org.alljoyn.bus.SessionOpts)
	 */
	@Override
	public boolean acceptSessionJoiner(short sessionPort, String joiner, SessionOpts opts) {
		
		if ( nativePlatform == null ) { // Protection - this may happens if the clean has been called, but daemon had some events in its stack
			return false;
		}
		
		GenericLogger logger = nativePlatform.getNativeLogger();
		logger.debug(TAG, "Received SessionJoiner request from: '" + joiner + "', requested port: '" + sessionPort + "'");
		
		if ( sessionPort == PORT_NUM ) {
			return true;
		}
		
		return false;
	}//acceptSessionJoiner

	/**
	 * @see org.alljoyn.bus.SessionPortListener#sessionJoined(short, int, java.lang.String)
	 */
	@Override
	public void sessionJoined(short sessionPort, int id, String joiner) {
		
		if ( nativePlatform == null ) {
			return;
		}
		
		GenericLogger logger = nativePlatform.getNativeLogger();
		logger.debug(TAG, "The session was established with: '" + joiner + "', SID: '" + id +  "'");
	}//sessionJoined

	/**
	 * @return {@link SessionOpts}
	 */
	public static SessionOpts getSessionOpts() {
		SessionOpts sessionOpts   = new SessionOpts();
	    sessionOpts.traffic       = SessionOpts.TRAFFIC_MESSAGES;   // Use reliable message-based communication to move data between session endpoints
	    sessionOpts.isMultipoint  = false;                          // A session is multi-point if it can be joined multiple times 
	    sessionOpts.proximity     = SessionOpts.PROXIMITY_ANY;      // Holds the proximity for this SessionOpt
	    sessionOpts.transports    = SessionOpts.TRANSPORT_ANY;      // Holds the allowed transports for this SessionOpt
	    return sessionOpts;
	}//getSessionOpts
}
