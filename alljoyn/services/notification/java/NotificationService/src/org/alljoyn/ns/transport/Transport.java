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


package org.alljoyn.ns.transport;

import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.Map;
import java.util.UUID;

import org.alljoyn.about.AboutKeys;
import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.BusObject;
import org.alljoyn.bus.Status;
import org.alljoyn.bus.Variant;
import org.alljoyn.ns.Notification;
import org.alljoyn.ns.NotificationMessageType;
import org.alljoyn.ns.NotificationReceiver;
import org.alljoyn.ns.NotificationServiceException;
import org.alljoyn.ns.commons.GenericLogger;
import org.alljoyn.ns.commons.NativePlatform;
import org.alljoyn.ns.commons.NativePlatformFactory;
import org.alljoyn.ns.commons.NativePlatformFactoryException;
import org.alljoyn.ns.transport.consumer.ReceiverTransport;
import org.alljoyn.ns.transport.producer.SenderTransport;
import org.alljoyn.services.android.storage.Property;
import org.alljoyn.services.common.PropertyStore;
import org.alljoyn.services.common.PropertyStore.Filter;
import org.alljoyn.services.common.PropertyStoreException;

/**
 * The main transport controller class 
 * Manages {@link SenderTransport} and {@link ReceiverTransport} objects
 */
public class Transport {
	
	private static final String TAG = "ioe" + Transport.class.getSimpleName(); 
	
	/**
	 * Reference to Transport object
	 */
	private static final Transport transport = new Transport();
	
	/**
	 * {@link SenderTransport} logic
	 */
	private SenderTransport senderTransport;
	
	/**
	 * {@link ReceiverTransport} logic
	 */
	private ReceiverTransport receiverTransport;
	
	/**
	 * Reference to native platform object
	 */
	private NativePlatform nativePlatform;
	
	/**
	 * Service configuration properties
	 */
	private PropertyStore propertyStore;
	
	/**
	 * Reference to BusAttachment object
	 */
	private BusAttachment busAttachment;
	
	/**
	 * Received TRUE if sender transport was already called
	 */
	private boolean isSenderTransportCalled     = false;
	
	/**
	 * Receives TRUE if receiver transport was called
	 */
	private boolean isReceiverTransportCalled   = false;
	
	/**
	 * @return  Return the {@link Transport} object
	 */
	public static Transport getInstance() {
		return transport;
	}//getInstance

	/**
	 * Constructor
	 * private constructor - to avoid creation of Transport object by other classes
	 */
	private Transport() {}//Constructor
	
	/**
	 * @return Reference to the {@link BusAttachment}
	 */
	public BusAttachment getBusAttachment() {
		return busAttachment;
	}
	
	/**
	 * @return All properties read from the {@link PropertyStore}
	 * @throws If failed to read the properties from the {@link PropertyStore}
	 */
	public Map<String, Object> readAllProperties() throws NotificationServiceException {
   	    Map<String, Object> props = new HashMap<String, Object>(); 
   	    try {
			propertyStore.readAll(Property.NO_LANGUAGE, Filter.READ, props);
		} catch (PropertyStoreException pse) {
			throw new NotificationServiceException("Failed to read properties from the PropertyStore, Error: '" + pse.getMessage() + "'", pse);
		}
   	    
   	    return props;
	}//readAllProperties
	
	/**
	 * Retrieve AppId from the given properties
	 * @param props {@link PropertyStore} properties Map object
	 * @return AppId
	 * @throws NotificationServiceException if failed to retrieve the AppId parameter
	 */
	public UUID getAppId(Map<String, Object> props) throws NotificationServiceException {
		if ( props == null ) {
			throw new NotificationServiceException("props can't be NULL");
		}
		
   	    Object appIdObj   = props.get(AboutKeys.ABOUT_APP_ID);
   	    if ( !(appIdObj instanceof UUID) ) {
   	    	throw new NotificationServiceException("The AppId is NULL or not an instance of UUID");
   	    }
   	    
   	    return (UUID) appIdObj;
	}//getAppId
	
	/**
	 * @return Returns TRUE if SuperAgent was found
	 */
	public synchronized boolean getIsSuperAgentFound() {
		if ( receiverTransport == null ) {
			return false;
		}
		return receiverTransport.getIsSuperAgentFound();
	}//getSuperAgentFound
	
	
	
