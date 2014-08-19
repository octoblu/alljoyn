/******************************************************************************
 * Copyright (c) 2013-2014, AllSeen Alliance. All rights reserved.
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

import java.lang.reflect.Method;
import java.util.Map;
import java.util.UUID;
import java.util.concurrent.RejectedExecutionException;
import java.util.concurrent.atomic.AtomicBoolean;

import org.alljoyn.about.AboutService;
import org.alljoyn.about.AboutServiceImpl;
import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.Status;
import org.alljoyn.bus.Variant;
import org.alljoyn.bus.annotation.BusSignal;
import org.alljoyn.ns.Notification;
import org.alljoyn.ns.NotificationReceiver;
import org.alljoyn.ns.NotificationServiceException;
import org.alljoyn.ns.commons.GenericLogger;
import org.alljoyn.ns.commons.NativePlatform;
import org.alljoyn.ns.transport.TaskManager;
import org.alljoyn.ns.transport.Transport;
import org.alljoyn.ns.transport.TransportNotificationText;
import org.alljoyn.ns.transport.interfaces.NotificationDismisser;
import org.alljoyn.ns.transport.interfaces.NotificationTransport;
import org.alljoyn.ns.transport.interfaces.NotificationTransportSuperAgent;
import org.alljoyn.services.common.AnnouncementHandler;
import org.alljoyn.services.common.BusObjectDescription;

/**
 * The class manages NotificationReceiver transport logic
 */
public class ReceiverTransport implements AnnouncementHandler {
	private static final String TAG = "ioe" + ReceiverTransport.class.getSimpleName();
	
	/**
	 * The reference to the platform dependent object
	 */
	private NativePlatform nativePlatform;

	private static final String SESSION_LESS_RULE           = "sessionless='t'";
	
	/**
	 * addMatch rule to receive {@link NotificationTransport} session-less-signals 
	 */
	private static final String NOTIF_TRANS_MATCH_RULE      = "type='signal',interface='" + NotificationTransport.IF_NAME + "'," + SESSION_LESS_RULE;

	/**
	 * addMatch rule to receive {@link NotificationTransportSuperAgent} session-less-signals
	 */
	private static final String SUPER_AGENT_MATCH_RULE      = "type='signal',interface='" + NotificationTransportSuperAgent.IF_NAME + "'," + SESSION_LESS_RULE;
	
	/**
	 * addMatch rule to receive {@link NotificationDismisser} session-less-signals
	 */
	private static final String DISMISSER_MATCH_RULE        = "type='signal',interface='" + NotificationDismisser.IF_NAME + "'," + SESSION_LESS_RULE;
	
	/**
	 * To receive Announcement signals from a Super Agent
	 */
	private static final String[] ANNOUNCEMENT_IFACES       = new String[]{NotificationTransportSuperAgent.IF_NAME};
	
	/**
	 * addMatch rule to receive session-less-signals from a specific SuperAgent identified by the superAgentSenderName
	 */
	private String superAgentSpecificRule;
	
	/**
	 * The sender name of the SuperAgent that this consumer is listening to receive Notification signals
	 */
	private String superAgentSenderName;
		
	/**
	 * The name of the notification signal
	 */
	private static final String NOTIF_SIGNAL_NAME           = "notify";
	
	/**
	 * The name of the Dismiss signal
	 */
	private static final String DISMISS_SIGNAL_NAME         = "dismiss";
	
	/**
	 * TRUE if we need to look for the SuperAgent
	 */
	private boolean isNeedSearchSA    = true;
	
	/**
	 * TRUE means stop forwarding notification messages to notificationReceiver
	 */
	private boolean stopReceiving     = false;
		
	
	/**
	 * TRUE means received message from SuperAgent, possible to stop receiving message from a regular consumers
	 */
	private AtomicBoolean isSuperAgentFound;
	
	/**
	 * Notification transport producer
	 * Receives and handles session less signals from a regular producers
	 */
	private NotificationTransport fromProducerChannel;
	
