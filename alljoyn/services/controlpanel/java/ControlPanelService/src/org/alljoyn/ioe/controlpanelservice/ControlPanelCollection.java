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

import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.alljoyn.bus.BusException;
import org.alljoyn.bus.ProxyBusObject;
import org.alljoyn.ioe.controlpanelservice.communication.CommunicationUtil;
import org.alljoyn.ioe.controlpanelservice.communication.ConnectionManager;
import org.alljoyn.ioe.controlpanelservice.communication.IntrospectionNode;
import org.alljoyn.ioe.controlpanelservice.communication.UIWidgetSignalHandler;
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.ControlPanel;
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.NotificationAction;
import org.alljoyn.ioe.controlpanelservice.ui.ControlPanelEventsListener;
import org.alljoyn.ioe.controlpanelservice.ui.DeviceControlPanel;

import android.util.Log;

/**
 *  ControlPanelCollection represents a family of the {@link DeviceControlPanel} objects per language that belong to the same functional {@link Unit}
 */
public class ControlPanelCollection {
	private static final String TAG = "cpan" + ControlPanelCollection.class.getSimpleName();
	
	public class NotificationActionReceiver implements NotificationAction {
		@Override
		public short getVersion() throws BusException {return 0;}

		/**
		 * @see org.alljoyn.ioe.controlpanelservice.communication.interfaces.NotificationAction#Dismiss()
		 */
		@Override
		public void Dismiss() throws BusException {
			
			Log.d(TAG, "Received NotificationAction.Dismiss() signal, objPath: '" + objectPath + "', notify ControlPanels");
			for (DeviceControlPanel panel : controlPanels.values() ) {
				ControlPanelEventsListener eventsListener = panel.getEventsListener();
				if ( eventsListener != null ) {
					eventsListener.notificationActionDismiss(panel);
				}
			}//for :: controlPanels
		}//Dismiss
		
	}
	
	//========================================================//
	
	/**
	 * The device
	 */
	private ControllableDevice device;
	
	/**
	 * The {@link Unit} the collection belongs to
	 */
	private Unit unit;
	
	/**
	 * The name of the collection
	 */
	private String name;
	
	/**
	 * The object path that represents the {@link ControlPanelCollection} 
	 */
	private String objectPath;
	
	/**
	 * The mask of the interfaces the object path implements
	 */
	private int ifaceMask;
	
	/**
	 * The version of the {@link ControlPanel} interface of the remote device
	 */
	private Short controlPanelVersion;
	
	/**
	 * The version of the {@link NotificationAction} of the remote device
	 */
	private Short notificationActionVersion;
	
	/**
	 * Signal handler of the {@link NotificationAction} interface
	 */
	private UIWidgetSignalHandler notifActSignalHandler;
	
	/**
	 * Map from the language tag to the {@link DeviceControlPanel}
	 */
	private Map<String, DeviceControlPanel> controlPanels; 
	
	/**
	 * Constructor 
	 * @param controllableDevice The {@link ControllableDevice} the collection belongs to
	 * @param unit The {@link Unit} the collection belongs to 
	 * @param collName The name of the control panel collection
	 * @param objPath The object path to introspect the control panel collection
	 */
	public ControlPanelCollection(ControllableDevice controllableDevice, Unit unit, String collName, String objPath) {
		this.device 		= controllableDevice;
		this.unit 		    = unit;
		this.name  			= collName;
		this.objectPath     = objPath;
		controlPanels       = new HashMap<String, DeviceControlPanel>();
	}
	
	/**
	 * @return The {@link ControllableDevice}
	 */
	public ControllableDevice getDevice() {
		return device;
	}

	/**
	 * @return The functional {@link Unit} the collection belongs to
	 */
	public Unit getUnit() {
		return unit;
	}

	/**
	 * @return The name of the collection
	 */
	public String getName() {
		return name;
	}

	/**
	 * @return The {@link ControlPanelCollection} object path
	 */
	public String getObjectPath() {
		return objectPath;
	}

