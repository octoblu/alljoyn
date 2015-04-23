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
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.alljoyn.bus.BusException;
import org.alljoyn.bus.ProxyBusObject;
import org.alljoyn.bus.Variant;
import org.alljoyn.bus.VariantTypeReference;
import org.alljoyn.bus.ifaces.Properties;
import org.alljoyn.ioe.controlpanelservice.ControlPanelException;
import org.alljoyn.ioe.controlpanelservice.ControlPanelService;
import org.alljoyn.ioe.controlpanelservice.communication.CommunicationUtil;
import org.alljoyn.ioe.controlpanelservice.communication.ConnectionManager;
import org.alljoyn.ioe.controlpanelservice.communication.IntrospectionNode;
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.ContainerSuper;

import android.util.Log;

/**
 * Container widget
 */
public class ContainerWidget extends UIElement {
	private static final String TAG = "cpan" + ContainerWidget.class.getSimpleName();
	
	private static final int ENABLED_MASK = 0x01;
	
	/**
	 * The remote container object
	 */
	private ContainerSuper remoteControl;
		
	/**
	 * The list of UI elements  <br>
	 * Not required to be set
	 */
	private List<UIElement> elements;
	 
    /**
     * The layout hints  <br>
     * Not required to be set
     */
    private List<LayoutHintsType> layoutHints;
 
    /**
     * The label <br>
     * Not required to be set
     */
    private String label;
 
    /**
     * BGColor <br>
     * Not required to be set
     */
    private Integer bgColor;
     
    /**
     * Indicates whether the UI element is enabled <br>
     * Default is true <br>
     * Not required to be set
     */
    private boolean enabled;
	
    /**
     * Constructor
     * @param ifName
     * @param objectPath
     * @param controlPanel
     * @param children
     * @throws ControlPanelException
     */
	public ContainerWidget(String ifName, String objectPath, DeviceControlPanel controlPanel, List<IntrospectionNode> children) throws ControlPanelException {
		super(UIElementType.CONTAINER, ifName, objectPath, controlPanel, children);
	}
			
	public List<UIElement> getElements() {
		return elements;
	}

	public List<LayoutHintsType> getLayoutHints() {
		return layoutHints;
	}

	public String getLabel() {
		return label;
	}

	public Integer getBgColor() {
		return bgColor;
	}

	public boolean isEnabled() {
		return enabled;
	}
	
	//===========================================//

	/**
	 * @see org.alljoyn.ioe.controlpanelservice.ui.UIElement#setProperty(java.lang.String, org.alljoyn.bus.Variant)
	 */
	@Override
	protected void setProperty(String propName, Variant propValue) throws ControlPanelException {
		try{
			if ( "States".equals(propName) ) {
				int states = propValue.getObject(int.class);
				enabled    =  (states & ENABLED_MASK) == ENABLED_MASK;
			}
			else if ( "OptParams".equals(propName) ) {
				Map<Short, Variant> optParams = propValue.getObject(new VariantTypeReference<HashMap<Short, Variant>>() {});
				fillOptionalParams(optParams);
			}
		}
		catch(BusException be) {
			throw new ControlPanelException("Failed to unmarshal the property: '" + propName + "', Error: '" + be.getMessage() + "'");
		}
	}//setProperty
	
	/**
	 * @see org.alljoyn.ioe.controlpanelservice.ui.UIElement#setRemoteController()
	 */
	@Override
	protected void setRemoteController() throws ControlPanelException {
		WidgetFactory widgetFactory = WidgetFactory.getWidgetFactory(ifName);

		if ( widgetFactory == null ) {
			String msg = "Received an unrecognized interface name: '" + ifName + "'";
			Log.e(TAG, msg);
			throw new ControlPanelException(msg);
		}
		
		Class<?> ifClass = widgetFactory.getIfaceClass();
		
	    ProxyBusObject proxyObj = ConnectionManager.getInstance().getProxyObject(
	            device.getSender(),
	            objectPath,
	            sessionId,
	            new Class[]{ifClass, Properties.class}
	    ); 
	    
	    Log.d(TAG, "Setting remote control ContainerWidget, objPath: '" + objectPath + "'");
	    properties    = proxyObj.getInterface(Properties.class);
	    remoteControl = (ContainerSuper) proxyObj.getInterface(ifClass);
	    
        try {
        	this.version = remoteControl.getVersion();
		} catch (BusException e) {
			String msg = "Failed to call getVersion for element: '" + elementType + "'";
			Log.e(TAG, msg);
			throw new ControlPanelException(msg);
		}
	    
	}//newSessionEstablished

