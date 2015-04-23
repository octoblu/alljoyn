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
import java.util.Map;

import org.alljoyn.bus.BusException;
import org.alljoyn.bus.ProxyBusObject;
import org.alljoyn.ioe.controlpanelservice.communication.ConnectionManager;
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.HTTPControl;
import org.alljoyn.ioe.controlpanelservice.ui.DeviceControlPanel;

import android.util.Log;

/**
 * The class represents a functional unit of the controllable device {@link ControllableDevice} <br>
 * The unit might has a list of the {@link ControlPanelCollection}
 */
public class Unit {
	private static final String TAG = "cpan" + Unit.class.getSimpleName(); 
	
	/**
	 * The controllable device the unit belongs to
	 */
	private ControllableDevice device;
	
	/**
	 * The unit id retrieved from the object path 
	 */
	private String unitId;
	
	/**
	 * The object path to HTTPControl Interface
	 */
	private String httpControlObjPath;

	/**
	 * The Map from object path to {@link ControlPanelCollection}  
	 */
	private Map<String, ControlPanelCollection> panelCollections;

	/**
	 * The HTTPControl proxy bus object
	 */
	private ProxyBusObject proxyObj;
	
	/**
	 * The remote HTTP control object
	 */
	private HTTPControl remoteControl;
	
	/**
	 * HTTP Protocol version
	 */
	private Short version;
	
	/**
	 * Constructor
	 * @param unitId The id of the functional unit
	 */
	public Unit(ControllableDevice device, String unitId) {
		this.device = device;
		this.unitId = unitId;
		this.panelCollections = new HashMap<String,ControlPanelCollection>();
	}

	/**
	 * @return object path of the HTTPControl Interface, if the object path is defined, otherwise NULL is returned
	 */
	public String getHttpIfaceObjPath() {
		return httpControlObjPath;
	}

	/**
	 * Set the HTTP control object path
	 * @param httpControlObjPath
	 */
	void setHttpControlObjPath(String httpControlObjPath) {
		this.httpControlObjPath = httpControlObjPath;
	}

	/**
	 * @return Get unit Id
	 */
	public String getUnitId() {
		return unitId;
	}//getUnitId
	
	/**
	 * @return Get {@link ControllableDevice}
	 */
	public ControllableDevice getDevice() {
		return device;
	}//getDevice

	/**
	 * @return {@link DeviceControlPanel} collection of this functional unit
	 */
	public Collection<ControlPanelCollection> getControlPanelCollection() {
		return Collections.unmodifiableCollection(panelCollections.values());
	}//getControlPanelList
	
	/**
	 * @return The URL of this functional unit or NULL of not defined
	 * @throws ControlPanelException
	 */
	public String getRootURL() throws ControlPanelException {
		if ( httpControlObjPath == null ) {
			return null;
		}
		
    	setRemoteController();
		
		try {
			return remoteControl.GetRootURL();
		} catch (BusException be) {
			throw new ControlPanelException("Failed to receive the Root URL, Error: '" + be.getMessage() + "'");
		}
	}//getRootURL
	
	/**
	 * @return The HTTP Protocol version or NULL if the version is not defined
	 * @throws ControlPanelException if failed to get version
	 */
	public Short getHttpControlVersion() throws ControlPanelException {
		if ( httpControlObjPath == null ) {
			return null;
		}
		
		setRemoteController();
		return version;
	}//getVersion

	/**
	 * Release all the object resources
	 */
	public void release() {
		Log.d(TAG, "Cleaning the Unit name: '" + unitId + "'");
		
		if ( proxyObj != null ) {
			proxyObj.release();
		}
		
		remoteControl = null;
		version       = null;
		
		for (ControlPanelCollection coll : panelCollections.values()) {
			coll.release();
		}
		
		panelCollections.clear();
	}//release
	
	/**
	 * Creates and adds to the {@link Unit} a new {@link ControlPanelCollection} <br>
	 * If the session with a controllable device is already been established, then the created {@link ControlPanelCollection} 
	 * is filled 
	 * @param objPath The object path that identifies the {@link ControlPanelCollection}
	 * @param collName The collection name
	 * @throws ControlPanelException Is thrown if failed to create the collection
	 */
	ControlPanelCollection createControlPanelCollection(String objPath, String collName) throws ControlPanelException {
		
		ControlPanelCollection coll = panelCollections.get(collName);
		if ( coll == null ) {
			Log.i(TAG, "Received a new ControlPanelCollection Name: '" + collName + "' objPath: '" + objPath + "', creating...");
			coll = new ControlPanelCollection(device, this, collName, objPath);
			panelCollections.put(collName, coll);
		}
		else {
			Log.d(TAG, "Received a known ControlPanelCollection Name: '" + collName + "' objPath: '" + objPath + "'");
		}
		
		Integer sessionId = device.getSessionId();
		if ( sessionId != null && coll.getControlPanels().size() == 0 ) {
			Log.d(TAG, "The session with the remote device has been previously established , sid: '" + sessionId + "', filling the new collection");
			coll.retrievePanels();
		}
		
		return coll;
	}//addControlPanel

	/**
	 * Fills the {@link ControlPanelCollection} objects of this Unit <br>
	 * The method invokes {@link ControlPanelCollection#retrievePanels()} for each {@link ControlPanelCollection}
	 * @throws ControlPanelException If failed to fill the ControlPanelCollection 
	 */
	void fillControlPanelCollections() throws ControlPanelException {
		for (ControlPanelCollection coll : panelCollections.values() ) {
			coll.retrievePanels();
		}
	}//fillControlPanelCollections
	
	/**
	 * Set the remote controller of the HTTP protocol interface
	 * @throws ControlPanelException 
	 */
	private void setRemoteController() throws ControlPanelException {

		//If the proxy bus object is defined and is built with the correct bus name, then no need to execute this method
	    if ( proxyObj != null && device.getSender().equals(proxyObj.getBusName()) ) {
	    	return;
	    }
	    
	    //Release the resources of the previously created BusObject
	    if ( proxyObj != null ) {
	    	proxyObj.release();
	    }
	    
		Integer sessionId = device.getSessionId();
	    if ( sessionId == null ) {
			throw new ControlPanelException("Session is not established");
		}

		proxyObj = ConnectionManager.getInstance().getProxyObject(
		    device.getSender(),
			httpControlObjPath,
			sessionId,
			new Class<?>[]{HTTPControl.class}
        );

		remoteControl = proxyObj.getInterface(HTTPControl.class);
		  
	    try {
		    short version = remoteControl.getVersion();			
		    Log.d(TAG, "Version check for HTTP Protocol, my protocol version is: '" + HTTPControl.VERSION + "'" +
												" the remote device version is: '" + version + "'");
		    
			if ( version > HTTPControl.VERSION ) {
			    throw new ControlPanelException("Incompatible HTTPProtocol version, my protocol version is: '" + HTTPControl.VERSION + "'" +
												" the remote device version is: '" + version + "'");
			}
			
			this.version = version;
	    } catch (BusException be) {
	    	proxyObj      = null;
	    	remoteControl = null;
		    throw new ControlPanelException("Failed to call getVersion() for HTTPProtocol, objPath: '" + httpControlObjPath + "', Error: '" + be.getMessage() + "'");
		}
	}//setRemoteController
}
