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

package org.alljoyn.ioe.controlpanelservice;

import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;

import org.alljoyn.bus.Status;
import org.alljoyn.ioe.controlpanelservice.communication.CommunicationUtil;
import org.alljoyn.ioe.controlpanelservice.communication.ConnManagerEventType;
import org.alljoyn.ioe.controlpanelservice.communication.ConnManagerEventsListener;
import org.alljoyn.ioe.controlpanelservice.communication.ConnectionManager;
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.ControlPanel;
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.HTTPControl;

import android.util.Log;

/**
 * The class includes the generic information about the controllable device. <br>
 */
public class ControllableDevice implements ConnManagerEventsListener {
	private static final String TAG = "cpan" + ControllableDevice.class.getSimpleName(); 

	/**
	 * The time window in which this device sender name may be found by AJ foundAdvertisedName <br>
	 * Time in seconds
	 */
	private static final int DEVICE_FOUND_TIME = 45; 
	
	/**
	 * Connection manager
	 */
	private ConnectionManager connMgr;
	
	/**
	 * The device registry
	 */
	private DeviceRegistry deviceRegistry;
	
	/**
	 * The scheduled service executing special task
	 */
	private ScheduledExecutorService scheduledService;
	
	/**
	 * Device events listener
	 */
	private DeviceEventsListener deviceEventsListener;
	
	/**
	 * The session id with the remote controllable device
	 * If session isn't established the value is null
	 */
	private Integer sessionId;

	/**
	 * The device unique identifier
	 */
	private String deviceId;
	
	/**
	 * The unique identifier of the remote device
	 */
	private String sender;
	
	/**
	 * The Map of functional units belonging to this {@link ControllableDevice} <br>
	 * The map is from unitId to the {@Link Unit} object
	 */
	private Map<String, Unit> unitMap;
	
	/**
	 * Whether the device is reachable, meaning may be controlled
	 */
	private AtomicBoolean isReachable;
			
	/**
	 * Constructor
	 * @param deviceId The device unique identifier
	 * @param sender The unique identifier of the remote device
	 */
	public ControllableDevice(String deviceId, String sender) {
		this.deviceId 		= deviceId;
		this.sender 		= sender;
		this.isReachable    = new AtomicBoolean(false);
		this.unitMap        = new HashMap<String, Unit>();
		
		this.sessionId      = null;
		this.deviceRegistry = ControlPanelService.getInstance().getDeviceRegistry();
		this.connMgr        = ConnectionManager.getInstance();	
	}//Constructor

	/**
	 * The device unique identifier
	 * @return the deviceId
	 */
	public String getDeviceId() {
		return deviceId;
	}

	/**
	 * The controllable device reachability state
	 * @return Whether the remotely controllable device can be reached
	 */
	public boolean isReachable() {
		return isReachable.get();
	}
	
	
	/**
	 * @return the sender's BusAttachment unique name 
	 */
	public String getSender() {
		return sender;
	}
	
	/**
	 * @return Returns sessionId. If the there is no established session with the remote device, NULL is returned
	 */
	public Integer getSessionId() {
		return sessionId;
	}
	
	/**
	 * @return Device events listener
	 */
	public DeviceEventsListener getDeviceEventsListener() {
		return deviceEventsListener;
	}//getDeviceEventsListener

	/**
	 * Starts the session with the remote controllable device
	 * @param eventsListener
	 */
	public void startSession(DeviceEventsListener eventsListener) throws ControlPanelException {
		if ( eventsListener == null ) {
			throw new ControlPanelException("Events listener can't be NULL");
		}
		
		deviceEventsListener = eventsListener;
		
		if ( sessionId != null ) {
			String msg = "The device is already in session: '" + deviceId + "', sessionId: '" + sessionId + "'";
			Log.d(TAG, msg);
			deviceEventsListener.sessionEstablished(this, getControlPanelCollections());
			return;
		}
		
		connMgr.registerEventListener(ConnManagerEventType.SESSION_JOINED, this);
		connMgr.registerEventListener(ConnManagerEventType.SESSION_LOST, this);
		connMgr.registerEventListener(ConnManagerEventType.SESSION_JOIN_FAIL, this);
		
		Log.d(TAG, "Device: '" + deviceId + "' starting session with sender: '" + sender + "'");
		Status status = connMgr.joinSession(sender, deviceId);
		
		if ( status != Status.OK ) {
			String statusName = status.name();
			Log.e(TAG, "Failed to join session: '" + statusName + "'");
			deviceEventsListener.errorOccurred(this, statusName);
			return;
		}
	}//startSession
	