	/**
	 * Starts the service in the Sender mode
	 * @param propertyStore The reference to the application {@link PropertyStore} object
	 * @param bus The {@link BusAttachment} to be used by this {@link Transport} object
	 * @throws NotificationServiceException Is thrown if failed to start the SenderTransport
	 */
	public synchronized void startSenderTransport(BusAttachment bus, PropertyStore propertyStore) throws NotificationServiceException {
		GenericLogger logger;
		logger = getLogger();

		if ( isSenderTransportCalled ) {
			logger.debug(TAG, "Sender transport was previously started, returning");
			return;
		}
		
		//Store the received busAttachment or verify if already exists
		saveBus(bus);
		
		TaskManager taskManager = TaskManager.getInstance();
		if ( !taskManager.isRunning() ) {
			taskManager.initPool(nativePlatform);
		}
		
		this.propertyStore = propertyStore;
		
		senderTransport    = new SenderTransport(nativePlatform);
		
		try {
			senderTransport.startSenderTransport();   //Delegate the starting sender transport logic
		}
		catch(NotificationServiceException nse) {
			stopSenderTransport(logger);
			throw nse;
		}
		
		isSenderTransportCalled = true;
	}//startSenderTransport
	
	
	/**
	 * Starts the Receiver Transport
	 * @param bus The {@link BusAttachment} to be used by this {@link Transport} object
	 * @param receiver {@link NotificationReceiver} 
	 * @throws NotificationServiceException Is thrown if failed to start the ReceiverTransport
	 */
	public synchronized void startReceiverTransport(BusAttachment bus, NotificationReceiver receiver) throws NotificationServiceException {
		GenericLogger logger;
		logger = getLogger();
		
		if ( isReceiverTransportCalled ) {
			logger.debug(TAG, "Receiver transport was previously started, returning");
			return;
		}
		
		saveBus(bus);
		
		TaskManager taskManager = TaskManager.getInstance();
		if ( !taskManager.isRunning() ) {
			taskManager.initPool(nativePlatform);
		}
		
		receiverTransport = new ReceiverTransport(nativePlatform, receiver);
		
		try {
			receiverTransport.startReceiverTransport();
		}
		catch(NotificationServiceException nse) {
			stopReceiverTransport(logger);
			throw nse;
		}
		
		isReceiverTransportCalled = true;
	}//startReceiverTransport
	

	/**
	 * Called when we need to send a signal
	 * @param version The version of the message signature
	 * @param msgId notification Id the id of the sent signal
	 * @param messageType  The message type of the sent message
	 * @param deviceId Device id
	 * @param deviceName Device name
	 * @param appId App id
	 * @param appName App name
	 * @param attributes All the notification metadata
	 * @param customAttributes The customAttributes
	 * @param text Array of texts to be sent
	 * @param ttl Notification message TTL 
	 * @throws NotificationServiceException
	 */
	public void sendNotification(int version, int msgId, NotificationMessageType messageType, String deviceId, String deviceName, byte[] appId, String appName, Map<Integer, Variant> attributes, Map<String, String> customAttributes,TransportNotificationText[] text, int ttl) throws NotificationServiceException {
		GenericLogger logger;
		
		try {
			logger = getLogger();
		}
		catch (NotificationServiceException nse) {
		    throw nse;
		}
		
		if (!isSenderTransportCalled){
			logger.error(TAG,"Notification service is not running, can't send message");
			throw new NotificationServiceException("Notification service is not running, can't send message");
		}
		
		senderTransport.sendNotification(version,
										 msgId,
										 messageType,
										 deviceId, 
										 deviceName,
										 appId,
										 appName,
										 attributes,
										 customAttributes,
										 text,
										 ttl);

	}//sendNotification
	

