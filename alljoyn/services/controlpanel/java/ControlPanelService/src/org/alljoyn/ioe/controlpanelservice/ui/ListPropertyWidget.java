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
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.ListPropertyControlSuper;
import org.alljoyn.ioe.controlpanelservice.ui.ajstruct.ListPropertyWidgetRecordAJ;

import android.util.Log;

/**
 * List of Property Widget
 */
public class ListPropertyWidget extends UIElement {
	private static final String TAG = "cpan" + ListPropertyWidget.class.getSimpleName();

	/**
	 * The Record object of the {@link ListPropertyWidget}
	 */
	public static class Record {
		
		/**
		 * Record Id
		 */
		private short recordId;
		
		/**
		 * Record label 
		 */
		private String label;
		
		/**
		 * Constructor
		 * @param recordId
		 * @param label
		 */
		private Record(short recordId, String label) {
			this.recordId  	= recordId;
			this.label      = label;
		}

		public short getRecordId() {
			return recordId;
		}

		public String getLabel() {
			return label;
		}

		@Override
		public String toString() {
			return recordId + ":'" + label + "'"; 
		}//toString
		
	}
	
     //========================================//
	
    private static final int ENABLED_MASK = 0x01;
	
    /**
     * The remote list of property object
     */
    private ListPropertyControlSuper remoteControl;
           
    /**
     * Whether the ui object is enabled
     */
    private boolean enabled;
	
    /**
     * The UI widget label <br>
     * An optional field
     */
    private String label;
    
    /**
     * The background color  <br>
     * An optional field
     */
    private Integer bgColor;
    
    /**
     * The widget rendering hints
     */
    private List<ListPropertyWidgetHintsType> hints;
    
    /**
     * The Input Form Container widget introspection node to be used to create a {@link ContainerWidget}
     */
    private IntrospectionNode containerIntroNode;
    
    /**
     * The Input Form Container Widget interface
     */
    private String containerIfname;
    
    /**
     * The input form container
     */
    private ContainerWidget inputFormContainer;
    