	/**
	 * End the session with the remote controllable device
	 * @return {@link Status} of endSession execution
	 */
	public Status endSession() {
		Log.i(TAG, "endSession has been called, leaving the session");

		if ( sessionId == null ) {
			Log.w(TAG, "Fail to execute endSession, sessionId is NULL, returning Status of FAIL");
			return Status.FAIL;
		}
		
		Status status;
		try {
			status = connMgr.leaveSession(sessionId);
		} catch (ControlPanelException cpe) {
			Log.e(TAG, "Failed to call leaveSession, Error: '" + cpe.getMessage() + "', returning status of FAIL");
			return Status.FAIL;
		}
		
		String  logMsg = "endSession return Status is: '" + status + "'";
	
		if ( status == Status.OK ) {
			sessionId = null;
			Log.i(TAG, logMsg);
			
			// Unregister the session relevant events
			connMgr.unregisterEventListener(ConnManagerEventType.SESSION_JOINED, this);
			connMgr.unregisterEventListener(ConnManagerEventType.SESSION_LOST, this);
			connMgr.unregisterEventListener(ConnManagerEventType.SESSION_JOIN_FAIL, this);
		}
		else {
			Log.w(TAG, logMsg);
		}
				
		return status;
	}//endSession
	
	/**
	 * Receives Connection manager events
	 * @see org.alljoyn.ioe.controlpanelservice.communication.ConnManagerEventsListener#connMgrEventOccured(org.alljoyn.ioe.controlpanelservice.communication.ConnManagerEventType, java.util.Map)
	 */
	@Override
	public void connMgrEventOccured(ConnManagerEventType eventType, Map<String, Object> args) {
		switch (eventType) {
			case FOUND_DEVICE: {
				handleFoundAdvName(args);
				break;
			}
			case LOST_DEVICE: {
				handleLostAdvName(args);
				break;
			}
			case SESSION_JOINED: {
				handleSessionJoined(args);
				break;
			}
			case SESSION_JOIN_FAIL: {
				handleSessionJoinFailed(args);
				break;
			}
			case SESSION_LOST: {
				handleSessionLost(args);
				break;
			}
			default:
				break;
			}//switch
	}//connMgrEventOccured

	/**
	 * @return Collection of {@link Unit} that belongs to this device
	 */
	public Collection<Unit> getUnitCollection() {
		return Collections.unmodifiableCollection(unitMap.values());
	}//getUnits
	
	/**
	 * Adds a ControlPanel for the given objectPath.
	 * @param objectPath The object path of the added control panel
	 * @param interfaces The interfaces the object path implements
	 * @return {@link Unit} this object path belongs to
	 * @throws ControlPanelException
	 */
	public Unit addControlPanel(String objectPath, String... interfaces) throws ControlPanelException {
		if ( objectPath == null ) {
			throw new ControlPanelException("The objectPath: '" + objectPath + "' is undefined");
		}
	
		int ifaceMask = CommunicationUtil.getInterfaceMask(interfaces);
		
		if ( !CommunicationUtil.maskIncludes(ifaceMask, ControlPanel.ID_MASK) &&
			 !CommunicationUtil.maskIncludes(ifaceMask, HTTPControl.ID_MASK)
		   ) {
			throw new ControlPanelException("The objectPath: '" + objectPath + "', doesn't implement any ControlPanel permitted interface");
		}
	
		return addControlPanel(objectPath, ifaceMask);
	}//addControlPanel
	