	/**
	 * @return The version of the {@link ControlPanel} interface, NULL if failed to be set
	 */
	public Short getControlPanelVersion() {
		return controlPanelVersion;
	}

	/**
	 * @return The version of the {@link NotificationAction} interface, may be NULL if failed to be set, or the objectPath doesn't implement
	 * the {@link NotificationAction} interface
	 */
	public Short getNotificationActionVersion() {
		return notificationActionVersion;
	}

	/**
	 * @return Collection of {@link DeviceControlPanel}
	 */
	public Collection<DeviceControlPanel> getControlPanels() {
		return Collections.unmodifiableCollection(controlPanels.values());
	}//getControlPanels
	
	/**
	 * @return A set of languages of the control panels of this collection 
	 */
	public Set<String> getLanguages() {
		return Collections.unmodifiableSet(controlPanels.keySet());
	}//getLanguages
	
	/**
	 * Cleans the object resources
	 */
	public void release() {
		Log.d(TAG, "Cleaning the ControlPanelCollection: '" + name + "'");
		
		if ( notifActSignalHandler != null ) {
			try {
				notifActSignalHandler.unregister();
			} catch (ControlPanelException cpe) {
				Log.d(TAG, "Failed to unregister the NotificationAction signal handler");
			}
			notifActSignalHandler = null;
		}
		
		releaseControlPanels();
	}//release
	
	/**
	 * Creates and fills this collection with the {@link DeviceControlPanel} objects <br>
	 * The operation is done by introspection of the remote object  
	 * @throws ControlPanelException Is thrown if failed to retrieve the ControlPanel objects 
	 */
	 void retrievePanels() throws ControlPanelException {
		Log.d(TAG, "Introspecting ControlPanels for objectPath: '" + objectPath + "'");
		if ( device.getSessionId() == null ) {
			throw new ControlPanelException("The session wasn't established, can't introspect ControlPanels");
		}
		
		try {
			
			//Introspect the ControlPanelCollection child nodes
			IntrospectionNode introNode = new IntrospectionNode(objectPath);
			introNode.parseOneLevel(ConnectionManager.getInstance().getBusAttachment(), device.getSender(), device.getSessionId());
			
			//Analyze interfaces
			ifaceMask = CommunicationUtil.getInterfaceMask((String[])introNode.getInterfaces().toArray(new String[0]));
				
			//Perform version check
			checkVersion();
			releaseControlPanels();
			
			Log.d(TAG, "Fill the ControlPanelCollection, objectPath: '" + objectPath + "'");
			List<IntrospectionNode> children = introNode.getChidren();
			for ( IntrospectionNode node : children ) {
				String path = node.getPath();
				String segments[] = path.split("/");
				//Extract the language tag and replace "_" with "-" to comply the IETF standard
				String lang       = segments[ segments.length - 1 ].replace("_", "-");
				
				Log.v(TAG, "Introspected ControlPanel lang: '" + lang + "', objPath: '" + objectPath + "'");
				DeviceControlPanel panel = new DeviceControlPanel(device, unit, this, path, lang);
				controlPanels.put(lang, panel);
			}//for :: children
			
		} catch(Exception e) {		
			Log.e(TAG, "Failed to introspect the ControlPanels for objectPath: '" + objectPath + "', Error: '" + e.getMessage() + "'");
			throw new ControlPanelException("Failed to introspect the ControlPanels");
		}
	}//introspectPanels
	
	/**
	 * Checks whether the given objectPath implements the {@link NotificationAction} interface and registers the signal handler
	 * @throws ControlPanelException
	 */
	void handleNotificationAction() throws ControlPanelException {
		
		if ( !CommunicationUtil.maskIncludes(ifaceMask, NotificationAction.ID_MASK)) {
			throw new ControlPanelException("The received objectPath: '" + objectPath + "' doesn't implement the NotificationAction protocol");
		}
		
		if ( notificationActionVersion == null ) {
			throw new ControlPanelException("Undefined the NotificationAction protocol version");
		}
		
		Log.d(TAG, "Registering NotificationAction signal handler, objPath: '" + objectPath + "'");
		Method method  = CommunicationUtil.getNotificationActionDismissSignal("Dismiss");
		if ( method == null ) {
			throw new ControlPanelException("Failed to register the NotificationAction.Dismiss signal, no reflection method was found");
		}
		
		notifActSignalHandler  = new UIWidgetSignalHandler(objectPath, new NotificationActionReceiver(), method, NotificationAction.IFNAME);
		notifActSignalHandler.register();
	}//handleNotificationAction
	