	/**
	 * Notification transport super agent
	 * Receives and handles session less signals from Super Agent
	 */
	private NotificationTransport fromSuperAgentChannel; 
	
	/**
	 * Receives the Dismiss signals
	 */
	private DismissConsumer dismissSignalHandler;
	
	/**
	 * Reference to NotificationReceiver object
	 */
	private NotificationReceiver notificationReceiver;
	
	/**
	 * Constructor
	 * @param nativePlatform The reference to the platform dependent object
	 */
	public ReceiverTransport(NativePlatform nativePlatform, NotificationReceiver receiver) {
		this.notificationReceiver = receiver;
		this.nativePlatform       = nativePlatform;
	}
	
	/**
	 * Is SuperAgent found
	 * @return TRUE if the SuperAgent was found
	 */
	public boolean getIsSuperAgentFound() {
		return isSuperAgentFound.get();
	}//getIsSuperAgentFound
	
	/**
	 * Starts the service in the Notification Receiver mode
	 * @throws NotificationServiceException Is thrown if failed to start the SenderTransport	 
	 */
	public void startReceiverTransport() throws NotificationServiceException {
		
		GenericLogger logger = nativePlatform.getNativeLogger();
		
		logger.debug(TAG, "Starting receiver transport");
		
		//Register to receive Notification signals directly from Producers
		//Additionally calls AddMatch(NOTIF_SLS_BASIC_RULE)
		registerReceivingProducerNotifications();

		//Register to receive Dismiss signals
		dismissSignalHandler      = new DismissConsumer();
		boolean regDismissHandler = registerDismissSignalHandler(dismissSignalHandler);
		
		if ( !regDismissHandler ) {
			logger.error(TAG, "Failed to register Dismiss signal handler");
			throw new NotificationServiceException("Failed to register Dismiss signal handler");
		}
				
		//Add match to receive notification signals from producers
		addMatchRule(NOTIF_TRANS_MATCH_RULE);
		
		//Add Match rule to receive dismiss signals
		addMatchRule(DISMISSER_MATCH_RULE);
		
		isSuperAgentFound = new AtomicBoolean(false);
		
		if ( isNeedSearchSA ) {
			logger.debug(TAG, "Need to search for SuperAgent, register SuperAgent signal receiver and announcement receiver");

			superAgentSenderName = "";
			
			//Register to receive signals directly from SA
			fromSuperAgentChannel = new NotificationTransportConsumer(NotificationTransportConsumer.FROM_SUPERAGENT_RECEIVER_PATH);
			boolean regSAHandler  = registerNotificationSignalHandlerChannel(fromSuperAgentChannel, NotificationTransportConsumer.FROM_SUPERAGENT_RECEIVER_PATH, NotificationTransportSuperAgent.IF_NAME);
			
			if ( !regSAHandler ) {
				logger.error(TAG, "Failed to register a SuperAgent signal handler");
				throw new NotificationServiceException("Failed to register a SuperAgent signal handler");
			}
			
			//Check whether About service is running in the client mode,
			// then registers to receive Announcements
			AboutService aboutService = AboutServiceImpl.getInstance();
			if ( !aboutService.isClientRunning() ) {
				logger.error(TAG, "The AboutClient wasn't started, unable to register to receive Announcement signals");
				stopReceiverTransport();
		        throw new NotificationServiceException("The AboutClient wasn't started");
			}
			
			//Register to receive announcements from SA
			logger.debug(TAG, "Registering AnnouncementReceiver");
			aboutService.addAnnouncementHandler(this, ANNOUNCEMENT_IFACES);
			
			//Add SuperAgent match rule, this allows to receive Notification signals from all the SuperAgents in proximity
		    addMatchRule(SUPER_AGENT_MATCH_RULE);
		}//if :: isNeedSearchSA
		
	}//startReceiverTransp
	