	/**
	 * Creates a NotificationAction control panel for the given objectPath <br>
	 * The method should be used after establishment a session with a controllable device  
	 * @param objectPath The object path to the created control panel
	 * @return {@link ControlPanelCollection} 
	 * @throws ControlPanelException
	 */
	public ControlPanelCollection createNotificationAction(String objectPath) throws ControlPanelException {
		
		if ( sessionId == null ) {
			throw new ControlPanelException("The session wasn't established, can't create a ControlPanelCollection");
		}
		
		if ( objectPath == null ) {
			throw new ControlPanelException("Received an undefined objectPath");
		}
		
		Log.i(TAG, "Creating a NotificationAction control panel, objectPath: '" + objectPath + "'");
		
		String[] segments = CommunicationUtil.parseObjPath(objectPath);
		String unitId     = segments[0];
		String panelName  = segments[1];
		
		Unit unit  = new Unit(this, unitId);
		ControlPanelCollection coll = unit.createControlPanelCollection(objectPath, panelName);
		coll.retrievePanels();
		coll.handleNotificationAction();
		
		return coll;
	}//addNotificationAction
	
	/**
	 * Cleans the {@link ControlPanelCollection} that was created following the invocation of the createNotificationAction() method. <br>
	 * It's important to call this method after the NotificationAction control panel has been completed
	 * @param panelCollection The {@link ControlPanelCollection} to be cleaned out
	 */
	public void removeNotificationAction(ControlPanelCollection panelCollection) {
		panelCollection.getUnit().release();
	}//removeNotificationAction
	
	//========================================//
	
	/**
	 * @param isReachable the isActive to set
	 */
	void setReachable(boolean isReachable) {
		this.isReachable.set(isReachable);
	}
	
	/**
	 * @param sender the sender to set
	 */
	void setSender(String sender) {
		this.sender = sender;
	}
	
	/**
	 * Subscribe to receive foundAdvName and lostAdvName events of ConnectionManager
	 */
	void subscribeOnFoundLostEvents() {
		Log.d(TAG, "Register on ConnManager to receive events of found and lost advertised name");
		connMgr.registerEventListener(ConnManagerEventType.FOUND_DEVICE, this);
		connMgr.registerEventListener(ConnManagerEventType.LOST_DEVICE, this);
	}//subscribeOnFoundLostEvents
	

	/**
	 * Stops all the device activities <br>
	 * close session 				   <br>
	 * stop find adv name              <br>
	 * stop scheduled service          <br>
	 * Set is reachable to false
	 */
	void stopDeviceActivities() {
		isReachable.set(false);
		
		try {
			connMgr.cancelFindAdvertisedName(sender);
		} catch (ControlPanelException cpe) {
			Log.e(TAG, "Failed to call cancelFindAdvertisedName(), Error: '" + cpe.getMessage() + "'");
		}
		
		connMgr.unregisterEventListener(ConnManagerEventType.FOUND_DEVICE, this);
		connMgr.unregisterEventListener(ConnManagerEventType.LOST_DEVICE, this);
		
		Status status = endSession();
		if ( status != Status.OK ) {
			Log.e(TAG, "Failed to end the session, Status: '" + status + "'");
		}
		
		stopDeviceFoundVerificationService();
		
		for (Unit unit : unitMap.values() ) {
			unit.release();
		}
		
		unitMap.clear();
	}//stopDeviceActivities
	

	/**
	 * Start device found scheduled service
	 */
	void startDeviceFoundVerificationService() {
		Log.d(TAG, "Start DeviceFoundVerificationService for device: '" + deviceId + "', the verification will be done after: " + DEVICE_FOUND_TIME  + " seconds");
	
		//If the process started previously -> stop it
		stopDeviceFoundVerificationService();
		
		scheduledService = Executors.newScheduledThreadPool(1);
		scheduledService.schedule(new Runnable() {
			@Override
			public void run() {
				Log.d(TAG, "DeviceFoundVerificationService is wake up, set the device: '" + deviceId + "' reachability to 'false'");
				isReachable.set(false);
				deviceRegistry.reachabilityChanged(ControllableDevice.this, false);
			}
		}, DEVICE_FOUND_TIME, TimeUnit.SECONDS);
	}//startDeviceFoundVerification

