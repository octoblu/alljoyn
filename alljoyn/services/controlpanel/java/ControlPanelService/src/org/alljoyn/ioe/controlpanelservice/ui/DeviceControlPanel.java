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

import java.util.LinkedList;
import java.util.List;

import org.alljoyn.ioe.controlpanelservice.ControlPanelCollection;
import org.alljoyn.ioe.controlpanelservice.ControlPanelException;
import org.alljoyn.ioe.controlpanelservice.ControlPanelService;
import org.alljoyn.ioe.controlpanelservice.ControllableDevice;
import org.alljoyn.ioe.controlpanelservice.Unit;
import org.alljoyn.ioe.controlpanelservice.communication.ConnectionManager;
import org.alljoyn.ioe.controlpanelservice.communication.IntrospectionNode;
import org.alljoyn.ioe.controlpanelservice.communication.UIWidgetSignalHandler;

import android.util.Log;

/**
 * Represents a device control panel. <br>
 * This class provides methods of receiving a root UI container and its label 
 */
public class DeviceControlPanel {
	private static final String TAG = "cpan" + DeviceControlPanel.class.getSimpleName();
	
	/**
	 * The device this control panel belongs to
	 */
	private ControllableDevice device;

	/**
	 * The functional unit that this control panel belongs to
	 */
	private Unit unit;
	
	/**
	 * The {@link ControlPanelCollection} the device belongs to
	 */
	private ControlPanelCollection collection;
	
	/**
	 * The remote object identifier of this control panel
	 */
	private String objPath;
	
	/**
	 * The IETF standard language tag
	 */
	private String language;
	
	/**
	 * The signal handler objects of this ControlPanel
	 */
	private List<UIWidgetSignalHandler> widgetSignalHandlers;
	
	/**
	 * The listener of the events that happen on the widgets of this {@link DeviceControlPanel} 
	 */
	private ControlPanelEventsListener eventsListener;
	
	/**
	 * Constructor
	 * @param device
	 */
	public DeviceControlPanel(ControllableDevice device, Unit unit, ControlPanelCollection collection, String objPath, String language) {
		this.device 	= device;
		this.objPath 	= objPath;
		this.unit 	    = unit;
		this.collection = collection;
		this.language   = language;
		
		this.widgetSignalHandlers = new LinkedList<UIWidgetSignalHandler>(); 
	}
	
	/**
	 * @return The functional unit of the device
	 */
	public Unit getUnit() {
		return unit;
	}//getUnit

	/**
	 * @return {@link DeviceControlPanel} object path
	 */
	public String getObjPath() {
		return objPath;
	}//getObjPath
	
	/**
	 * @return The IETF language tag
	 */
	public String getLanguage() {
		return language;
	}//getLanguage
	
	/**
	 * @return The {@link ControllableDevice} this {@link DeviceControlPanel} belongs to
	 */
	public ControllableDevice getDevice() {
		return device;
	}//getDevice

	/**
	 * @return The {@link ControlPanelCollection} the device belongs to
	 */
	public ControlPanelCollection getCollection() {
		return collection;
	}

	/**
	 * @return The events listener that is used by this {@link DeviceControlPanel}
	 */
	public synchronized ControlPanelEventsListener getEventsListener() {
		return eventsListener;
	}

	/**
	 * @param listener The listener of the events that happen on the UI widgets of this {@link DeviceControlPanel}
	 * @return UIElement of either {@link ContainerWidget} or {@link AlertDialogWidget}. If failed to build the element, NULL is returned
	 * @throws ControlPanelException
	 */
	public synchronized UIElement getRootElement(ControlPanelEventsListener listener) throws ControlPanelException {
		
		if ( listener == null ) {
			throw new ControlPanelException("Received an undefined ControlPanelEventsListener");
		}
		
		this.eventsListener = listener;
		
		Integer sessionId = device.getSessionId();
		
		if ( sessionId == null ) {
			Log.e(TAG, "Device: '" + device.getDeviceId() + "', getRootElement() called, but session not defined");
			throw new ControlPanelException("Session not established");
		}
		
		// Clean the previous signal handlers if exist
		cleanSignalHandlers();
		
		Log.i(TAG, "GetRootElement was called, handling...");

		IntrospectionNode introNode;
		
		try {
			introNode = new IntrospectionNode(objPath);
			introNode.parse(ConnectionManager.getInstance().getBusAttachment(), device.getSender(), sessionId);
		} catch (Exception e) {
			String msg = "Failed to introspect the path '" + objPath + "', Error: '" + e.getMessage() + "'"; 
			Log.e(TAG, msg);
			throw new ControlPanelException(msg);
		}
		
		List<String> interfaces = introNode.getInterfaces();
		Log.v(TAG, "For requested objPath: '" + objPath + "', the found interfaces are: '" + interfaces + "'");
		Log.d(TAG, "Search for Container or Dialog interface");
		
		if ( !WidgetFactory.isInitialized() ) {
			Log.e(TAG, "Failed to initialize the WidgetFactory, returning NULL");
			return null;
		}
		
		for (String ifName : interfaces) {
			if ( !ifName.startsWith(ControlPanelService.INTERFACE_PREFIX + ".") ) {
				Log.v(TAG, "Found not a ControlPanel interface: '" + ifName + "'");
				continue;
			}
			
			//Check the ControlPanel interface
			WidgetFactory widgFactory = WidgetFactory.getWidgetFactory(ifName);
			if ( widgFactory == null ) {
				Log.e(TAG, "Received an unknown ControlPanel interface: '" + ifName + "', return NULL");
				return null;
			}
			
			if ( widgFactory.isTopLevelObj() ) {
				Log.d(TAG, "Found the top level interface: '" + ifName + "', creating widgets...");
				return widgFactory.create(objPath, this, introNode.getChidren());
			}
		}//for::interfaces
		
		return null;
	}//getRootContainer

	/**
	 * Cleans the object resources
	 */
	public synchronized void release() {
		Log.d(TAG, "Cleaning the DeviceControlPanel, objPath: '" + objPath + "'");
		try {
			cleanSignalHandlers();
		}
		catch (ControlPanelException cpe) {
			Log.e(TAG, "Failed to unregister a signal handler from the bus");
		}
		
		eventsListener = null;
	}//release
	
	/**
	 * Adds the UI widget signal handler to the signal handlers list
	 * @param signalHandler
	 */
	synchronized void  addSignalHandler(UIWidgetSignalHandler signalHandler) {
		this.widgetSignalHandlers.add(signalHandler);
	}//addSignalHandler

	/**
	 * Unregister the signal handlers  
	 * @throws ControlPanelException Is thrown if failed to unregister a signal handler
	 */
	private void cleanSignalHandlers() throws ControlPanelException {
		if ( widgetSignalHandlers.size() > 0 ) {
			Log.d(TAG, "Found the previous signal handlers, unregistering them from the bus");			
			for (UIWidgetSignalHandler handler : widgetSignalHandlers) {
				handler.unregister();
			}
			widgetSignalHandlers.clear();
		}//if :: handler size > 0
	}//cleanSignalHandlers
}
