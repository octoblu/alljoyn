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

package org.alljoyn.ioe.controlpanelservice.ui;

import java.lang.reflect.Method;
import java.util.List;
import java.util.Map;

import org.alljoyn.bus.BusException;
import org.alljoyn.bus.Variant;
import org.alljoyn.bus.ifaces.Properties;
import org.alljoyn.ioe.controlpanelservice.ControlPanelException;
import org.alljoyn.ioe.controlpanelservice.ControllableDevice;
import org.alljoyn.ioe.controlpanelservice.communication.IntrospectionNode;
import org.alljoyn.ioe.controlpanelservice.communication.UIWidgetSignalHandler;

import android.util.Log;

/**
 * The parent class for another UI elements  
 */
public abstract class UIElement {
	private static final String TAG = "cpan" + UIElement.class.getSimpleName();
	
	/**
	 * Element type
	 */
	protected UIElementType elementType;

	/**
	 * The interface name this element is connected to the remote device
	 */
	protected String ifName;
	
	/**
	 * The object path to the object with the UIContainer  Interface
	 */
	protected String objectPath;
	
	/**
	 * This {@link UIElement} child nodes after the introspection 
	 */
	protected List<IntrospectionNode> children;
	
	/**
	 * The proxy bus object of the remote device
	 */
	protected Properties properties;
	
	/**
	 * The {@link DeviceControlPanel} the widget belongs to
	 */
	protected DeviceControlPanel controlPanel;
	
	/**
	 * The controllable device this widget belongs to
	 */
	protected ControllableDevice device;
	
	/**
	 * The session id in which this object is connected to the remote controlled object
	 */
	protected Integer sessionId;
	
	/**
	 * The interface version
	 */
	protected short version;
	
	/**
	 * Constructor
	 * @param elementType The type of the element
	 * @param objectPath The object path to the remote object
	 * @throws ControlPanelException if failed to build the element
	 */
	public UIElement(UIElementType elementType, String ifName, String objectPath, DeviceControlPanel controlPanel, List<IntrospectionNode> children) 
												throws ControlPanelException {
		this.elementType 	= elementType;
		this.ifName 		= ifName;
		this.objectPath  	= objectPath;
		this.children 		= children;
		this.controlPanel   = controlPanel;
		this.device  	 	= controlPanel.getDevice();
		this.sessionId      = device.getSessionId();
		
		if ( this.sessionId == null ) {
			throw new ControlPanelException("Failed to create widget: '" + elementType + "', objPath: '" + objectPath + "', session not established");
		}
		
		setRemoteController();
		versionCheck();
		registerSignalHandler();
		refreshProperties();
		createChildWidgets();
	}//Constructor
	
	public UIElementType getElementType() {
		return elementType;
	}

	public String getObjectPath() {
		return objectPath;
	}
	
	/**
	 * Retrieve and set all the widget properties from the remote device
	 * @throws ControlPanelException if failed to retrieve a property data from the remote device 
	 */
	public void refreshProperties() throws ControlPanelException {
		Log.d(TAG, "Retreive the " + elementType + " properties, object path: '" + objectPath + "'");
		Map<String, Variant> props;
		
		try {
			props = properties.GetAll(ifName);
		} catch (BusException be) {
			throw new ControlPanelException("Failed to retreive properties for ifName: '" + ifName + "', Error: '" + be.getMessage() + "'");
		}
		catch (Exception e) {
			throw new ControlPanelException("Unexpected error happened on retrieving properties for ifName: '" + ifName + "', Error: '" + e.getMessage() + "'");
		}
		
		for(String propName : props.keySet() ) {
			Log.v(TAG, "Set property: '" + propName + "', object path: '" + objectPath + "'");
			setProperty(propName, props.get(propName));
		}
	}//refreshProperties
	
	/**
	 * Returns the version number of this widget interface
	 * @throws ControlPanelException if failed to call remote object
	 * @return The widget version number
	 */
	public short getVersion() {
		return version;
	}
	
	/**
	 * Performs version check of this interface version vs. the interface version of the remote device
	 * @throws ControlPanelException If the interface version of the remote device is greater than the version of this interface
	 */
	protected void versionCheck() throws ControlPanelException {
		WidgetFactory widgFactory = WidgetFactory.getWidgetFactory(ifName);
		
	    //Check the ControlPanel interface
        if ( widgFactory == null ) {
        	String msg = "Received an unknown ControlPanel interface: '" + ifName + "'";
        	Log.e(TAG, msg);
            throw new ControlPanelException(msg);
        }
		
		Class<?> ifaceClass = widgFactory.getIfaceClass();
	
		try {
   			short myVersion = ifaceClass.getDeclaredField("VERSION").getShort(short.class);
			
   	        Log.d(TAG, "Version check for interface: '" + ifName + "' my version is: '" + myVersion + "'" +
					" the remote device version is: '" + this.version + "'");
   	        
   	        if ( this.version > myVersion ) {
   	            throw new ControlPanelException("Incompatible interface version: '" + ifName + "', my interface version is: '" + myVersion + "'" +
   	                                            " the remote device interface version is: '" + this.version + "'");
   	        }
		} catch (Exception e) {
			throw new ControlPanelException("Failed to perform version check for interface: '" + ifName + "', unable to get the reflection of the VERSION field");
		}
	}//versionCheck
	
	/**
	 * Register signal handler of the given busObject and its signalHandlerMethod
	 * @param signalReceiver The object that receives the signal
	 * @param signalHandlerMethod The method that handles the signal
	 * @throws ControlPanelException Thrown if failed to register signal handler
	 */
	protected void registerSignalHander(Object signalReceiver, Method signalHandlerMethod) throws ControlPanelException {
		UIWidgetSignalHandler signalHandler = new UIWidgetSignalHandler(objectPath, signalReceiver, signalHandlerMethod, ifName);
		signalHandler.register();
		controlPanel.addSignalHandler(signalHandler);
	}//registerSignalHander
	
	/**
	 * Set remote controller of the object
	 * @throws ControlPanelException if failed to set remote control 
	 */
	protected abstract void setRemoteController() throws ControlPanelException;
	
	/**
	 * Register signal handler of the remote object
	 * @throws ControlPanelException Thrown if failed to register signal handler
	 */
	protected abstract void registerSignalHandler() throws ControlPanelException;

	/**
	 * Sets the specific property 
	 * @param propName The name of the property to be set
	 * @param propValue The property value
	 * @throws ControlPanelException If failed to set the property value
	 */
	protected abstract void setProperty(String propName, Variant propValue) throws ControlPanelException;
	
	/**
	 * Create this element child widgets
	 * @throws ControlPanelException
	 */
	protected abstract void createChildWidgets() throws ControlPanelException;
}