	/**
	 * 
	 * @param objPath
	 * @param ifaceMask
	 * @throws ControlPanelException
	 */
	Unit addControlPanel(String objPath, int ifaceMask) throws ControlPanelException {
		Log.d(TAG, "Creating ControlPanelCollection object for objPath: '" + objPath + "', device: '" + deviceId + "'");
		
		//parse the received object path
		String[] segments = CommunicationUtil.parseObjPath(objPath);
		String unitId     = segments[0];
		String panelId    = segments[1];

		Unit unit = unitMap.get(unitId);
				
		if ( unit == null ) {
			Log.v(TAG, "Found new functional unit: '" + unitId + "', panel: '" + panelId + "'");
			unit = new Unit(this, unitId);
			unitMap.put(unitId, unit);            // Store the new unit object
		}else {
			Log.v(TAG, "Found an existent functional unit: '" + unitId + "', panel: '" + panelId + "'");
		}

		if ( CommunicationUtil.maskIncludes(ifaceMask, HTTPControl.ID_MASK) ) {
			Log.d(TAG, "The objPath: '" + objPath + "' belongs to the HTTPControl interface, setting");
			unit.setHttpControlObjPath(objPath);
		}
		else {
			unit.createControlPanelCollection(objPath, panelId);
		}
		
		return unit;
	}//createUnit
	
	/**
	 * @return Collection of the {@link ControlPanelCollection} of all of the functional units of this device
	 */
	private Collection<ControlPanelCollection> getControlPanelCollections() {
		List<ControlPanelCollection> collect = new LinkedList<ControlPanelCollection>();
		for ( Unit unit : unitMap.values() ) {
			collect.addAll(unit.getControlPanelCollection());
		}
		return collect;
	}// getControlPanelCollection
	
