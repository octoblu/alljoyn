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
import java.util.List;
import java.util.Map;

import org.alljoyn.bus.BusException;
import org.alljoyn.bus.ProxyBusObject;
import org.alljoyn.bus.Variant;
import org.alljoyn.bus.VariantTypeReference;
import org.alljoyn.bus.ifaces.Properties;
import org.alljoyn.ioe.controlpanelservice.ControlPanelException;
import org.alljoyn.ioe.controlpanelservice.communication.CommunicationUtil;
import org.alljoyn.ioe.controlpanelservice.communication.ConnectionManager;
import org.alljoyn.ioe.controlpanelservice.communication.IntrospectionNode;
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.Label;

import android.util.Log;

public class LabelWidget extends UIElement {
	private static final String TAG = LabelWidget.class.getSimpleName();
	
	private static final int ENABLED_MASK = 0x01;
	
    /**
     * The remote property object
     */
	private Label remoteControl;
	
	/**
	 * Whether the label is enabled
	 */
	private boolean enabled;
	
	/**
	 * The label
	 */
	private String label;
	
	/**
	 * The label widget bgcolor
	 */
	private Integer bgColor;
	
	/**
	 * The hints for this Label Widget
	 */
	private List<LabelWidgetHintsType> hints;
	

	/**
	 * Constructor
	 * @param ifName
	 * @param objectPath
	 * @param controlPanel
	 * @param children
	 * @throws ControlPanelException
	 */
	public LabelWidget(String ifName, String objectPath, DeviceControlPanel controlPanel, List<IntrospectionNode> children) throws ControlPanelException {
		super(UIElementType.LABEL_WIDGET, ifName, objectPath, controlPanel, children);
	}
	
	public boolean isEnabled() {
		return enabled;
	}

	public String getLabel() {
		return label;
	}

	public Integer getBgColor() {
		return bgColor;
	}
	
	public List<LabelWidgetHintsType> getHints() {
		return hints;
	}

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
                new Class[]{ ifClass, Properties.class }
        );

        Log.d(TAG, "Setting remote control LabelWidget, objPath: '" + objectPath + "'");
        properties    = proxyObj.getInterface(Properties.class);
        remoteControl = (Label) proxyObj.getInterface(ifClass);
        
        try {
        	this.version = remoteControl.getVersion();
		} catch (BusException e) {
			String msg = "Failed to call getVersion for element: '" + elementType + "'";
			Log.e(TAG, msg);
			throw new ControlPanelException(msg);
		}
        
	}//setRemoteController

	/**
	 * @see org.alljoyn.ioe.controlpanelservice.ui.UIElement#registerSignalHandler()
	 */
	@Override
	protected void registerSignalHandler() throws ControlPanelException {
		Method labelMetadataChanged = CommunicationUtil.getLabelWidgetMetadataChanged("MetadataChanged");
		if ( labelMetadataChanged == null ) {
    		String msg = "LabelWidget, MetadataChanged method isn't defined";
    		Log.e(TAG, msg);
    		throw new ControlPanelException(msg);
		}
		
        try {
        	registerSignalHander(new LabelWidgetSignalHandler(this), labelMetadataChanged);
        }
        catch(ControlPanelException cpe) {
            String msg = "Device: '" + device.getDeviceId() +
                         "', LabelWidget, failed to register signal handler, Error: '" + cpe.getMessage() + "'";
            Log.e(TAG, msg);
            controlPanel.getEventsListener().errorOccurred(controlPanel, msg);
        }
	}//registerSignalHandler
	

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
            else if ( "Label".equals(propName) ) {
            	label = propValue.getObject(String.class);
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
	 * @see org.alljoyn.ioe.controlpanelservice.ui.UIElement#createChildWidgets()
	 */
	@Override
	protected void createChildWidgets() throws ControlPanelException {
        int size = children.size();
        Log.d(TAG, "Test LabelWidget validity - LabelWidget can't has child nodes. #ChildNodes: '" + size + "'");
        if ( size > 0 ) {
            throw new ControlPanelException("The LabelWidget objPath: '" + objectPath + "' is not valid, found '" + size + "' child nodes");
        }
	}//createChildWidgets
	
    /**
     * Fill LabelWidget optional parameters
     * @param optParams
     * @throws ControlPanelException if failed to read optional parameters 
     */
    private void fillOptionalParams(Map<Short, Variant> optParams) throws ControlPanelException {
        // Add optional parameters
        Log.d(TAG, "LabelWidget - scanning optional parameters");

        for (LabelWidgetEnum optKeyEnum : LabelWidgetEnum.values()) {

            Variant optParam = optParams.get(optKeyEnum.ID);

            if ( optParam == null ) {
                Log.v(TAG, "OptionalParameter: '" + optKeyEnum + "', is not found");
                continue;
            }

            Log.v(TAG, "Found OptionalParameter: '" + optKeyEnum + "'");

            try {
                switch (optKeyEnum) {
                    case BG_COLOR: {
                        bgColor = optParam.getObject(int.class);
                        break;
                    }
                    case HINTS: {
   					    short[] labelHints = optParam.getObject(short[].class);
					    fillLabelHints(labelHints);
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
     * Iterate over the alert dialog hints and fill it
     * @param hIds
     */
    private void fillLabelHints(short[] hIds) {
         hints = new ArrayList<LabelWidgetHintsType>( hIds.length );
 
         Log.v(TAG, "Searching for LabelWidget hints");
		 
		 //Fill layout hints
		 for (short hintId : hIds) {
		     LabelWidgetHintsType hintType = LabelWidgetHintsType.getEnumById(hintId);
		     if ( hintType != null ) {
		         Log.v(TAG, "Found LabelWidget hint: '" + hintType + "', adding");
		         hints.add(hintType);
		     }
		     else {
		         Log.w(TAG, "LabelWidget hint id: '" + hintId + "' is unknown");
		     }
		 }//hints
    }//fillLabelHints

}