	/**
	 * ReceiverTransport cleanups
	 */
	public void stopReceiverTransport() {
		
		GenericLogger logger        = nativePlatform.getNativeLogger();
		BusAttachment busAttachment = Transport.getInstance().getBusAttachment();
		
		logger.debug(TAG, "Stopping ReceiverTransport");
		
		Method notifConsumerMethod = getNotificationConsumerSignalMethod();
		
		if ( fromSuperAgentChannel != null ) {
			
			logger.debug(TAG, "Searched for a SuperAgent, cleaning up...");
			logger.debug(TAG, "Unregister SuperAgent signal handler");
			
			if ( notifConsumerMethod != null ) {
				busAttachment.unregisterSignalHandler(fromSuperAgentChannel, notifConsumerMethod);
			}
				
			busAttachment.unregisterBusObject(fromSuperAgentChannel);
			
			if ( isSuperAgentFound.get() ) {
				removeMatchRule(superAgentSpecificRule);
				superAgentSpecificRule = "";
			}
			else {
				removeMatchRule(SUPER_AGENT_MATCH_RULE);
			}
			
			fromSuperAgentChannel = null;
			isSuperAgentFound     = null;
			
		}//if ::SuperAgent
		
		logger.debug(TAG, "Remove the AnnouncementReceiver");
		AboutServiceImpl.getInstance().removeAnnouncementHandler(this, ANNOUNCEMENT_IFACES);
		
		if ( fromProducerChannel != null ) {
			logger.debug(TAG, "Unregister Producer signal handler");
			
			if ( notifConsumerMethod != null ) {
				busAttachment.unregisterSignalHandler(fromProducerChannel, notifConsumerMethod);
			}
			
			busAttachment.unregisterBusObject(fromProducerChannel);
			fromProducerChannel = null;
			removeMatchRule(NOTIF_TRANS_MATCH_RULE);
		}//if :: producer
		
		if ( dismissSignalHandler != null ) {
			logger.debug(TAG, "Unregister Dismiss signal handler");
			
			Method dismisSignalMethod = getDismissSignalMethod();
			if ( dismisSignalMethod == null ) {
				busAttachment.unregisterSignalHandler(dismissSignalHandler, dismisSignalMethod);
			}
			
			busAttachment.unregisterBusObject(dismissSignalHandler);
			dismissSignalHandler = null;
			
			removeMatchRule(DISMISSER_MATCH_RULE);
		}//if :: dismiss
		
	}//stopReceiverTransport
	
	/**
	 * Received notification, call the notification receiver callback to pass the notification
	 * @param notification
	 */
	public void onReceivedNotification(final Notification notification) {
		
		GenericLogger logger = nativePlatform.getNativeLogger();
		
		if (stopReceiving) {
			logger.debug(TAG, "In stopSending mode NOT delivering notifications!!!");
			return;
		}

		try {
			 TaskManager.getInstance().execute (
					new Runnable() {
						@Override
						public void run() {
							notificationReceiver.receive(notification);
						}
					}//runnable
			);
		}
		catch(RejectedExecutionException ree) {
			logger.error(TAG, "Failed to return a received notification, id: '" + notification.getMessageId() + "', Error: '" + ree.getMessage() + "'");
		}
		
	}//onReceivedNotification
	
	/**
	 * Handle the received Dismiss signal
	 * @param msgId The message id of the {@link Notification} that should be dismissed
	 * @param appId The appId of the Notification sender service
	 */
	public void onReceivedNotificationDismiss(final int msgId, final UUID appId) {
		GenericLogger logger = nativePlatform.getNativeLogger();
		logger.debug(TAG, "Received the Dismiss signal notifId: '" + msgId + "', from the appId: '" + appId + "', delivering to the NotificationReceiver");
		
		try {
			TaskManager.getInstance().execute(
					new Runnable() {
						@Override
						public void run() {
							notificationReceiver.dismiss(msgId, appId);
						}
					}//runnable
			);
		}
		catch(RejectedExecutionException ree) {
			logger.error(TAG, "Failed to deliver the Dismiss event of the notifId: '" + msgId + "', from the appId:'" + appId + "', Error: '" + ree.getMessage() + "'");
		}
	}//onReceivedNotificationDismiss
	