	/**
	 * Performs version comparison for ControlPanel and NotificationAction interfaces (if is relevant)
	 * @throws ControlPanelException
	 */
	private void checkVersion() throws ControlPanelException {
		
		boolean isControlPanel = CommunicationUtil.maskIncludes(ifaceMask, ControlPanel.ID_MASK);
		boolean isNotifAction  = CommunicationUtil.maskIncludes(ifaceMask, NotificationAction.ID_MASK); 
		
		List<Class<?>> interfaces = new ArrayList<Class<?>>(2);
		
		if ( isControlPanel ) {
			interfaces.add(ControlPanel.class);
		}
		if ( isNotifAction ) {
			interfaces.add(NotificationAction.class);
		}
		
		ProxyBusObject proxyObj = ConnectionManager.getInstance().getProxyObject(
		    device.getSender(),
			objectPath,
			device.getSessionId(),
			(Class<?>[])interfaces.toArray(new Class<?>[0])
        );
		
		if ( isControlPanel ) {
			
			Log.d(TAG, "The objPath: '" + objectPath + "' implements the ControlPanel interface, performing version check");
			
		    try {
		    	ControlPanel remoteControl = proxyObj.getInterface(ControlPanel.class);
			    short version = remoteControl.getVersion();			
			    Log.d(TAG, "Version check for ControlPanel interface, my protocol version is: '" + ControlPanel.VERSION + "'" +
													" the remote device version is: '" + version + "'");
			    
				if ( version > ControlPanel.VERSION ) {
				    throw new ControlPanelException("Incompatible ControlPanel version, my protocol version is: '" + ControlPanel.VERSION + "'" +
													" the remote device version is: '" + version + "'");
				}
				
				this.controlPanelVersion = version;
		    } catch (BusException be) {
			    throw new ControlPanelException("Failed to call getVersion() for ControlPanel interface, objPath: '" + objectPath + "', Error: '" + be.getMessage() + "'");
			}
		    
		}//if :: isControlPanel
	    
	    if ( isNotifAction ) {
	    	
	    	Log.d(TAG, "The objPath: '" + objectPath + "' implements the NotificationAction interface, performing version check");
	    	
		    try {
		    	NotificationAction remoteControl = proxyObj.getInterface(NotificationAction.class);
			    short version = remoteControl.getVersion();			
			    Log.d(TAG, "Version check for NotificationAction interface, my protocol version is: '" + NotificationAction.VERSION + "'" +
													" the remote device version is: '" + version + "'");
			    
				if ( version > NotificationAction.VERSION ) {
				    Log.e(TAG,"Incompatible NotificationAction version, my protocol version is: '" + NotificationAction.VERSION + "'" +
													" the remote device version is: '" + version + "'");
				    this.notificationActionVersion = null;
				}
				else {		
					this.notificationActionVersion = version;
				}
		    } catch (BusException be) {
			    Log.e(TAG, "Failed to call getVersion() for NotificationAction interface, objPath: '" + objectPath + "', Error: '" + be.getMessage() + "'");
			    this.notificationActionVersion = null;
			}
	    	
	    }//if :: notifAction
	    
	}//setRemoteController
	
	/**
	 * Cleans the DeviceControlPanel collection
	 */
	private void releaseControlPanels() {
		for (DeviceControlPanel panel : controlPanels.values() ) {
			panel.release();
		}
		
		controlPanels.clear();
	}//releaseControlPanels
}
