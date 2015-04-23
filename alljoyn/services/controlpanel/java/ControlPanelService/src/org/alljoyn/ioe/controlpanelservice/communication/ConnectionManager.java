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

package org.alljoyn.ioe.controlpanelservice.communication;

import java.lang.reflect.Method;
import java.util.Comparator;
import java.util.EnumMap;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentSkipListSet;

import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.BusListener;
import org.alljoyn.bus.OnJoinSessionListener;
import org.alljoyn.bus.ProxyBusObject;
import org.alljoyn.bus.SessionListener;
import org.alljoyn.bus.SessionOpts;
import org.alljoyn.bus.Status;
import org.alljoyn.ioe.controlpanelservice.ControlPanelException;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.util.Log;

/**
 * Performs AJ communication for controller 
 */
public class ConnectionManager implements Handler.Callback {

	private class ConnMgrBusListener extends BusListener {
	
		/**
		 * @see org.alljoyn.bus.BusListener#foundAdvertisedName(java.lang.String, short, java.lang.String)
		 */
		@Override
		public void foundAdvertisedName(String name, short transport, String namePrefix) {
			if ( bus == null ) {
				return;
			}
			
			bus.enableConcurrentCallbacks();
			Map<String, Object> args = new HashMap<String, Object>();
			args.put("SENDER", name);
			
			if ( handler != null ) {
				Message.obtain(handler, ConnManagerEventType.FOUND_DEVICE.ordinal(), args).sendToTarget();
			}
		}//foundAdvertisedName

		/**
		 * @see org.alljoyn.bus.BusListener#lostAdvertisedName(java.lang.String, short, java.lang.String)
		 */
		@Override
		public void lostAdvertisedName(String name, short transport, String namePrefix) {
			bus.enableConcurrentCallbacks();
			Map<String, Object> args = new HashMap<String, Object>();
			args.put("SENDER", name);
			
			if ( handler != null ) {
				Message.obtain(handler, ConnManagerEventType.LOST_DEVICE.ordinal(), args).sendToTarget();
			}
		}//lostAdvertisedName
		
	}
	
	//===================================================//
	
	/**
	 * Events listener of lost session 
	 */
	private class ConnMgrSessionListener extends SessionListener {
		
		@Override
		public void sessionLost(int sessionId, int reason) {
			
			Log.d(TAG, "Received SESSION_LOST for session: '" + sessionId + "', Reason: '" + reason + "'");
			Map<String, Object> args = new HashMap<String, Object>();
			args.put("SESSION_ID", sessionId);
			
			if ( handler != null ) {
				Message.obtain(handler, ConnManagerEventType.SESSION_LOST.ordinal(), args).sendToTarget();
			}
		}//sessionLost
		
	}//ControlPanelSessionListener

	//=====================================================//
	
	/**
	 * OnJoinSession events listener
	 */
	private class ConnMgrOnJoinSessionListener extends OnJoinSessionListener {
		@Override
		public void onJoinSession(Status status, int sessionId, SessionOpts opts, Object context) {
			
			super.onJoinSession(status, sessionId, opts, context);
			Map<String, Object> args = new HashMap<String, Object>();
			args.put("DEVICE_ID", context.toString());
			args.put("STATUS", status);
			
			if (status != Status.OK) {
				Log.e(TAG, "JoinSessionCB - Failed to join session, device: '" + context.toString() +"' Status: '" + status + "'");
				
				if ( handler != null ) {
					Message.obtain(handler, ConnManagerEventType.SESSION_JOIN_FAIL.ordinal(), args).sendToTarget();
				}
				
				return;
			}
			
			Log.d(TAG, "JoinSessionCB - Succeeded to join session, device: '" + context.toString() +"' SID: '" + sessionId + "'");
			args.put("SESSION_ID", sessionId);
			
			if ( handler != null ) {
				Message.obtain(handler, ConnManagerEventType.SESSION_JOINED.ordinal(), args).sendToTarget();
			}
			
		}//onJoinSession
	}//ControlPanelOnJoinSessionListener