        /**
         * Constructor
         * @param ifName
         * @param objectPath
         * @param controlPanel
         * @param children
         * @throws ControlPanelException
         */
	public ListPropertyWidget(String ifName, String objectPath, DeviceControlPanel controlPanel, List<IntrospectionNode> children) throws ControlPanelException {
	    super(UIElementType.LIST_PROPERTY_WIDGET, ifName, objectPath, controlPanel, children);
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

	public List<ListPropertyWidgetHintsType> getHints() {
	    return hints;
	}
	
	/**
	 * @return Returns a list of the {@link Record} objects. <br>
	 * Each object has a necessary data to control the input form
	 * @throws ControlPanelException If failed to read the remote property value 
	 */
	public List<Record> getValue() throws ControlPanelException {
       	    Log.d(TAG, "getValue() called, device: '" + device.getDeviceId() + "' objPath: '" + objectPath + "'");
	    List<Record> records;
		
	    try {
		    ListPropertyWidgetRecordAJ[] recordsAJ = remoteControl.getValue();
		    records = createListOfRecords(recordsAJ);
	    }
	    catch(BusException be) {
 	        String msg = "Failed to call getValue(),  objPath: '" + objectPath + "', Error: '" + be.getMessage() + "'";
                Log.e(TAG, msg); 
	        throw new ControlPanelException(msg);
	    }
		
	    return records;
	}//getValue

	/**
	 * Prepares the input form for adding a new record to the list. <br>
	 * Complete filling the added form by calling the {@link ListPropertyWidget#confirm()} or the {@link ListPropertyWidget#cancel()} methods 
	 * @return A form {@link ContainerWidget} to be filled
	 * @throws ControlPanelException If failed to call the add method on the remote object
	 */
	public ContainerWidget add() throws ControlPanelException {
	    Log.d(TAG, "Add() called, device: '" + device.getDeviceId() + "' objPath: '" + objectPath + "'");
		
	    try {
	        remoteControl.Add();
	    }
	    catch (BusException be) {
		 String msg = "Failed to call Add(),  objPath: '" + objectPath + "', Error: '" + be.getMessage() + "'";
                 Log.e(TAG, msg); 
	         throw new ControlPanelException(msg);
	    }
		
	    if ( inputFormContainer == null ) {
	        createContainerWidget();
	    }
		
	    return inputFormContainer;
	}//add
	
	/**
	 * Prepares the form for view the record prior to the delete action.  <br>
	 * Complete deleting the form by calling the {@link ListPropertyWidget#confirm()} or the {@link ListPropertyWidget#cancel()} methods
	 * @param recordId The record id of the deleted form
	 * @throws ControlPanelException If failed to call the delete method on the remote object
	 */
	public void delete(short recordId) throws ControlPanelException {
	     Log.d(TAG, "Delete() called, device: '" + device.getDeviceId() + "' objPath: '" + objectPath + "'");
		
	     try {
	         remoteControl.Delete(recordId);
	     }
	     catch (BusException be) {
                 String msg = "Failed to call Delete(),  objPath: '" + objectPath + "', Error: '" + be.getMessage() + "'";
                 Log.e(TAG, msg); 
	         throw new ControlPanelException(msg);
             }
	}//delete
	
	/**
	 * Prepare the display form to view the record identified by the record id.
	 * @param recordId The record id of the shown form
	 * @return A form {@link ContainerWidget} to be shown
	 * @throws ControlPanelException If failed to call the view method on the remote object
	 */
	public ContainerWidget view(short recordId) throws ControlPanelException {
	    Log.d(TAG, "View() called, device: '" + device.getDeviceId() + "' objPath: '" + objectPath + "'");
		
	    try {
		remoteControl.View(recordId);
	    }
	    catch (BusException be) {
		 String msg = "Failed to call  View(),  objPath: '" + objectPath + "', Error: '" + be.getMessage() + "'";
                 Log.e(TAG, msg); 
	         throw new ControlPanelException(msg);
            }
		
	    if ( inputFormContainer == null ) {
		createContainerWidget();
            }
	    
	    return inputFormContainer;
	}//view
	
	/**
	 * Prepare the input form to view the record identified by the record id
	 * @param recordId The record id of the updated form <br>
	 * Complete updating the form by calling the {@link ListPropertyWidget#confirm()} or the {@link ListPropertyWidget#cancel()} methods
	 * @return A form {@link ContainerWidget} to be updated
	 * @throws ControlPanelException If failed to call the update method on the remote object
	 */
	public ContainerWidget update(short recordId) throws ControlPanelException {
	    Log.d(TAG, "Update() called, device: '" + device.getDeviceId() + "' objPath: '" + objectPath + "'");
		
	    try {
	        remoteControl.Update(recordId);
	    }
	    catch (BusException be) {
	        String msg = "Failed to call  Update(),  objPath: '" + objectPath + "', Error: '" + be.getMessage() + "'";
                Log.e(TAG, msg); 
	        throw new ControlPanelException(msg);
	    }
		
	    if ( inputFormContainer == null ) {
		createContainerWidget();
	    }
		
	    return inputFormContainer;
	}//update
	
	/**
	 * Confirm the action and save the change requested
	 * @throws ControlPanelException If failed to call the confirm method on the remote object
	 */
	public void confirm() throws ControlPanelException {
	    Log.d(TAG, "Confirm() called, device: '" + device.getDeviceId() + "' objPath: '" + objectPath + "'");
		
	    try {
		remoteControl.Confirm();
	    }
	    catch (BusException be) {
		String msg = "Failed to call  Confirm(),  objPath: '" + objectPath + "', Error: '" + be.getMessage() + "'";
                Log.e(TAG, msg); 
	        throw new ControlPanelException(msg);
	    }
	}//confirm
	
	/**
	 * Cancel the current action
	 * @throws ControlPanelException If failed to call the cancel method on the remote object
	 */
	public void cancel() throws ControlPanelException {
	    Log.d(TAG, "Cancel() called, device: '" + device.getDeviceId() + "' objPath: '" + objectPath + "'");
		
	    try {
		remoteControl.Cancel();
	    }
	    catch (BusException be) {
	        String msg = "Failed to call  Cancel(),  objPath: '" + objectPath + "', Error: '" + be.getMessage() + "'";
                Log.e(TAG, msg); 
	        throw new ControlPanelException(msg);
	    }
	}//cancel
	
	//============================================//
	
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
    
            Log.d(TAG, "Setting remote control ListOfProperties widget, objPath: '" + objectPath + "'");
            properties    = proxyObj.getInterface(Properties.class);
            remoteControl = (ListPropertyControlSuper) proxyObj.getInterface(ifClass);
    
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
		
	    Method listMetaDataChanged = CommunicationUtil.getListPropertyWidgetSignal("MetadataChanged");
	    if ( listMetaDataChanged == null ) {
    		String msg = "ListPropertyWidget, MetadataChanged method isn't defined";
    		Log.e(TAG, msg);
    		throw new ControlPanelException(msg);
	    }
		
	    Method listValueChanged = CommunicationUtil.getListPropertyWidgetSignal("ValueChanged");
	    if ( listValueChanged == null ) {
    		String msg = "ListPropertyWidget, ValueChanged method isn't defined";
    		Log.e(TAG, msg);
    		throw new ControlPanelException(msg);
	    }
		
            try {
                Log.d(TAG, "ListPropertyWidget objPath: '" + objectPath + "', registering signal handler 'MetadataChanged'");
                registerSignalHander(new ListPropertyWidgetSignalHandler(this), listMetaDataChanged);
                
                Log.d(TAG, "ListPropertyWidget objPath: '" + objectPath + "', registering signal handler 'ValueChanged'");
                registerSignalHander(new ListPropertyWidgetSignalHandler(this), listValueChanged);
            }//try
            catch (ControlPanelException cpe) {
                  String msg = "Device: '" + device.getDeviceId() +
                          "', ListPropertyWidget, failed to register signal handler, Error: '" + cpe.getMessage() + "'";
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
                if( "States".equals(propName) ) {
                    int states = propValue.getObject(int.class);
                    enabled    =  (states & ENABLED_MASK)  == ENABLED_MASK;
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
		
	    if ( size == 0 || size > 1 ) {
                throw new ControlPanelException("ListPropertyWidget has a wrong number of child elements, the child num is: '" + size + "'");
	    }
		
            IntrospectionNode childNode = children.get(0);
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
    
                //Found a known interface, check whether it's a Container
                if ( widgFactory.getElementType() == UIElementType.CONTAINER ) {
                    containerIntroNode = childNode;
                    containerIfname    = ifName; 
                    return;
                } 
            }//for :: interfaces

            throw new ControlPanelException("ListPropertyWidget objPath: '" + objectPath + "', does not have a ContainerWidget interface");
	}//createChildWidgets

	/**
	 * Creates the {@link ContainerWidget} which is used as an input form for the {@link ListPropertyWidget}
	 * @throws ControlPanelException If failed to create the widget
	 */
	private void createContainerWidget() throws ControlPanelException {
	    if ( containerIntroNode == null || containerIfname == null ) {
	        throw new ControlPanelException("ListProperty objPath: '" + objectPath + "' can't create the input form ContainerWidget, IntrospectionNode is undefined");
	    }
		
	    String path        = containerIntroNode.getPath();
	    Log.i(TAG, "ListProperty objPath: '" + objectPath + "', has a Container element, objPath: '" + path + "', creating...");
	    inputFormContainer = new ContainerWidget(containerIfname, path, controlPanel, containerIntroNode.getChidren());
	}//createContainerWidget
	
	/**
	 * Creates a list of records 
	 * @param recordsAJ The list of records received from the remote device 
	 * @return List of records
	 */
	private List<Record> createListOfRecords(ListPropertyWidgetRecordAJ[] recordsAJ) {
            List<Record> records = new LinkedList<ListPropertyWidget.Record>();
		
	    if ( recordsAJ == null ) {
		return records;
	    }
		
	    for (ListPropertyWidgetRecordAJ recordAJ : recordsAJ) { 
	        Record record = new Record(recordAJ.recordId, recordAJ.label);
		records.add(record);
	    }
		
	    return records;
	}//createListOfRecords
	
        /**
         * Fill the ListOfProperties optional parameters
         * @param valueType The value type of this property 
         * @param propertyMetadata To be filled with the optional parameters
         * @throws ControlPanelException if failed to read optional parameters
         */
        private void fillOptionalParams(Map<Short, Variant> optParams) throws ControlPanelException {
            // Add optional parameters
            Log.d(TAG, "ListOfProperties - scanning optional parameters");
    
            for (ListPropertyWidgetEnum optKeyEnum : ListPropertyWidgetEnum.values()) {
    
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
                            short[] listOfPropertiesHints = optParam.getObject(short[].class);
                            setListOfPropertyWidgetHints(listOfPropertiesHints);
                            break;
                        }
                    }//switch
                }//try
                catch(BusException be) {
                    throw new ControlPanelException("Failed to unmarshal optional parameters for ListOfProperties objPath: '" + objectPath + "'");
                }
            }//for
    
        }//fillOptionalParams

       /**         
	* Iterates over the hintIds array and fills list of {@link ListPropertyWidgetHintsType}
	* @param hintIds Ids of the widget hints
	* @return
	*/     
	private void setListOfPropertyWidgetHints(short[] hintIds) {
	    Log.v(TAG, "ListOfProperty objPath: '" + objectPath + "', filling ListOfProperties hints");
	    this.hints = new ArrayList<ListPropertyWidgetHintsType>( hintIds.length );
		
	    for (short hId : hintIds) {
	        ListPropertyWidgetHintsType hintType = ListPropertyWidgetHintsType.getEnumById(hId);
		if (hintType == null) {
		    Log.w(TAG, "Received unrecognized hintId: '" + hId + "', ignoring");
		    continue;
		}
		hints.add(hintType);
	    }//for :: hintId
	
	}//setListOfPropertyWidgetHints
    
}