	/**
	 * Called on received an Announcement signal
	 */
	@Override
	public void onAnnouncement(String serviceName, short port, BusObjectDescription[] objectDescriptions, Map<String, Variant> serviceMetadata) {
		GenericLogger logger 			= nativePlatform.getNativeLogger();
		BusAttachment busAttachment 	= Transport.getInstance().getBusAttachment();
		
		busAttachment.enableConcurrentCallbacks();
		
		//if SA is already found no need to parse this announcement		
		if ( isSuperAgentFound.get() ) {
		    return;	
		}
		
		logger.debug(TAG, "Received announcement signal from SA, call onReceivedFirstSuperAgentNotification");
		onReceivedFirstSuperAgentNotification(busAttachment.getMessageContext().sender);
	}//onReceivedAnnouncement

	
	/**
	 * When first Notification from SuperAgent is received:
	 * 1. Unregister producer signal handler
	 * 2. remove existing match rule
	 * 3. add match rule only of this SuperAgent 
	 */
	public synchronized void onReceivedFirstSuperAgentNotification(String superAgentSenderName) {
		
		GenericLogger logger 		= nativePlatform.getNativeLogger();
		BusAttachment busAttachment = Transport.getInstance().getBusAttachment();
		
		if ( isSuperAgentFound.get() ) {
			logger.info(TAG, "The SuperAgent has already been found, returning");
			return;
		}
		
		//Build the AddMatch rule to receive Notification from the specified SuperAgent
		superAgentSpecificRule = SUPER_AGENT_MATCH_RULE + ",sender='" + superAgentSenderName + "'";
		logger.debug(TAG, "Add the Match rule to receive Notifications from the specific SuperAgent, Rule: '" + superAgentSpecificRule + "'");
		try {	
			addMatchRule(superAgentSpecificRule);
		}
		catch(NotificationServiceException nse) {
			logger.error(TAG, "Failed to add the new Match rule: '" + superAgentSpecificRule + "', not listening to this SuperAgent, Error: '" + nse.getMessage() + "'");
			return;
		}
	 	
		//Remove the Match rule to receive notifications from ALL the SuperAgents
		logger.debug(TAG, "Remove the generic SuperAgent Match rule: '" + SUPER_AGENT_MATCH_RULE + "'");
		Status status = removeMatchRule(SUPER_AGENT_MATCH_RULE);
		if ( status != Status.OK ) {
			logger.warn(TAG, "Failed to remove the generic SuperAgent Match rule: '" + SUPER_AGENT_MATCH_RULE + "', we may continue receiving Notifications from another SuperAgents");
		}
		
		logger.debug(TAG, "Remove the Producer Match rule: '" + NOTIF_TRANS_MATCH_RULE + "'");
		status = removeMatchRule(NOTIF_TRANS_MATCH_RULE);
		if ( status != Status.OK ) {
			logger.warn(TAG, "Failed to remove Notification Producer Match rule: '" + NOTIF_TRANS_MATCH_RULE + "', we may continue receiving Notifications from Notification Producers");
		}
		
		logger.debug(TAG, "Set SuperAgent found as TRUE");
		isSuperAgentFound.set(true);
		
		this.superAgentSenderName = superAgentSenderName; 
		
		logger.debug(TAG, "Unregister from Producer signal handler");
		busAttachment.unregisterSignalHandler(fromProducerChannel, getNotificationConsumerSignalMethod());
		
		logger.debug(TAG, "Unregister from Producer bus object");
		busAttachment.unregisterBusObject(fromProducerChannel);
		fromProducerChannel = null;
	}//onReceivedFirstSuperAgentNotification

	
	/**
	 * @see org.alljoyn.services.common.AnnouncementHandler#onDeviceLost(java.lang.String)
	 */
	@Override
	public void onDeviceLost(String senderName) {
		
		GenericLogger logger = nativePlatform.getNativeLogger();
		logger.debug(TAG, "lostAdvertisedName: '" + senderName + "'");
		
		if ( !senderName.equals(superAgentSenderName) ) {
			return;
		}
		
		logger.debug(TAG, "Received lostAdvertisedName for SuperAgent: '" + superAgentSenderName + "', registering to listen for Producer and SuperAgent notifications");
		
		//Register signal handler to receive Notifications directly from consumers
		try {
			
			registerReceivingProducerNotifications();
			addMatchRule(NOTIF_TRANS_MATCH_RULE);
		} catch (NotificationServiceException nse) {
			logger.error(TAG, "Failed to register receiving Notifications back directly from Producers, Error: '" + nse.getMessage() + "'");
			return;
		}
		
		//Add the match rule to receive Notifications from all the SuperAgents in the proximity 
		try {
			addMatchRule(SUPER_AGENT_MATCH_RULE);
		} catch (NotificationServiceException nse) {
			logger.warn(TAG, "Failed to add SuperAgent generic match rule: '" + SUPER_AGENT_MATCH_RULE + "', possibly we will not receive Notifications from a SuperAgent, Error: '" + nse.getMessage() + "'");
		}
		
		logger.debug(TAG, "Removing the specific SuperAgent Match rule: '" + superAgentSpecificRule + "'");
		removeMatchRule(superAgentSpecificRule);
		
		
		superAgentSenderName   = "";
		superAgentSpecificRule = "";
		isSuperAgentFound.set(false);
	}//onDeviceLost
	