	//=====================================================//
	
	private class ListenersComparator implements Comparator<ConnManagerEventsListener> {
		@Override
		public int compare(ConnManagerEventsListener lhs, ConnManagerEventsListener rhs) {
			if ( lhs.hashCode() > rhs.hashCode() ) {
				return 1;
			}
			if ( lhs.hashCode() < rhs.hashCode() ) {
				return -1;
			}
			else {
				return 0;
			}			
		}//compare		
	}//ListenersComparator
	
	//=============================================================//
	//                   END OF NESTED CLASSES                     //
	//=============================================================//
	
	private static final String TAG = "cpan" + ConnectionManager.class.getSimpleName();

	
	private static final ConnectionManager SELF       = new ConnectionManager();
	
	public static final String ANNOUNCE_SIGNAL_NAME   = "Announce";
	
	private static final short PORT_NUMBER            = 1000;

	/**
	 * BusAttachment
	 */
	private BusAttachment bus;
	
	/**
	 * AJ Session listener object
	 */
	private ConnMgrSessionListener sessionListener;
	
	/**
	 * AJ On join session listener
	 */
	private ConnMgrOnJoinSessionListener onJoinSessionListener;
	
	/**
	 * AJ Bus listener
	 */
	private ConnMgrBusListener busListener;
	
	/**
	 * Announcement receiver
	 */
	private AnnouncementReceiver announcementReceiver;
	
	/**
	 * Events listeners
	 */
	private volatile Map<ConnManagerEventType, Set<ConnManagerEventsListener>> listenersContainer;
	
	/**
	 * Handler thread for the Looper thread
	 */
	private HandlerThread handlerThread;
	
	/**
	 * Handler
	 */
	private Handler handler;
	
	/**
	 * Constructor
	 */
	private ConnectionManager(){
		listenersContainer 		= new EnumMap<ConnManagerEventType, Set<ConnManagerEventsListener>>(ConnManagerEventType.class);
		sessionListener    		= new ConnMgrSessionListener();
		onJoinSessionListener   = new ConnMgrOnJoinSessionListener();
		busListener             = new ConnMgrBusListener();
	}
	
	/**
	 * @return ConnectionManager instance
	 */
	public static ConnectionManager getInstance() {
		return SELF;
	}
	
	/**
	 * @see android.os.Handler.Callback#handleMessage(android.os.Message)
	 */
	@Override
	public boolean handleMessage(Message msg) {
		ConnManagerEventType eventType = ConnManagerEventType.values()[msg.what];
		Log.d(TAG, "Received message type: '" + eventType + "'");
		@SuppressWarnings("unchecked")
		Map<String, Object> args = (Map<String, Object>)msg.obj;
		notifyListeners(eventType, args);
		return true;
	}//handleMessage

	/**
	 * Set bus attachment to be used
	 * @param busAttachment
	 */
	public void setBusAttachment(BusAttachment busAttachment) throws ControlPanelException {
		if ( busAttachment == null || !busAttachment.isConnected() ) {
			throw new ControlPanelException("The received BusAttachment is not connected");
		}
		if ( bus != null ) {
			throw new ControlPanelException("The BusAttachment already exists");
		}
		
		if ( handlerThread == null ) {
			handlerThread           = new HandlerThread("ControlPanelConnMgr");
			handlerThread.start();
			handler                 = new Handler(handlerThread.getLooper(), this);
		}
		
		bus = busAttachment;
		bus.registerBusListener(busListener);
	}//setBusAttachment
	
	/**
	 * Returns handler
	 * @return
	 */
	public Handler getHandler() {
		return handler;
	}//returns handler

	/**
	 * @return BusAttachement that in use of the ConnectionManager
	 */
	public BusAttachment getBusAttachment() {
		return bus;
	}
	
	/**
	 * @return {@link AnnouncementReceiver} object
	 */
	public AnnouncementReceiver getAnnouncementReceiver() {
		return announcementReceiver;
	}
	
