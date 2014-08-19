/******************************************************************************
 * Copyright (c) 2014, AllSeen Alliance. All rights reserved.
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
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.BusException;
import org.alljoyn.bus.ErrorReplyBusException;
import org.alljoyn.bus.Mutable;
import org.alljoyn.bus.OnJoinSessionListener;
import org.alljoyn.bus.ProxyBusObject;
import org.alljoyn.bus.SessionListener;
import org.alljoyn.bus.Status;
import org.alljoyn.ns.Notification;
import org.alljoyn.ns.NotificationServiceException;
import org.alljoyn.ns.commons.GenericLogger;
import org.alljoyn.ns.transport.DismissEmitter;
import org.alljoyn.ns.transport.Transport;
import org.alljoyn.ns.transport.interfaces.NotificationProducer;
import org.alljoyn.ns.transport.producer.SenderSessionListener;

/**
 * The class implements the functionality of sending feedback about the received {@link Notification}. <br>
 * Feedback types: <br>
 * 		1) dismiss     - Notification Producer deletes the {@link Notification} message and then sends 
 * 						 a Dismiss session-less-signal to update Notification Consumers that 
 * 						 this {@link Notification} message has been dismissed. <br>
 * 
 * If there is a failure in reaching the Notification Producer to dismiss the {@link Notification}, the dismiss 
 * session-less-signal is sent by the {@link NotificationFeedback}.
 */
public class NotificationFeedback extends OnJoinSessionListener {
	private static final String TAG = "ioe" + NotificationFeedback.class.getSimpleName();
	
	private class NFSessionListener extends SessionListener {
		
		/**
		 * @see org.alljoyn.bus.SessionListener#sessionLost(int, int)
		 */
		@Override
		public void sessionLost(int sessionId, int reason) {
			if ( logger != null ) {
				logger.debug(TAG, "Received session lost for sid: '" + sessionId + "', reason: '" + reason + "'");
			}
		}//sessionLest
	}
	
	//============================================//

	/**
	 * Handles sequentially the ack | dismiss tasks
	 */
	private static ExecutorService taskDispatcher = Executors.newSingleThreadExecutor();
	
	/**
	 * {@link Transport} object
	 */
	private Transport transport;
	
	/**
	 * The logger
	 */
	private GenericLogger logger;

	/**
	 * Session target
	 */
	private final String origSender;
	
	/**
	 * The notification id
	 */
	private final int notifId;
	
	/**
	 * The application 
	 */
	private final UUID appId;
	
	/**
	 * Notification version
	 */
	private final int version;
	
	/**
	 * Constructor
	 * @param notification The {@link Notification} to send feedback
	 * @throws NotificationServiceException If failed to create the {@link NotificationFeedback}
	 */
	public NotificationFeedback(Notification notification) throws NotificationServiceException {
		super();
		transport  = Transport.getInstance();
		logger     = transport.getLogger();
		
		version    = notification.getVersion();
		origSender = notification.getOriginalSenderBusName();
		notifId    = notification.getMessageId();
		appId      = notification.getAppId();
	}
	
	/**
	 * Call the dismiss
	 */
	public void dismiss() {
		
		taskDispatcher.execute( new Runnable() {
			@Override
			public void run() {
				
				//Version 1 doesn't support the NotificationProducer interface and the original sender 
				if ( version < 2 || origSender == null ) {
					logger.debug(TAG, "The notification sender version: '" + version + "', doesn't support the NotificationProducer interface, notifId: '" + notifId + "', can't call the Dismiss method, sending the Dismiss signal");
					DismissEmitter.send(notifId, appId);
					return;
				}

				invokeDismiss();
			}//run
		});
	}//dismiss
	
	/**
	 * Calls the remote dismiss method. <br>
	 * If fails, send the Dismiss signal. The signal should be sent by the consumer if the producer is not reachable.
	 * @param status Session establishment status
	 */
	private void invokeDismiss() {

	    BusAttachment bus = transport.getBusAttachment();
		if ( bus == null ) {
			logger.error(TAG, "Failed to call Dismiss for notifId: '" + notifId + "', BusAttachment is not defined, returning...");
			return;
		}
		
		Mutable.IntegerValue sid = new Mutable.IntegerValue();
		Status status            = establishSession(bus, sid);
		
		//The status ALLJOYN_JOINSESSION_REPLY_ALREADY_JOINED here is returned if the 
		//BusAttachment is trying to establish a session with itself (producer and consumer are sharing a BusAttachment)
		if ( status != Status.OK && status != Status.ALLJOYN_JOINSESSION_REPLY_ALREADY_JOINED ) {
			logger.error(TAG, "Failed to call Dismiss method for notifId: '" + notifId + "', session is not established, Error: '" + status + "', Sending a Dismiss signal");
			DismissEmitter.send(notifId, appId);
			return;
		}
		
		logger.debug(TAG, "Handling Dismiss method call for notifId: '" + notifId + "', session: '" + sid.value + "', SessionJoin status: '" + status + "'");
		NotificationProducer notifProducer = getProxyObject(bus, sid.value);
					
		try {
			notifProducer.dismiss(notifId);
		}
		catch (ErrorReplyBusException erbe) {
			logger.error(TAG, "Failed to call Dismiss for notifId: '" + notifId + "', ErrorName: '" + erbe.getErrorName() + "', ErrorMessage: '" + erbe.getErrorMessage() + "', sending Dismiss signal");
			DismissEmitter.send(notifId, appId);
		}
		catch (BusException be) {
			logger.error(TAG, "Failed to call Dismiss method for notifId: '" + notifId + "', Error: '" + be.getMessage() + "', Sending Dismiss signal");
			DismissEmitter.send(notifId, appId);
		}
		finally {
			if ( status != Status.ALLJOYN_JOINSESSION_REPLY_ALREADY_JOINED ) {
				leaveSession(bus, sid.value);
			}
		}
		
	}//invokeDismiss
	
	/**
	 * Calls to establish a session with a NotificationProducer
	 * @param bus {@link BusAttachment}
	 * @param sessionId The sessionId that has been created following the request
	 * @return Status The session establish status
	 */
	private Status establishSession(BusAttachment bus, Mutable.IntegerValue sessionId) {
		return bus.joinSession(origSender,
							   SenderSessionListener.PORT_NUM,
							   sessionId, 
							   SenderSessionListener.getSessionOpts(), 
							   new NFSessionListener());
	}//establishSession

	
	/**
	 * Leave the session if the sessionId isn't NULL and not zero <br>
	 * @param bus {@link BusAttachment}
	 * @param sid 
	 */
	private void leaveSession(BusAttachment bus, int sid) {
		Status status = bus.leaveSession(sid);
		
		if ( status == Status.OK ) {
			logger.debug(TAG, "The session: '" + sid + "' was disconnected successfully");
		}
		else {
			logger.error(TAG, "Failed to disconnect the session: '" + sid + "', Error: '" + status + "'");
		}
	}//leaveSession
	
	/**
	 * Create {@link ProxyBusObject}
	 * @param bus {@link BusAttachment}
	 * @return Creates and returns the {@link ProxyBusObject}, casted to the {@link NotificationProducer} object
	 */
	private NotificationProducer getProxyObject(BusAttachment bus, int sid) {
		logger.debug(TAG, "Creating ProxyBusObject with sender: '" + origSender + "', SID: '" + sid + "'");
		ProxyBusObject proxyObj = bus.getProxyBusObject(origSender,
														NotificationProducer.OBJ_PATH,
														sid, 
														new Class<?>[]{NotificationProducer.class});
		
		return proxyObj.getInterface(NotificationProducer.class);
	}//getProxyObject
	
}