	/**
	 * Cancel the last message sent for the given messageType
	 * @param messageType
	 * @throws NotificationServiceException 
	 */
	public void deleteLastMessage(NotificationMessageType messageType) throws NotificationServiceException {
		GenericLogger logger;
		try {
			logger = getLogger();
		}
		catch (NotificationServiceException nse) {
		    throw nse;
		}

		if ( !isSenderTransportCalled ) {
			logger.error(TAG,"Notification service is not running, can't delete message");
			throw new NotificationServiceException("Notification service is not running, can't delete message");
		}
		
		senderTransport.deleteLastMessage(messageType);
	}//deleteLastMessage

	
	/**
	 * Received notification, call the notification receiver callback to pass the notification
	 * @param notification
	 */
	public void onReceivedNotification(final Notification notification) {
		
		GenericLogger logger;
		try {
			logger = getLogger();
		}
		catch (NotificationServiceException nse) {
		    System.out.println("Could not get logger in receive notification error: " + nse.getMessage());
		    return;
		}
		
		synchronized (this) {
			
			if ( !isReceiverTransportCalled ) {
				logger.error(TAG, "Received a Notification message, but the Notification Receiver transport is stopped");
				return;
			}
			
			receiverTransport.onReceivedNotification(notification);
		}//synchronized
		
	}//onReceivedNotification
	
	/**
	 * Handle the received Dismiss signal
	 * @param msgId The message id of the {@link Notification} that should be dismissed
	 * @param appId The appId of the Notification sender service
	 */
	public void onReceivedNotificationDismiss(int msgId, UUID appId) {
		receiverTransport.onReceivedNotificationDismiss(msgId, appId);
	}//onDismissReceived

	/**
	 * Handle receiving a first Notification from a SuperAgent
	 */
	public void onReceivedFirstSuperAgentNotification(String superAgentUniqueName) {
		
		GenericLogger logger;
		try {
			logger = getLogger();
		}
		catch (NotificationServiceException nse) {
		    System.out.println("Could not get logger in onReceivedFirstSuperAgentNotification error: '" + nse.getMessage() + "'");
		    return;
		}
		
		if ( !isReceiverTransportCalled ) {
			logger.error(TAG, "The method 'onReceivedFirstSuperAgentNotification' was called, but the Notification Received is stopped");
			return;
		}
		
		receiverTransport.onReceivedFirstSuperAgentNotification(superAgentUniqueName);
	}//onReceivedFirstSuperAgentNotification
	
	/**
	 * Stop Notification Service
	 * @throws NotificationServiceException
	 */
	public synchronized void shutdown() throws NotificationServiceException {
		GenericLogger logger = getLogger();
		
		if ( busAttachment == null ) {
			logger.warn(TAG,"No BusAttachment defined, sender and receiver must be down, returning");
			return;
		}
		
		logger.debug(TAG, "Shutdown called, stopping sender and receiver transports");
		stopSenderTransport(logger);
		stopReceiverTransport(logger);
	}//shutdown
	
	/**
	 * Verifies that sender transport was started, then calls stopSenderTransport() 
	 * @throws NotificationServiceException
	 */
	public synchronized void shutdownSender() throws NotificationServiceException {
		GenericLogger logger = getLogger();
		
		if ( !isSenderTransportCalled ) {
			logger.error(TAG, "Sender service wasn't started");
			throw new NotificationServiceException("Sender service wasn't started");
		}
		
		logger.debug(TAG, "Stopping sender transport");
		stopSenderTransport(logger);
	}//stopSender
	
	/**
	 * Verifies that receiver transport was started, then calls stopReceiverTransport()
	 * @throws NotificationServiceException
	 */
	public synchronized void shutdownReceiver() throws NotificationServiceException {
		GenericLogger logger = getLogger();
		
		if ( !isReceiverTransportCalled ) {
			logger.error(TAG, "Receiver service wasn't started");
			throw new NotificationServiceException("Receiver service wasn't started");
		}
		
		logger.debug(TAG, "Stopping receiver transport");
		stopReceiverTransport(logger);
	}//stopReceiver
	
	/**
	 * Uses setNativePlatform to receive the GenericLogger
	 * @return GenericLogger Returns GenericLogger
	 * @throws NotificationServiceException thrown if no native platform object is defined
	 */
	public GenericLogger getLogger() throws NotificationServiceException {
		setNativePlatform();
		return nativePlatform.getNativeLogger();
	}//getLogger
	
	
	//********************* PRIVATE METHODS *********************//