	/**
	 * Register receiving {@link Notification} messages directly from Notification producers
	 * Additionally calls AddMatch(NOTIF_SLS_BASIC_RULE)
	 * @throws NotificationServiceException
	 */
	private void registerReceivingProducerNotifications() throws NotificationServiceException {
		
		GenericLogger logger  	     = nativePlatform.getNativeLogger();
		
		//Register to receive signals directly from Producers
		logger.debug(TAG, "Registering to receive signals from producers");
		
		fromProducerChannel        = new NotificationTransportConsumer(NotificationTransportConsumer.FROM_PRODUCER_RECEIVER_PATH);
		boolean regProducerHandler = registerNotificationSignalHandlerChannel(fromProducerChannel, NotificationTransportConsumer.FROM_PRODUCER_RECEIVER_PATH, NotificationTransport.IF_NAME);
		
		if ( !regProducerHandler ) {
			logger.error(TAG, "Failed to register a Producer signal handler");
			throw new NotificationServiceException("Failed to register a Producer signal handler");
		}
		
	}//regReceivingProducerNotifications
	
	/**
	 * Register channel object to receive Notification signals 
	 * @param receiverChannel Receiver channel object
	 * @param receiverChannelServicePath The service path of the receiver channel object
	 * @param signalHandlerIfName The interface name the receiver channel is listening
	 * @param signalName The signal name that belongs to the interface name
	 * @return TRUE on success or FALSE on fail
	 */
	private boolean registerNotificationSignalHandlerChannel (NotificationTransport receiverChannel, String receiverChannelServicePath, String signalHandlerIfName) {
		GenericLogger logger = nativePlatform.getNativeLogger();
		
		logger.debug(TAG, "Registering signal handler for interface: '" + signalHandlerIfName + "' servicePath: " + receiverChannelServicePath);

		Method handlerMethod = getNotificationConsumerSignalMethod();
		if ( handlerMethod == null ) {
			return false;
		}
		
		boolean regRes = Transport.getInstance().registerObjectAndSetSignalHandler(logger, signalHandlerIfName, NOTIF_SIGNAL_NAME, handlerMethod, receiverChannel, receiverChannelServicePath);

		if ( !regRes ) {
			stopReceiverTransport();       // Stop receiver transport to allow later recovery
			return false;
		}
			
		return true;
	}//registerSignalHandlerChannel
	
	
	/**
	 * Registers Dismiss signal receiver 
	 * @param dismissConsumer
	 * @return TRUE on success or FALSE on fail
	 */
	private boolean registerDismissSignalHandler(DismissConsumer dismissConsumer) {
		GenericLogger logger = nativePlatform.getNativeLogger();
		
		logger.debug(TAG, "Registering signal handler for interface: '" + DismissConsumer.IF_NAME + "' servicePath: '" + DismissConsumer.OBJ_PATH + "'");
		
		Method handlerMethod = getDismissSignalMethod();
		if ( handlerMethod == null ) {
			return false;
		}
		
		String allJoynName = handlerMethod.getAnnotation(BusSignal.class).name();
		boolean regRes = Transport.getInstance().registerObjectAndSetSignalHandler(logger, DismissConsumer.IF_NAME, allJoynName, handlerMethod, dismissConsumer, DismissConsumer.OBJ_PATH);

		if ( !regRes ) {
			stopReceiverTransport();       // Stop receiver transport to allow later recovery
			return false;
		}
			
		return true;
	}//registerDismissSignalReceiver
	
