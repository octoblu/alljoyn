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

import java.lang.reflect.InvocationTargetException;
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
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.AlertDialogSuper;

import android.util.Log;

public class AlertDialogWidget extends UIElement {
    private static final String TAG = "cpan" + AlertDialogWidget.class.getSimpleName();
    
    /**
     * The class represents a dialog button 
     */
    public static class DialogButton {
    	/**
    	 * Action label
    	 */
    	private String label;
    	
    	/**
    	 * The method that should be executed when exec called
    	 */
    	private Method bindMethod;
    	
    	/**
    	 * The alert dialog on which to call the bind method
    	 */
    	private AlertDialogWidget alertDialog;
    	
    	/**
    	 * Constructor
    	 */
    	private DialogButton(String label, Method bindMethod, AlertDialogWidget alertDialog) {
    		this.label    	   = label;
    		this.bindMethod    = bindMethod;
    		this.alertDialog   = alertDialog;
    	}
    	
    	/**
    	 * @return Get the button label
    	 */
    	public String getLabel() {
			return label;
		}

		/**
    	 * Execute the action
    	 * @throws ControlPanelException
    	 */
    	public void exec() throws ControlPanelException {
    		try {
				bindMethod.invoke(alertDialog);
    		}
    		catch (InvocationTargetException ite) {
    			String invokeError = ite.getTargetException().getMessage();
    			throw new ControlPanelException("DialogButton label: '" + label + "', failed to invoke the method: '" + bindMethod.getName() + "', Error: '" + invokeError + "'");
			} catch (Exception e) {
				throw new ControlPanelException("DialogButton label: '" + label + "', failed to invoke the method: '" + bindMethod.getName() + "'");
			}
    	}//exec
    }
    
    //=====================================================//
    
    //                 END OF NESTED CLASSES               // 

    //=====================================================//

    
    private static final int ENABLED_MASK = 0x01;

    /**
     * The remote AlertDialog object
     */
    private AlertDialogSuper remoteControl;
	
	/**
	 * Whether the widget is enabled
	 */
	private boolean enabled;
	
	/**
	 * The Alert dialog display message
	 */
	private String message;
	
	/**
	 * The number of the available actions
	 */
	private short numActions;
	
	/**
	 * The Alert dialog label
	 */
	private String label;
	
	/**
	 * Background color
	 */
	private Integer bgColor;
	
	/**
	 * The hints for this alert dialog
	 */
	private List<AlertDialogHintsType> hints;
	
	/**
	 * List of this alert dialog supported buttons
	 */
	private List<DialogButton> execButtons;	
	
	/**
	 * Constructor
	 * @param ifName
	 * @param objectPath
	 * @param controlPanel
	 * @param children
	 * @throws ControlPanelException
	 */
	public AlertDialogWidget(String ifName, String objectPath, DeviceControlPanel controlPanel, List<IntrospectionNode> children) throws ControlPanelException {
		super(UIElementType.ALERT_DIALOG, ifName, objectPath, controlPanel, children);
	}

	public boolean isEnabled() {
		return enabled;
	}

	/**
	 * @return Display message
	 */
	public String getMessage() {
		return message;
	}

	/**
	 * @return The number of the available actions
	 */
	public short getNumActions() {
		return numActions;
	}

	public String getLabel() {
		return label;
	}

	public Integer getBgColor() {
		return bgColor;
	}

	public List<AlertDialogHintsType> getHints() {
		return hints;
	}
	
	/**
	 * @return List of this alert dialog supported buttons
	 */
	public List<DialogButton> getExecButtons() {
		return execButtons;
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
                 new Class[]{ifClass, Properties.class}
         );
 
         Log.d(TAG, "Setting remote control AlertDialog, objPath: '" + objectPath + "'");
         properties    = proxyObj.getInterface(Properties.class);
         remoteControl = (AlertDialogSuper) proxyObj.getInterface(ifClass);
         
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

		Method alertMetaDataChanged = CommunicationUtil.getAlertDialogMetadataChanged("MetadataChanged");
    	if ( alertMetaDataChanged == null ) {
    		String msg = "AlertDialogWidget, MetadataChanged method isn't defined";
    		Log.e(TAG, msg);
    		throw new ControlPanelException(msg);
    	}
    	