	/**
	 * Stores the {@link BusAttachment} if it's not already exists and is connected to the Bus<br>
	 * If the {@link BusAttachment} already exists, checks whether the received {@link BusAttachment} is the same
	 * as the existent one. If not the {@link NotificationServiceException} is thrown 
	 * @param bus
	 * @throws NotificationServiceException
	 */
	private void saveBus(BusAttachment bus) throws NotificationServiceException {
		
		GenericLogger logger = nativePlatform.getNativeLogger();
		
		if ( bus == null ) {
			logger.error(TAG, "Received a NULL BusAttachment");
			throw new NotificationServiceException("Received a not initialized BusAttachment");
		}
		
		if ( this.busAttachment == null ) {
			if ( !bus.isConnected() ) {
				logger.error(TAG, "Received a BusAttachment that is not connected to the AJ Bus");
				throw new NotificationServiceException("Received a BusAttachment that is not connected to the AJ bus");
			}
			logger.info(TAG, "BusAttachment is stored successfully");
			this.busAttachment = bus;
		}
		else if ( !this.busAttachment.getUniqueName().equals(bus.getUniqueName()) ) {
			logger.error(TAG, "NotificationService received a new BusAttachment: '" + bus.getUniqueName() + "', while previously initialized with a BusAttachment: '" + busAttachment.getUniqueName() + "'");
			throw new NotificationServiceException("BusAttachment already exists");
		}
		
	}//saveBus
	
		
	
	/**
	 * Uses NativePlatformFactory to receive NativePlatform object
	 * @throws NotificationServiceException thrown if failed to receive native platform factory
	 */
	private void setNativePlatform() throws NotificationServiceException {
		if ( nativePlatform == null ) {
			try {
				nativePlatform = NativePlatformFactory.getPlatformObject();
			}
			catch (NativePlatformFactoryException npfe) {
				throw new NotificationServiceException(npfe.getMessage());
			}
		}
	}//setNativePlatform

	
	/**
	 * Register bus object that is intended to be a signal handler
	 * @param logger
	 * @param ifName Interface name that signal handler object belongs to
	 * @param signalName 
	 * @param handlerMethod The reflection of the method that is handling the signal 
	 * @param toBeRegistered The object to be registered on the bus and set to be signal handler
	 * @param servicePath The identifier of the object
	 * @return TRUE on success and FALSE on fail
	 */
	public boolean registerObjectAndSetSignalHandler(GenericLogger logger, String ifName, String signalName, Method handlerMethod, BusObject toBeRegistered, String servicePath) {
		logger.debug(TAG, "Registering BusObject and setting signal handler, IFName: '" + ifName + ", method: '" + handlerMethod.getName() + "'");
		Status status = busAttachment.registerBusObject(toBeRegistered, servicePath);
		if ( status != Status.OK ) {
			logger.error(TAG, "Failed to register bus object, status: " + status);
			return false;
		}
				
		status = busAttachment.registerSignalHandler(ifName, signalName, toBeRegistered, handlerMethod);
		if ( status != Status.OK ) {
			logger.error(TAG, "Failed to register signal handler, status: " + status);
			return false;
		}
		return true;
	}//registerObjectAndSetSignalHandler

	
	
	/**
	 * Sender Transport cleanup
	 * @param logger
	 */
	private void stopSenderTransport(GenericLogger logger) {
		 
		if ( senderTransport != null ) {
			senderTransport.stopSenderTransport();
			senderTransport = null;
		}
		
		// clear the resources that are generic between sender and receiver
		if ( !isReceiverTransportCalled ) {
			
			logger.debug(TAG, "Receiver is not running, clearing common resources");
			
			TaskManager taskManager = TaskManager.getInstance();
			if ( taskManager.isRunning() ) {
				taskManager.shutdown();
			}
			
			busAttachment = null;
		}
		
		propertyStore           = null;
		isSenderTransportCalled = false;
	}//cleanTransportProducerChannel
	
	
	/**
	 * Receiver Transport cleanup
	 * @param logger
	 */
	private void stopReceiverTransport(GenericLogger logger) {
		
		if ( receiverTransport != null ) {
			receiverTransport.stopReceiverTransport();
			receiverTransport = null;
		}
		
		// clear the resources that are generic between sender and receiver
		if ( !isSenderTransportCalled ) {
			
			logger.debug(TAG, "Sender is not running, clearing common resources");
			
			TaskManager taskManager = TaskManager.getInstance();
			if ( taskManager.isRunning() ) {
				taskManager.shutdown();
			}
			
			busAttachment = null;
		}
		
		isReceiverTransportCalled = false;
	}//stopReceiverTransport

	
}//Transport

