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
import org.alljoyn.ioe.controlpanelservice.ControlPanelService;
import org.alljoyn.ioe.controlpanelservice.communication.CommunicationUtil;
import org.alljoyn.ioe.controlpanelservice.communication.ConnectionManager;
import org.alljoyn.ioe.controlpanelservice.communication.IntrospectionNode;
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.ActionControlSuper;

import android.util.Log;

/**
 * Action widget 
 */
public class ActionWidget extends UIElement {
    private static final String TAG = "cpan" + ActionWidget.class.getSimpleName();
	
    private static final int ENABLED_MASK = 0x01;
    
    /**
     * The remote action object
     */
    private ActionControlSuper remoteControl;

	/**
	 * Action label <br>
     * Not required to be sets
	 */
	private String label;
	
	/**
	 * Action bgcolor <br>
     * Not required to be sets
	 */
	private Integer bgColor;
	
	/**
	 * Whether the action is enabled <br>
     * Not required to be sets
	 */
	private boolean enabled;
    
	/**
	 * The widget rendering hints <br>
     * Not required to be sets
	 */
	private List<ActionWidgetHintsType> hints;

	/**
	 * The alert dialog that should be presented.
	 * In this case use of Exec button is illegal
	 */
	private AlertDialogWidget alertDialog;
	
	/**
	 * Constructor
	 * @param ifName 
	 * @param objectPath
	 * @param controlPanel
	 * @param children
	 * @throws ControlPanelException
	 */
	public ActionWidget(String ifName, String objectPath, DeviceControlPanel controlPanel, List<IntrospectionNode> children) throws ControlPanelException {
		super(UIElementType.ACTION_WIDGET, ifName, objectPath, controlPanel, children);
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

	public List<ActionWidgetHintsType> getHints() {
		return hints;
	}

	public AlertDialogWidget getAlertDialog() {
		return alertDialog;
	}

	/**
	 * Call the remote device to execute its activity
	 * @throws ControlPanelException
	 */
	public void exec() throws ControlPanelException {
		if ( alertDialog != null ) {
			throw new ControlPanelException("ActionWidget objPath: '" + objectPath + "', alertDialog is defined, can't call exec");
		}
		
		Log.d(TAG, "Exec called, device: '" + device.getDeviceId() + "' objPath: '" + objectPath + "'");

		try {
			remoteControl.Exec();
		}
		catch(BusException be) {
			 String msg = "Failed to call Exec,  objPath: '" + objectPath + "', Error: '" + be.getMessage() + "'";
             Log.e(TAG, msg);
             throw new ControlPanelException(msg);
		}
	}//exec

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
 
        Log.d(TAG, "Setting remote control ActionWidget, objPath: '" + objectPath + "'");
        properties    = proxyObj.getInterface(Properties.class);
        remoteControl = (ActionControlSuper) proxyObj.getInterface(ifClass);
        
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
	protected void registerSignalHandler() {
		Method metaDataChangedMethod = CommunicationUtil.getActionMetadataChanged("MetadataChanged");
		
		if ( metaDataChangedMethod == null ) {
			Log.e(TAG, "ActionWidget, MetadataChanged method is not defined");
			return;
		}
		
        try {
        	registerSignalHander(new ActionWidgetSignalHandler(this), metaDataChangedMethod);
        }
        catch(ControlPanelException cpe) {
             String msg = "Device: '" + device.getDeviceId() +
                          "', ActionWidget, failed to register signal handler, Error: '" + cpe.getMessage() + "'";
             Log.e(TAG, msg);
             controlPanel.getEventsListener().errorOccurred(controlPanel, msg);
        }
	}//registerSignalHandler


	/**
	 * @see org.alljoyn.ioe.controlpanelservice.ui.UIElement#setProperty(java.lang.String, org.alljoyn.bus.Variant)
	 */
	@Override
	protected void setProperty(String propName, Variant propValue) throws ControlPanelException {
        try {
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
	 * @see org.alljoyn.ioe.controlpanelservice.ui.UIElement#createChildWidgets()
	 */
	@Override
	protected void createChildWidgets() throws ControlPanelException {
		int size = children.size();
		if ( size == 0 ) {
			Log.d(TAG, "ActionWidget objPath: '" + objectPath + "', doesn't have any child nodes");
			return;
		}
		
		//ActionWidget may has only AlertDialog widget as a child node
		if ( size > 1) {
			throw new ControlPanelException("ActionWidget objPath: '" + objectPath + "' has more than one child nodes: '" + size + "'");
		}		
		
		IntrospectionNode childNode = children.get(0);
		String path                 = childNode.getPath();
        List<String> interfaces     = childNode.getInterfaces();
        
        //Search for existence of the AlertDialog interface
        for (String ifName : interfaces) {
            if ( !ifName.startsWith(ControlPanelService.INTERFACE_PREFIX + ".") ) {
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
            
            //Found a known interface, check whether it's an AlertDialog
            if ( widgFactory.getElementType() == UIElementType.ALERT_DIALOG ) {
            	Log.i(TAG, "ActionWidget objPath: '" + objectPath + "', has AlertDialog objPath: '" + path + "', creating...");
            	alertDialog = new AlertDialogWidget(ifName, path, controlPanel, childNode.getChidren());
            	return;
            }  
        }//for :: interfaces
        
        throw new ControlPanelException("ActionWidget objPath: '" + objectPath + "', not found the AlertDialog interface");
	}//createChildWidgets
	
   /**
    * Fill the actionWidget optional parameters
    * @throws ControlPanelException if failed to read optional parameters
    */
    private void fillOptionalParams(Map<Short, Variant> optParams) throws ControlPanelException {
        // Add optional parameters
	    Log.d(TAG, "ActionWidget - scanning optional parameters");

	    for (ActionWidgetEnum optKeyEnum : ActionWidgetEnum.values()) {

            Variant optParam = optParams.get(optKeyEnum.ID);
			
		    if ( optParam == null ) {
		        Log.v(TAG, "OptionalParameter: '" + optKeyEnum + "', is not found");
				continue;
		    }
			
		    Log.v(TAG, "Found OptionalParameter: '" + optKeyEnum + "'");
			
		    try {
				switch (optKeyEnum) {
					 case LABEL: {
						 label = optParam.getObject(String.class);
						 break;
					 }
					 case BG_COLOR: {
						 bgColor = optParam.getObject(int.class);
						 break;
					 }
					 case HINTS: {
						 short[] actHints = optParam.getObject(short[].class);
						 setListOfActionWidgetHints(actHints);
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
     * Fill the list of ActionWidgetHint types
     * @param hIds
     * @return List of Action hints
     */
    private void setListOfActionWidgetHints(short[] hIds) {
 	    Log.v(TAG, "Scanning ActionWidgetHints");
 	    hints = new ArrayList<ActionWidgetHintsType>( hIds.length );
 	
 	    for (short hintId : hIds) {
 		    ActionWidgetHintsType hintType = ActionWidgetHintsType.getEnumById(hintId);
 		    if (hintType == null) {
 			    Log.w(TAG, "Received unrecognized ActionWidgetHint: '" + hintId + "', ignoring...");
 			    continue;
 		    }

 		    hints.add(hintType);
 	    }//for :: hIds
    }//getListOfActionWidgetHints
}