	/**
	 * Add listener to be notified when the event of eventType happened
	 * @param eventType
	 * @param listener The listener object
	 */
	public void registerEventListener(ConnManagerEventType eventType, ConnManagerEventsListener listener) {
		synchronized (listenersContainer) {
			Set<ConnManagerEventsListener> listeners = listenersContainer.get(eventType);
			if ( listeners == null ) {
				 listeners = new ConcurrentSkipListSet<ConnManagerEventsListener>( new ListenersComparator() );
				 listeners.add(listener);
				 listenersContainer.put(eventType, listeners);
			}
			else {
				listeners.add(listener);
			}//listener == null
		}//synch
	}//registerEventListener
	
	/**
	 * Removes the listener from receiving events of type eventType
	 * @param eventType
	 * @param listener
	 */
	public void unregisterEventListener (ConnManagerEventType eventType, ConnManagerEventsListener listener) {
		Set<ConnManagerEventsListener> listeners = listenersContainer.get(eventType);
		if ( listeners == null) {
			return;
		}
		else {
			listeners.remove(listener);
	    }
	}//unregisterEventListener
	
	/**
	 * Notify the events listeners
	 * @param eventType
	 */
	public void notifyListeners(ConnManagerEventType eventType, Map<String, Object> args) {
		Set<ConnManagerEventsListener> listeners = listenersContainer.get(eventType);
		if ( listeners == null ) {
			return;
		}
		else {
			// notify listeners
			for (ConnManagerEventsListener listener : listeners) {
				listener.connMgrEventOccured(eventType, args);
			}
	    }
	}//notify listeners
		
	/**
	 * Start find advertised name
	 * @param wellKnownName
	 * @return Status AllJoyn Status
	 * @throws ControlPanelException if failed to execute findAdvertisedName 
	 */
	public Status findAdvertisedName(String wellKnownName) throws ControlPanelException {
		if ( bus == null ) {
			throw new ControlPanelException("The BusAttachment is not defined");
		}
		
		return bus.findAdvertisedName(wellKnownName);
	}//findAdvertisedName
	
	/**
	 * Cancel findAdvertisedName
	 * @return AllJoyn Status
	 * @throws ControlPanelException If failed to execute cancelFindAdvertisedName 
	 */
	public Status cancelFindAdvertisedName(String wellKnownName) throws ControlPanelException {
		if ( bus == null ) {
			throw new ControlPanelException("The BusAttachment is not defined");
		}
		
		return bus.cancelFindAdvertisedName(wellKnownName);
	}
	
	/**
	 * Register bus object that is intended to be signal handler
	 * @param ifName Interface name that signal handler object belongs to
	 * @param signalName 
	 * @param handlerMethod The reflection of the method that is handling the signal 
	 * @param toBeRegistered The object to be registered on the bus and set to be signal handler
	 * @param servicePath The identifier of the object
	 * @param source The object path to receive signals from
	 * @throws ControlPanelException if failed to register signal handler
	 */
	public void registerObjectAndSetSignalHandler(String ifName, String signalName, Method handlerMethod, Object toBeRegistered, String objectPath, String source) throws ControlPanelException {
		Log.d(TAG, "Registering BusObject and setting signal handler, IFName: '" + ifName + ", method: '" + handlerMethod.getName() + "'");
		
		if ( bus == null ) {
			throw new ControlPanelException("Not initialized BusAttachment");
		}
		
		Status status;
		
		if ( source.length() == 0 ) {
			Log.d(TAG, "Registering signal handler without source object");
			status = bus.registerSignalHandler(ifName, signalName, toBeRegistered, handlerMethod);
		}
		else {
			Log.d(TAG, "Registering signal handler with source object: '" + source + "'");
			status = bus.registerSignalHandler(ifName, signalName, toBeRegistered, handlerMethod, source);
		}
		
		if ( status == Status.OK ) {
			Log.d(TAG, "Signal receiver ifname: '" + ifName + "', signal '" + signalName + "' registered successfully");
		}
		else {
			Log.e(TAG, "Failed to register signal handler, status: '" + status + "'");
			throw new ControlPanelException("Failed to register signal handler");
		}
		
		status = bus.addMatch("type='signal',interface='" + ifName + "',member='" + signalName + "'");
		if ( status != Status.OK ) {
			Log.e(TAG, "Failed to register addMatch rule for interface: '" + ifName + "',signal: '" + signalName + "', status: '" + status + "'");
			throw new ControlPanelException("Failed to register signal handler");
		}
	}//registerObjectAndSetSignalHandler
	