	/**
	 * Stop device found scheduled service
	 */
	private void stopDeviceFoundVerificationService() {
		if ( scheduledService == null ) {
			return;
		}
		Log.d(TAG, "Device: '" + deviceId + "' stops DeviceFoundVerification scheduled service");
		scheduledService.shutdownNow();
		scheduledService = null;
	}//stopDeviceFoundVerification
	
	
	/**
	 * If received foundAdvName: <br>
	 * Stop the scheduled timer. <br>
	 * If reachable state is false - set it to true and call the registry with reachability state changed
	 * @param args Event handler argument
	 */
	private void handleFoundAdvName(Map<String, Object> args){
		String foundSender = (String)args.get("SENDER");
		
		Log.v(TAG, "Received foundAdvertisedName of sender: '" + foundSender + "', my sender name is: '" + sender + "'");
		if ( foundSender == null || !foundSender.equals(sender) ) {
			Log.v(TAG, "The received sender: '" + foundSender + "' doesn't belong to this device");
			return;
		}
		
		stopDeviceFoundVerificationService();
		
		//Atomically sets the value to the given updated value if the current value == the expected value.
		//Returns - true if successful. False return indicates that the actual value was not equal to the expected value
		if ( isReachable.compareAndSet(false, true) ) {
			boolean newVal = isReachable.get();
			Log.d(TAG, "The device: '" + deviceId + "' isReachable set to: '" + newVal + "'");
			deviceRegistry.reachabilityChanged(this, newVal);
		}
	}//handleFoundAdvName
	
	
	/**
	 * If received lostAdvName:  <br>
	 * Stop the scheduled timer. <br>
	 * set isReachable to false
	 * @param args @param args Event handler argument
	 */
	private void handleLostAdvName(Map<String, Object> args) {
		String foundSender = (String)args.get("SENDER");
		
		Log.d(TAG, "Received lostAdvertisedName of sender: '" + foundSender + "', my sender name is: '" + sender + "'");
		if ( foundSender == null || !foundSender.equals(sender) ) {
			Log.v(TAG, "Received sender: '" + foundSender + "' doesn't belong to this device");
			return;
		}
		
		stopDeviceFoundVerificationService();
		
		//Atomically sets the value to the given updated value if the current value == the expected value.
		//Returns - true if successful. False return indicates that the actual value was not equal to the expected value
		if ( isReachable.compareAndSet(true, false) ) {
			boolean newVal = isReachable.get();
			Log.d(TAG, "The device: '" + deviceId + "' isReachable set to: '" + newVal + "'");
			deviceRegistry.reachabilityChanged(this, newVal);
		}
	}//handleLostAdvName
	
	
	/**
	 * Handle session joined
	 * @param args
	 */
	private void handleSessionJoined(Map<String, Object> args) {
		String deviceId   = (String)args.get("DEVICE_ID");
		Integer sessionId = (Integer)args.get("SESSION_ID");
		
		Log.i(TAG, "Received SESSION_JOINED event for deviceId: '" + deviceId + "', this deviceId is: '" + this.deviceId + "', sid: '" + sessionId + "'");

		if ( deviceId == null || !deviceId.equals(this.deviceId) ) {
			return;
		}
		
		if ( sessionId == null ) {
		    return;
		}
		 
		this.sessionId = sessionId;
		
		for (Unit unit : unitMap.values() ) {
			try {
				unit.fillControlPanelCollections();
			} catch (ControlPanelException cpe) {
				String error = "Failed to fill the ControlPanelCollection of the  unit: '" + unit.getUnitId() + "'";
				Log.e(TAG, error);
				deviceEventsListener.errorOccurred(this, error);
			}
		}//for unit
		
		deviceEventsListener.sessionEstablished(this, getControlPanelCollections());		
	}//handleSessionJoined

	
	/**
	 * Handle session lost
	 * @param args
	 */
	private void handleSessionLost(Map<String, Object> args) {
		Integer sessionId = (Integer)args.get("SESSION_ID");
		
		Log.w(TAG, "Received SESSION_LOST event for sessionId: '" + sessionId + "', this device sessionId is: '" + this.sessionId + "'");

		if ( sessionId == null || !sessionId.equals(this.sessionId) ) {
			return;
		}
		
		this.sessionId   = null;

		connMgr.unregisterEventListener(ConnManagerEventType.SESSION_JOINED, this);
		connMgr.unregisterEventListener(ConnManagerEventType.SESSION_LOST, this);
		connMgr.unregisterEventListener(ConnManagerEventType.SESSION_JOIN_FAIL, this);
		
		deviceEventsListener.sessionLost(this);
	}//handleSessionLost
	
	
	/**
	 * Handle session join failed
	 * @param args
	 */
	private void handleSessionJoinFailed(Map<String, Object> args) {
		String deviceId  = (String)args.get("DEVICE_ID");
		Object statusObj = args.get("STATUS");
		
		if ( statusObj == null || !(statusObj instanceof Status) ) {
			return;
		}
			
		String status = ((Status)statusObj).name();
		
		Log.w(TAG, "Received SESSION_JOIN_FAIL event for deviceId: '" + deviceId + "', this deviceId is: '" + this.deviceId + "', Status: '" + status + "'");
		if ( deviceId == null || !deviceId.equals(this.deviceId) ) {
			return;
		}
		
		this.sessionId = null;
		
		connMgr.unregisterEventListener(ConnManagerEventType.SESSION_JOINED, this);
		connMgr.unregisterEventListener(ConnManagerEventType.SESSION_LOST, this);
		connMgr.unregisterEventListener(ConnManagerEventType.SESSION_JOIN_FAIL, this);
		
		deviceEventsListener.errorOccurred(this, status);
	}//handleSessionJoinedFailed
}