	/**
	 * Call the method {@link BusAttachment#addMatch(String)} with the given rule
	 * @param rule The rule to add
	 * @throws NotificationServiceException is throws if the AddMatch return status wasn't OK
	 */
	private void addMatchRule(String rule) throws NotificationServiceException { 
		
		GenericLogger logger = nativePlatform.getNativeLogger();
		logger.debug(TAG, "Call AddMatch rule: '" + rule + "'");
		
		BusAttachment bus = Transport.getInstance().getBusAttachment();
		Status status     = bus.addMatch(rule);
		
		if ( status != Status.OK ) {
			logger.error(TAG, "Failed to call AddMatch rule: '" + rule + "', Error: '" + status + "'");
			throw new NotificationServiceException("Failed to call AddMatch rule: '" + rule + "', Error: '" + status + "'");
		}
		
		logger.debug(TAG, "Status from AddMatch rule: '" + rule + "', status: '" + status + "'");
	}//addMathcRule

	/**
	 * Call the method {@link BusAttachment#removeMatch(String)} with the given rule
	 * @param rule The rule to add	
	 * @return The result status
	 */
	private Status removeMatchRule(String rule) {	
		BusAttachment bus = Transport.getInstance().getBusAttachment();
		Status status     = bus.removeMatch(rule);
		nativePlatform.getNativeLogger().debug(TAG, "RemoveMatch rule: '" + rule + "' result: '" + status + "'");
		return status;
	}//removeMatchRule
	
	
	/**
	 * Returns reflection of {@link NotificationTransport#notify} method
	 * Used to register signal handler dynamically (without signal annotation on the method) 
	 * @return Method object or NULL if failed to retrieve
	 */
	private Method getNotificationConsumerSignalMethod() {
		Method  retMethod;
		try {
			retMethod = NotificationTransport.class.getMethod(NOTIF_SIGNAL_NAME,
														      Integer.TYPE,   				      // version
														      Integer.TYPE,   			          // msgId,														      
														      Short.TYPE,	 				      // messageType,
														      String.class,                       // deviceId,
														      String.class,                       // deviceName,
														      byte[].class,   				      // appId,
														      String.class,   				      // appName,														
														      Map.class,		 				  // attributes
														      Map.class,      				  	  // customAttributes
														      TransportNotificationText[].class); // text

		}
		catch(Exception ex) {
			nativePlatform.getNativeLogger().error(TAG, "Failed to get a reflection of the signal method: '" + NOTIF_SIGNAL_NAME + "', Error: " + ex.getMessage());
			retMethod = null;
		}
		
		return retMethod;
	}//getNotificationTransportSignalMethod


	
	/**
	 * Returns reflection of the {@link DismissConsumer#dismiss(int, byte[])}
	 * @return Method object or NULL if failed to retrieve
	 */
	private Method getDismissSignalMethod() {
		Method retMethod;
		
		try {
			retMethod = NotificationDismisser.class.getMethod(DISMISS_SIGNAL_NAME,
															  int.class,
															  byte[].class);
		}
		catch(Exception ex) {
			nativePlatform.getNativeLogger().error(TAG, "Failed to get a reflection of the signal method: '" + DISMISS_SIGNAL_NAME + "', Error: " + ex.getMessage());
			retMethod = null;
		}
		
		return retMethod;
	}//getDismissSignalMethod
	
}