	/**
	 * @see org.alljoyn.ioe.controlpanelservice.ui.UIElement#registerSignalHandler()
	 */
	@Override
	protected void registerSignalHandler() throws ControlPanelException {
		Method containerMetadataChanged = CommunicationUtil.getContainerMetadataChanged("MetadataChanged"); 
		if ( containerMetadataChanged == null ) {
    		String msg = "ContainerWidget, MetadataChanged method isn't defined";
    		Log.e(TAG, msg);
    		throw new ControlPanelException(msg);
		}
		
		try {
			registerSignalHander(new ContainerWidgetSignalHandler(this), containerMetadataChanged);
		}
		catch(ControlPanelException cpe) {
			String msg = "Device: '" + device.getDeviceId() + 
					     "', ContainerWidget, failed to register signal handler, Error: '" + cpe.getMessage() + "'";
			Log.e(TAG, msg);
			controlPanel.getEventsListener().errorOccurred(controlPanel, msg);
		}
	}//registerSignalHandler

    /** 
     * Fill container optional parameters
     * @param optParams
     * @throws ControlPanelException if failed to read optional parameters 
     */
    private void fillOptionalParams(Map<Short, Variant> optParams) throws ControlPanelException {
        // Add optional parameters
        Log.d(TAG, "Container - scanning optional parameters");

        for (ContainerWidgetEnum optKeyEnum : ContainerWidgetEnum.values()) {

            Variant optParam = optParams.get(optKeyEnum.ID);
       
            if ( optParam == null ) { 
                Log.v(TAG, "OptionalParameter: '" + optKeyEnum + "', is not found");
                continue;
            }   
       
            Log.v(TAG, "Found OptionalParameter: '" + optKeyEnum + "'");
       
            try {
                switch (optKeyEnum) {
                    case LABEL: {
                        label   = optParam.getObject(String.class);
                        break;
                    }   
                    case BG_COLOR: {
                        bgColor = optParam.getObject(int.class);
                        break;
                    }   
                    case LAYOUT_HINTS: {
                        short[] layoutHints = optParam.getObject( short[].class );
                        fillLayoutHints(layoutHints);
                        break;
                    }   
                }//switch
            }   
            catch(BusException be) {
                throw new ControlPanelException("Failed to unmarshal optional parameters, Error: '" + be.getMessage() + "'");
            }
        }//for
    }//fillOptionalParams
	
    /**
     * Iterate over the layout hints and fill it
     * @param hIds
     */
    private void fillLayoutHints(short[] hIds) {
         layoutHints = new ArrayList<LayoutHintsType>( hIds.length );
 
         Log.v(TAG, "Searching for layoutHints");
		 
		 //Fill layout hints
		 for (short hintId : hIds) {
		     LayoutHintsType hintType = LayoutHintsType.getEnumById(hintId);
		     if ( hintType != null ) {
		         Log.v(TAG, "Found layout hint: '" + hintType + "', adding");
		         layoutHints.add(hintType);
		     }
		     else {
		         Log.w(TAG, "Layout hint id: '" + hintId + "' is unknown");
		     }
		 }//hints
    }//fillLayoutHints
    
	/**
	 * @see org.alljoyn.ioe.controlpanelservice.ui.UIElement#createChildWidgets()
	 */
	@Override
	protected void createChildWidgets() {
		Log.d(TAG, "Device: '" + device.getDeviceId() + "', iterate over the child elements");
		
		elements = new LinkedList<UIElement>();
		
		for ( IntrospectionNode childNode : children ) {
			String path             = childNode.getPath();
		    List<String> interfaces = childNode.getInterfaces();
			Log.d(TAG, "Device: '" + device.getDeviceId() + "' found child node objPath: '" + path + "', interfaces: '" + interfaces + "'");
			
	        for (String ifName : interfaces) {
	        	try {
		        	
		            if ( !ifName.startsWith(ControlPanelService.INTERFACE_PREFIX  + ".") ) {
		                 Log.v(TAG, "Found not a ControlPanel interface: '" + ifName + "'");
		                 continue;
		            }
		             
		            //Check the ControlPanel interface
		            WidgetFactory widgFactory = WidgetFactory.getWidgetFactory(ifName);
		            if ( widgFactory == null ) {
		            	String msg = "Received an unknown ControlPanel interface: '" + ifName + "'";
		            	Log.e(TAG, msg);
		                throw new ControlPanelException(msg);
		            }
		            
		            UIElement childElement = widgFactory.create(path, controlPanel, childNode.getChidren());
		            elements.add(childElement);
		            
	            }//try
	    		catch (Exception e) {
	    			 Log.w(TAG, "An error occurred during creation the Object: '" + path + "', device: '" + device.getDeviceId() + "'");
					 controlPanel.getEventsListener().errorOccurred(controlPanel, e.getMessage());
					 try {
						ErrorWidget errorWidget = new ErrorWidget(UIElementType.ERROR_WIDGET, ifName, path, controlPanel, childNode.getChidren());
						errorWidget.setError(e.getMessage());
						elements.add(errorWidget);
					} catch (Exception ex) {
						//This should never happen, because ErrorWidget never throws an exception
						Log.w(TAG, "A failure has occurred in creation the ErrorWidget");
					}
			    }
	        	
	        }//for :: interfaces
		}//for :: children
	}//createChildWidgets
}