	/**
	 * Unregisters the signal handler and the bus object
	 * @param receiverObj
	 * @param handlerMethod
	 * @throws ControlPanelException if failed to register signal handler
	 */
	public void unregisterSignalHandler(Object receiverObj, Method handlerMethod) throws ControlPanelException {
		if ( bus == null ) {
			throw new ControlPanelException("The BusAttachment is not defined");
		}
		
		bus.unregisterSignalHandler(receiverObj, handlerMethod);
	}//unregisterObjectAndSignalHandler
	
	/**
	 * Join session with the sender's host
	 * @param sender The host to join the session with
	 * @param deviceId The device Id this join session request belongs to
	 * @return Return status whether the synchronous part of join session succeeded
	 * @throws ControlPanelException if failed to register signal handler
	 */
	public Status joinSession(String sender, String deviceId) throws ControlPanelException {
		if ( bus == null ) {
			throw new ControlPanelException("The BusAttachment is not defined");
		}
		
		Status status = bus.joinSession(sender, PORT_NUMBER, getSessionOpts(), sessionListener, onJoinSessionListener, deviceId);
		return status;
	}//joinSession
	
	/**
	 * @param sessionId Leave session
	 */
	public Status leaveSession(int sessionId) throws ControlPanelException {
		if ( bus == null ) {
			throw new ControlPanelException("LeaveSession was called, but BusAttachment has not been defined");
		}
		
		return bus.leaveSession(sessionId);
	}//leaveSession
	
	/**
	 * Creates proxy bus object and returns the desired interface
	 * @param busName
	 * @param objPath
	 * @param sessionId
	 * @param busInterfaces
	 * @param iface The desired interface class
	 * @return
	 * @throws ControlPanelException if failed to register signal handler
	 */
	public ProxyBusObject getProxyObject(String busName, String objPath, int sessionId, Class<?>[] busInterfaces) throws ControlPanelException {
		if ( bus == null ) {
			throw new ControlPanelException("The BusAttachment is not defined");
		}
		
		ProxyBusObject proxyObj = bus.getProxyBusObject(busName, objPath, sessionId, busInterfaces);
		return proxyObj;
	}
	
	/**
	 * Shutdown the connection manager
	 */
	public void shutdown() {
		Log.d(TAG, "Shutdown the ConnectionMgr service");
		
		if ( handlerThread != null ) {
			handlerThread.getLooper().quit();
			handlerThread = null;
			handler       = null;
		}
		
		bus = null;
	}//shutdown
	

	//================= PRIVATE ==================//
	
    /**
     * Creates SessionOpts
     * @return SessionOpts to be used to advertise service and ot join session
     */
	private SessionOpts getSessionOpts(){
		SessionOpts sessionOpts   = new SessionOpts();
	    sessionOpts.traffic       = SessionOpts.TRAFFIC_MESSAGES;   // Use reliable message-based communication to move data between session endpoints
	    sessionOpts.isMultipoint  = false;                          // A session is multi-point if it can be joined multiple times 
	    sessionOpts.proximity     = SessionOpts.PROXIMITY_ANY;      // Holds the proximity for this SessionOpt
	    sessionOpts.transports    = SessionOpts.TRANSPORT_ANY;      // Holds the allowed transports for this SessionOpt
	    return sessionOpts;
	}//getSessionOpts
}