        try {	
            registerSignalHander(new AlertDialogWidgetSignalHandler(this), alertMetaDataChanged);
        }
        catch(ControlPanelException cpe) {
            String msg = "Device: '" + device.getDeviceId() +
                         "', AlertDialogWidget, failed to register signal handler, Error: '" + cpe.getMessage() + "'";
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
            else if ( "OptParams".equals(propName) ) {
                Map<Short, Variant> optParams = propValue.getObject(new VariantTypeReference<HashMap<Short, Variant>>() {});
                fillOptionalParams(optParams);
            }
            else if ( "Message".equals(propName) ) {
            	message = propValue.getObject(String.class);
            }
            else if ( "NumActions".equals(propName) ) {
            	numActions = propValue.getObject(short.class);
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
         Log.d(TAG, "Test AlertDialogWidget validity - AlertDialogWidget can't has child nodes. #ChildNodes: '" + size + "'");
         if ( size > 0 ) {
             throw new ControlPanelException("The AlertDialogWidget objPath: '" + objectPath + "' is not valid, found '" + size + "' child nodes");
         }
	}//createChildWidgets

	/**
	 * Execute the action number 1
	 * @throws ControlPanelException if failed to execute the remote call
	 */
	void execAction1() throws ControlPanelException {
        Log.d(TAG, "ExecAction1 called, device: '" + device.getDeviceId() + "' objPath: '" + objectPath + "'");
       
        try {
            remoteControl.Action1();
        }
        catch(BusException be) {
             String msg = "Failed to call ExecAction1,  objPath: '" + objectPath + "', Error: '" + be.getMessage() + "'";
             Log.e(TAG, msg);
             throw new ControlPanelException(msg);
        }
	}//execAction1
	
	/**
	 * Execute the action number 2
	 * @throws ControlPanelException if failed to execute the remote call
	 */
	void execAction2() throws ControlPanelException {
        Log.d(TAG, "ExecAction2 called, device: '" + device.getDeviceId() + "' objPath: '" + objectPath + "'");
        
        try {
            remoteControl.Action2();
        }
        catch(BusException be) {
             String msg = "Failed to call ExecAction2,  objPath: '" + objectPath + "', Error: '" + be.getMessage() + "'";
             Log.e(TAG, msg);
             throw new ControlPanelException(msg);
        }
	}//execAction2
	
	/**
	 * Execute the action number 3
	 * @throws ControlPanelException if failed to execute the remote call
	 */
	void execAction3() throws ControlPanelException {
        Log.d(TAG, "ExecAction3 called, device: '" + device.getDeviceId() + "' objPath: '" + objectPath + "'");
        
        try {
            remoteControl.Action3();
        }
        catch(BusException be) {
             String msg = "Failed to call ExecAction3,  objPath: '" + objectPath + "', Error: '" + be.getMessage() + "'";
             Log.e(TAG, msg);
             throw new ControlPanelException(msg);
        }
	}//execAction2
	
	/**
    * Fill the alertDialogWidget optional parameters
    * @throws ControlPanelException if failed to read optional parameters
    */
    private void fillOptionalParams(Map<Short, Variant> optParams) throws ControlPanelException {
    	this.execButtons = new ArrayList<AlertDialogWidget.DialogButton>();
    	
        // Add optional parameters
	    Log.d(TAG, "AlertDialogWidget - scanning optional parameters");

	    for (AlertDialogWidgetEnum optKeyEnum : AlertDialogWidgetEnum.values()) {

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
						 short[] alertHints = optParam.getObject(short[].class);
						 fillAlertDialogHints(alertHints);
						break;
					 }
					 case LABEL_ACTION1: {
						 String label = optParam.getObject(String.class);
						 execButtons.add( new DialogButton(label, getActionMethodReflection("execAction1"), this) );
						 break;
					 }
					 case LABEL_ACTION2: {
						 String label = optParam.getObject(String.class);
						 execButtons.add( new DialogButton(label, getActionMethodReflection("execAction2"), this));
						 break;
					 }
					 case LABEL_ACTION3: {
						 String label = optParam.getObject(String.class);
						 execButtons.add( new DialogButton(label, getActionMethodReflection("execAction3"), this));
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
    private void fillAlertDialogHints(short[] hIds) {
         hints = new ArrayList<AlertDialogHintsType>( hIds.length );
 
         Log.v(TAG, "Searching for AlertDialog hints");
		 
		 //Fill layout hints
		 for (short hintId : hIds) {
		     AlertDialogHintsType hintType = AlertDialogHintsType.getEnumById(hintId);
		     if ( hintType != null ) {
		         Log.v(TAG, "Found AlertDialog hint: '" + hintType + "', adding");
		         hints.add(hintType);
		     }
		     else {
		         Log.w(TAG, "AlertDialog hint id: '" + hintId + "' is unknown");
		     }
		 }//hints
    }//fillAlertDialogHints

    /**
     * Return reflection of getAction1 method
     * @param methodName The name of the method to get the reflection object
     * @return Method reflection of the received methodName
     */
    private Method getActionMethodReflection(String methodName) throws ControlPanelException {
    	Method method;
    	try {
			 method = AlertDialogWidget.class.getDeclaredMethod(methodName,  (Class[])null);
		} catch (NoSuchMethodException nse) {
			throw new ControlPanelException("Failed to get reflection of the '" + methodName + "', Error: '" + nse.getMessage() + "'");
		}
    	return method;
    }//getAction1Method
}
