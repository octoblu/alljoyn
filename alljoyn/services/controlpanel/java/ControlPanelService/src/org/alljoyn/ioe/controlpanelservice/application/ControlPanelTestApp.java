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
package org.alljoyn.ioe.controlpanelservice.application;

import java.util.Collection;
import java.util.EnumMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Random;

import org.alljoyn.ioe.controlpanelservice.ControlPanelCollection;
import org.alljoyn.ioe.controlpanelservice.ControlPanelException;
import org.alljoyn.ioe.controlpanelservice.ControllableDevice;
import org.alljoyn.ioe.controlpanelservice.DefaultDeviceRegistry;
import org.alljoyn.ioe.controlpanelservice.DeviceEventsListener;
import org.alljoyn.ioe.controlpanelservice.Unit;
import org.alljoyn.ioe.controlpanelservice.ui.ActionWidget;
import org.alljoyn.ioe.controlpanelservice.ui.AlertDialogWidget;
import org.alljoyn.ioe.controlpanelservice.ui.AlertDialogWidget.DialogButton;
import org.alljoyn.ioe.controlpanelservice.ui.ContainerWidget;
import org.alljoyn.ioe.controlpanelservice.ui.ControlPanelEventsListener;
import org.alljoyn.ioe.controlpanelservice.ui.DeviceControlPanel;
import org.alljoyn.ioe.controlpanelservice.ui.ErrorWidget;
import org.alljoyn.ioe.controlpanelservice.ui.LabelWidget;
import org.alljoyn.ioe.controlpanelservice.ui.LayoutHintsType;
import org.alljoyn.ioe.controlpanelservice.ui.ListPropertyWidget;
import org.alljoyn.ioe.controlpanelservice.ui.ListPropertyWidget.Record;
import org.alljoyn.ioe.controlpanelservice.ui.PropertyWidget;
import org.alljoyn.ioe.controlpanelservice.ui.PropertyWidget.ConstrainToValues;
import org.alljoyn.ioe.controlpanelservice.ui.PropertyWidget.Date;
import org.alljoyn.ioe.controlpanelservice.ui.PropertyWidget.RangeConstraint;
import org.alljoyn.ioe.controlpanelservice.ui.PropertyWidget.Time;
import org.alljoyn.ioe.controlpanelservice.ui.PropertyWidget.ValueType;
import org.alljoyn.ioe.controlpanelservice.ui.UIElement;
import org.alljoyn.ioe.controlpanelservice.ui.UIElementType;

import android.annotation.SuppressLint;
import android.util.Log;

public class ControlPanelTestApp extends DefaultDeviceRegistry implements DeviceEventsListener, ControlPanelEventsListener {
	private static final String TAG = "cpan" + ControlPanelTestApp.class.getSimpleName(); 
	
	private Map<UIElementType, List<UIElement>> uiControls     = new EnumMap<UIElementType, List<UIElement>>(UIElementType.class); 
	
	private Map<UIElementType, List<UIElement>> uiControlsList = new EnumMap<UIElementType, List<UIElement>>(UIElementType.class);
	
	/**
	 * Constructor 
	 */
	public ControlPanelTestApp() {
	}

	@Override
	public void foundNewDevice(ControllableDevice device) {
		super.foundNewDevice(device);
		if ( !device.isReachable() ) {
			Log.d(TAG, "Device isn't reachable");
			return;
		}
		Log.d(TAG, "FOUND_NEW_DEVICE received, handling");
		handleNewDevice(device);
	}//foundNewDevice

	@Override
	public void reachabilityChanged(ControllableDevice device, boolean isReachable) {
		super.reachabilityChanged(device, isReachable);
		Log.d(TAG, "Device: '" + device.getDeviceId() + "' got event of 'REACHABILITY_CHANGED' value: '" + isReachable + "'");
		if ( isReachable ) {
			handleNewDevice(device);
		}
	}//reachabilityChanged

	@Override
	public void removeDevice(ControllableDevice device) {
		super.removeDevice(device);
	}
	
	private void handleNewDevice(ControllableDevice device) {
		try {
			Log.d(TAG, "Device: '" + device.getDeviceId() + "', call to start a session");
			device.startSession(this);
		} catch (ControlPanelException e) {
			e.printStackTrace();
		}
	}//handleNewDevice

	//==========================================//
	  
	@Override
	public void sessionEstablished(ControllableDevice device, Collection<ControlPanelCollection> controlPanelCollections) {
		Log.d(TAG, "Received SESSION_ESTABLISHED, looping over the received control panels");
		
		for(ControlPanelCollection controlPanelCollection : controlPanelCollections) {
			initCollectionTest(controlPanelCollection);
		}//for :: control panel collections
		
		
		try {
			
			//========   Test the NotificationAction================//
			
			Log.d(TAG, "Test the NotificationAction ControlPanel");
			ControlPanelCollection notifActColl = device.createNotificationAction("/ControlPanel/MyDevice/areYouSure");
			initCollectionTest(notifActColl);
			
			Log.d(TAG, "Calling removeNotificationAction()");
			notifActColl.getDevice().removeNotificationAction(notifActColl);
			
			//========   Test the ControlPanel using addControlPanel  ================//
			
			/*Log.d(TAG, "Test the ControlPanel using addControlPanel");
			//Unit unit = device.addControlPanel("/ControlPanel/MyDevice/rootContainer", "org.alljoyn.ControlPanel.ControlPanel");
			Unit unit = device.addControlPanel("/ControlPanel/MyDevice/areYouSure", "org.alljoyn.ControlPanel.ControlPanel");
			for ( ControlPanelCollection coll : unit.getControlPanelCollection() ) {
				initCollectionTest(coll);
			}
			*/
		}
		catch (ControlPanelException cpe) {
			Log.e(TAG, "Failed to create the NotificationAction application, Error: '" + cpe. getMessage() + "'");
		}
			
	}//sessionEstablished

	@Override
	public void sessionLost(ControllableDevice device) {
		Log.d(TAG, "Received SESSION_LOST for device: '" + device.getDeviceId() + "'");
		//If we lost session we don't want to store reference to the old UI widgets
		uiControls.clear();
		uiControlsList.clear();
   	    //reconnectSession(device);
	}//sessionLost

	@Override
	public void errorOccurred(ControllableDevice device, String reason) {
		Log.e(TAG, "Found error: '" + reason + "'");
	}//errorOccurred

	@Override
	public void errorOccurred(DeviceControlPanel panel, String reason) {
		Log.e(TAG, "An error occurred in the DeviceControlPanel, name: '" + panel.getObjPath() + "' Panels name: '" + panel.getCollection().getName() + "', Reason: '" + reason + "'");
	}

	@Override
	public void valueChanged(DeviceControlPanel panel, UIElement uielement, Object newValue) {
		Log.i(TAG, "VALUE_CHANGED : Received value changed signal, device: '" + panel.getDevice().getDeviceId() + "', ObjPath: '" + uielement.getObjectPath() + "', NewValue: '" + newValue + "'");
	} 

	@Override
	public void metadataChanged(DeviceControlPanel panel, UIElement uielement) {
		UIElementType elementType = uielement.getElementType();
		Log.i(TAG, "METADATA_CHANGED : Received metadata changed signal, device: '" + panel.getDevice().getDeviceId() + "', ObjPath: '" + uielement.getObjectPath() + "', element type: '" + elementType + "'");
		
		 switch(elementType) {
			 case CONTAINER: {
				 ContainerWidget container = (ContainerWidget)uielement;
				 Log.i(TAG, "Container metadata IsEnabled: '" + container.isEnabled() + "'");
				 break;
			 }
		     case ACTION_WIDGET: {
				 ActionWidget action = (ActionWidget)uielement;
				 Log.i(TAG, "Action metadata IsEnabled: '" + action.isEnabled() + "'");
		    	 break;
		     }
		     case PROPERTY_WIDGET: {
				 PropertyWidget prop = (PropertyWidget)uielement;
				 Log.i(TAG, "Property metadata IsEnabled: '" + prop.isEnabled() + "'");
		    	 break;
		     }
		     case ALERT_DIALOG : {
		    	 AlertDialogWidget alertDialog = (AlertDialogWidget)uielement;
				 Log.i(TAG, "AlertDialog metadata IsEnabled: '" + alertDialog.isEnabled() + "'");
		    	 break;
		     }
		     case LABEL_WIDGET: {
		    	 LabelWidget  labelWidget = (LabelWidget)uielement;
				 Log.i(TAG, "LabelWidget metadata IsEnabled: '" + labelWidget.isEnabled() + "'");
		    	 break;
		     }
		     case LIST_PROPERTY_WIDGET: {
		    	 ListPropertyWidget  listWidget = (ListPropertyWidget)uielement;
				 Log.i(TAG, "ListPropertyWidget metadata IsEnabled: '" + listWidget.isEnabled() + "'");
		    	 break;		
		     }
		     case ERROR_WIDGET: {
		    	 break;
		     }
		 }
	}//metadataChanged
	
	@Override
	public void notificationActionDismiss(DeviceControlPanel panel) {
		Log.i(TAG, "NOTIFICATION_ACTION_DISMISS has been received, dismissing the panel: '" + panel.getObjPath()+ "'");
		//Log.d(TAG, "Calling removeNotificationAction()");
		//panel.getDevice().removeNotificationAction(panel.getCollection());
	}

	
	//============================================//
	
	private void handleContainer(ContainerWidget container, boolean isFromList) {
		if ( container == null ) {
			Log.e(TAG, "The received main container is null");
			return;
		}
		
		String fromList = "";
		
		if ( isFromList ) {
			fromList = "(FROM LIST)";
			try {
				container.refreshProperties();
			} catch (ControlPanelException e) {
				Log.e(TAG, "Failed to update the properties of the ContainerWidget objPath: '" + container.getObjectPath() + "'");
				return;
			}
		}
		
		Log.i(TAG, "===== " + fromList + " CONTAINER profiler ===== ");
		
		Log.i(TAG, "Received container, Version: '" + container.getVersion() + "', objPath: '" + container.getObjectPath() + "'" + 
				   " label: '" + container.getLabel() + "' bgColor:  " + container.getBgColor() + "  enabled: " + container.isEnabled());
		
		List<LayoutHintsType> layHints = container.getLayoutHints();
		if ( layHints == null || layHints.size() == 0 ) {
			Log.i(TAG, "Not found layout hints");
		}
		Log.i(TAG, "LayoutHints: " + layHints);
		
		List<UIElement> elements = container.getElements();
		
		if ( elements.size() == 0 ) {
			Log.d(TAG, "Not found any nested elements in the container");
			return;
		}

		Log.i(TAG, "Iterating over the elements: ");
		
		for(UIElement element : elements) {
			UIElementType elementType = element.getElementType();
			
			Log.d(TAG, "Found element of type: '" + elementType + "'");
			
			switch(elementType) {
				case ACTION_WIDGET: {
					handleAction((ActionWidget)element, isFromList);
					addToUiControls(UIElementType.ACTION_WIDGET, element, isFromList);
					break;
				}//ACTION_WIDGET
				case CONTAINER: {
					handleContainer((ContainerWidget) element, isFromList);
					addToUiControls(UIElementType.CONTAINER, element, isFromList);
					break;
				}// CONTAINER
				case LIST_PROPERTY_WIDGET: {
					handlePropertyList((ListPropertyWidget) element, isFromList);
					addToUiControls(UIElementType.LIST_PROPERTY_WIDGET, element, isFromList);
					break;
				}//LIST_PROPERTY_WIDGET
				case PROPERTY_WIDGET: {
					handleProperty((PropertyWidget) element, isFromList);
					addToUiControls(UIElementType.PROPERTY_WIDGET, element, isFromList);
					break;
				}//PROPERTY_WIDGET
				case LABEL_WIDGET: {
					handleLabel((LabelWidget) element, isFromList);
					addToUiControls(UIElementType.LABEL_WIDGET, element, isFromList);
					break;
				}
				case ALERT_DIALOG: {
					handleAlertDialog((AlertDialogWidget) element, isFromList);
					addToUiControls(UIElementType.ALERT_DIALOG, element, isFromList);
					break;
				}
				case ERROR_WIDGET: {
					handleErrorWidget((ErrorWidget) element);
					break;
				}
			}//switch :: elementType
			
		}//for :: elements
		
		addToUiControls(UIElementType.CONTAINER, container, isFromList);
	}//render
	
	private void handleProperty(PropertyWidget property, boolean isFromList) {
		
		String fromList = "";
		
		if ( isFromList ) {
			fromList = "(FROM LIST)";
			try {
				property.refreshProperties();
			} catch (ControlPanelException e) {
				Log.e(TAG, "Failed to update the properties of the PropertyWidget objPath: '" + property.getObjectPath() + "'");
				return;
			}
		}
		
		Log.i(TAG, "===== " + fromList + " PROPERTY profiler ===== ");
		
		ValueType valueType = property.getValueType();
		
		Log.i(TAG, "Property of value type: '" + valueType + "', objPath: '" + property.getObjectPath() + "'"); 
		
		Log.i(TAG, "Property Version: '" + property.getVersion() + "' label: '" + property.getLabel() + "' Writable: '" + property.isWritable() + "' unitOfMeas: '" + property.getUnitOfMeasure() +
				"', BGcolor: '" + property.getBgColor() + "' PropHints: '" + property.getHints() + "' Enabled: '" + property.isEnabled() + "'");

		if ( property.getListOfConstraint() != null ) {
			Log.i(TAG, "Property List-Of-Constraints: ");
			for (ConstrainToValues<?> valueCons : property.getListOfConstraint() ) {
				Log.i(TAG, " lovCons Value: " + valueCons.getValue() + " label: " + valueCons.getLabel());
			}
		}//LOV constraints
		
		if ( property.getPropertyRangeConstraint() != null ) {
			RangeConstraint<?> propRangeCons = property.getPropertyRangeConstraint();
			Log.d(TAG, "Property Range Constraint, Min: " + propRangeCons.getMin() + " Max: " +
			      propRangeCons.getMax() + " Increment: " + propRangeCons.getIncrement());
		}
		
		Log.i(TAG, "===== END OF PROPERTY profiler ===== ");
	}//handleProperty

	private void handleAction(ActionWidget actionWidget, boolean isFromList) {
		String fromList = "";
		
		if ( isFromList ) {
			fromList = "(FROM LIST)";
			try {
				actionWidget.refreshProperties();
			} catch (ControlPanelException e) {
				Log.e(TAG, "Failed to update the properties of the ActionWidget objPath: '" + actionWidget.getObjectPath() + "'");
				return;
			}
		}
		
		Log.i(TAG, "===== " + fromList + " ACTION profiler ===== ");
		
		Log.i(TAG, "ActionWidget: objPath: '" + actionWidget.getObjectPath() + "'");
		Log.i(TAG, "Version: '" + actionWidget.getVersion() + "' Enabled: '" + actionWidget.isEnabled() + "'" + 
		          " Label: '" + actionWidget.getLabel() + "' BGColor: '" + actionWidget.getBgColor() + "' actionHints: '" + actionWidget.getHints() + "'");

		AlertDialogWidget alertDialog = actionWidget.getAlertDialog();
		if ( alertDialog != null ) {
			Log.i(TAG, "The ActionWidget: objPath: '" + actionWidget.getObjectPath() + "' has AlertDialog in the sub tree, handling...");
			handleAlertDialog(alertDialog, isFromList);
			addToUiControls(UIElementType.ALERT_DIALOG, alertDialog, isFromList);
		}
		else {
			Log.i(TAG, "The ActionWidget: objPath: '" + actionWidget.getObjectPath() + "' doesn't have the AlertDialog");
		}
		
	}//handleAction

	private void handleAlertDialog(AlertDialogWidget alertDialog, boolean isFromList) {
		String fromList = "";
		
		if ( isFromList ) {
			fromList = "(FROM LIST)";
			try {
				alertDialog.refreshProperties();
			} catch (ControlPanelException e) {
				Log.e(TAG, "Failed to update the properties of the AlertDialog objPath: '" + alertDialog.getObjectPath() + "'");
				return;
			}
		}
		
		Log.i(TAG, "===== " + fromList + " ALERT_DIALOG profiler ===== ");
		
		Log.i(TAG, "AlertDialog: objPath: '" + alertDialog.getObjectPath() + "'");
		Log.i(TAG, "Version: '" + alertDialog.getVersion() + "' Enabled: '" + alertDialog.isEnabled() + "'" +
				  " Message: '" + alertDialog.getMessage() + "' NumActions: '" + alertDialog.getNumActions() + "'" +
		          " Label: '" + alertDialog.getLabel() + "' BGColor: '" + alertDialog.getBgColor() + 
		          "' hints: '" + alertDialog.getHints() + "'");

		Log.i(TAG, "===== END OF ALERT_DIALOG profiler ===== ");
	}//handleAlertDialog
	
	private void handleLabel(LabelWidget label, boolean isFromList) {
		String fromList = "";
		
		if ( isFromList ) {
			fromList = "(FROM LIST)";
			try {
				label.refreshProperties();
			} catch (ControlPanelException e) {
				Log.e(TAG, "Failed to update the properties of the LabelWidget objPath: '" + label.getObjectPath() + "'");
				return;
			}
		}
		
		Log.i(TAG, "==== " + fromList + " LABEL profiler ===== ");
		
		Log.i(TAG, "LabelWidget: objPath: '" + label.getObjectPath() + "'");
		Log.i(TAG, "Version: '" + label.getVersion() + "' Enabled: '" + label.isEnabled() + "'" +
		          " Label: '" + label.getLabel() + "' BGColor: '" + label.getBgColor() + "', LabelHints: '" + label.getHints() + "'");
		
		Log.i(TAG, "===== END OF LABEL profiler ===== ");
	}//handleLabel
	
	private void handlePropertyList(ListPropertyWidget listProperty, boolean isFromList) {
		String fromList = "";
		
		if ( isFromList ) {
			fromList = "(FROM LIST)";
			try {
				listProperty.refreshProperties();
			} catch (ControlPanelException e) {
				Log.e(TAG, "Failed to update the properties of a AlertDialog objPath: '" + listProperty.getObjectPath() + "'");
				return;
			}
		}
		
		Log.i(TAG, "==== " + fromList + " LIST_PROPERTY profiler ===== ");
		
		Log.i(TAG, "ListProperty: objPath: '" + listProperty.getObjectPath() + "'");
		Log.i(TAG, "Version: '" + listProperty.getVersion() + "' Enabled: '" + listProperty.isEnabled() + "'" +
		          " Label: '" + listProperty.getLabel() + "' BGColor: '" + listProperty.getBgColor() + "', LabelHints: '" + listProperty.getHints() + "'");
		
		Log.i(TAG, "===== END OF LIST_PROPERTY profiler ===== ");		
	}//handlePropertyList
	
	private void handleErrorWidget(ErrorWidget errorWidget) {
		Log.w(TAG, "==== ERROR_WIDGET profiler ===== ");
		
		Log.w(TAG, "ErrorWidget: objPath: '" + errorWidget.getObjectPath() + "', failed to be created");
		Log.w(TAG, "Default Error Label: '" + errorWidget.getLabel() + "' Error: '" + errorWidget.getError() + "', OriginalUIElement: '" + errorWidget.getOriginalUIElement() + "'");
		
		Log.w(TAG, "===== END OF ERROR_WIDGET profiler ===== ");		
	}//handleErrorWidget
	
	//=================================================//
	//			           TESTS					   //
	//=================================================//

	/**
	 * @param collection Initialize test of the given collection
	 */
	private void initCollectionTest(ControlPanelCollection controlPanelCollection) {
	
		Unit unit = controlPanelCollection.getUnit();			
		try {
			Log.d(TAG, "Testing the UNIT: '" + unit.getUnitId() + "'");
			if ( unit.getHttpIfaceObjPath() != null ) {
			    Log.d(TAG, "The unit: '" + unit.getUnitId() + "' has a control, HTTPProtocol version: '" + unit.getHttpControlVersion() + " URL: '" + unit.getRootURL() + "'");
			}
		}
		catch (ControlPanelException cpe) {
			Log.e(TAG, "Failed to read attributes of the UNIT: '" + unit.getUnitId() + "'");
		}

		Log.i(TAG, "Test the ControlPanelCollection name: '" + controlPanelCollection.getName() + "', objPath: '" + controlPanelCollection.getObjectPath() + "' collection languages: '" + controlPanelCollection.getLanguages() + "', ControlPanelVersion: '" + controlPanelCollection.getControlPanelVersion() + "', NotificationActionVersion: '" + controlPanelCollection.getNotificationActionVersion() + "'");
		
		for ( DeviceControlPanel controlPanel : controlPanelCollection.getControlPanels() ) {
			
			uiControls.clear();
			uiControlsList.clear();
			
			Log.i(TAG, "STARTED TEST OF CONTROL PANEL, objPath: '" + controlPanel.getObjPath() + "', lang: '" + controlPanel.getLanguage() + "'");
			
			try {
				
				UIElement rootContainerElement = controlPanel.getRootElement(this);
				
				if ( rootContainerElement == null ) {
					Log.e(TAG, "RootContainerElement wasn't created!!! Can't continue");
					return;
				}
				
				UIElementType elementType      = rootContainerElement.getElementType();
				Log.d(TAG, "Found root container of type: '" + elementType + "'");
				
				if ( elementType == UIElementType.CONTAINER ) {
					handleContainer((ContainerWidget)rootContainerElement, false);
				}
				else if ( elementType == UIElementType.ALERT_DIALOG ) {
					handleAlertDialog((AlertDialogWidget)rootContainerElement, false);
					addToUiControls(UIElementType.ALERT_DIALOG, (AlertDialogWidget)rootContainerElement, false);
				}

				runTestForUiControls(uiControls);
			}//try
			catch(ControlPanelException cpe) {
				Log.e(TAG, "Failed to access remote methods of control panel, Error: '" + cpe.getMessage() + "'");
				continue;
			}
		
			Log.i(TAG, "FINISHED TEST OF THE CONTROL_PANEL: '" + controlPanel.getObjPath()  + "'");
			
		}//for :: ControlPanels

	}//initCollectionTest
	
	/**
	 * Adds the tested UI widget to the UI controls list
	 * @param type
	 * @param element
	 */
	private void addToUiControls(UIElementType type, UIElement element, boolean isFromList) {
		List<UIElement> elements = null;
		
		if ( !isFromList ) {
			elements = uiControls.get(type);
			if ( elements == null ) {
				elements = new LinkedList<UIElement>();
				uiControls.put(type, elements);
			}
		}
		else {
			elements = uiControlsList.get(type);
			if ( elements == null ) {
				elements = new LinkedList<UIElement>();
				uiControlsList.put(type, elements);
			}
		}
		
		elements.add(element);
	}//addToUiControls
	
	/**
	 * Run tests for UI controls
	 */
	@SuppressLint("DefaultLocale")
	private void runTestForUiControls(Map<UIElementType, List<UIElement>> uiControls) {

		Log.i(TAG, "==== START CONTROL TESTING ====");
		
		for (UIElementType elementType : uiControls.keySet() ) {
			String elementTypeStr = elementType.toString().toUpperCase();
			Log.d(TAG, "TEST element: '" + elementTypeStr + "'");
			switch(elementType) {
				 case CONTAINER: {
					  List<UIElement> elements = uiControls.get(UIElementType.CONTAINER);
					  for (UIElement element : elements) {
							ContainerWidget container = (ContainerWidget) element;
							try {
								Log.i(TAG, elementTypeStr + " - Test container: '" + container.getObjectPath() + "', Version: '" + container.getVersion() + "'");
								Log.i(TAG, elementTypeStr + " - Refreshing CONTAINER metadata");
								container.refreshProperties();
								Log.i(TAG, elementTypeStr + " - The refreshed container ENABLE is: '" + container.isEnabled() +  "'");
							} catch (ControlPanelException cpe) {
								Log.e(TAG, "Failed happened in calling remote object: '" + cpe.getMessage() + "'");
							}
					  }//for :: elements
				      break;
				 }//CONTAINER
				 case PROPERTY_WIDGET: {
					  List<UIElement> elements = uiControls.get(UIElementType.PROPERTY_WIDGET);
					  for (UIElement element : elements) {
							
						   PropertyWidget property = (PropertyWidget) element;
						  
							try {
								Log.i(TAG, elementTypeStr + " - Test property: '" + property.getObjectPath() + "', Version: '" + property.getVersion() + "'" +
								        " Type: '" + property.getValueType() + "', Current Property Value: '" + property.getCurrentValue() + "'");
								Log.i(TAG, elementTypeStr + " - Refreshing PROPERTY");
								property.refreshProperties();
								Log.i(TAG, elementTypeStr + " - The refreshed property ENABLE is: '" + property.isEnabled() +  "'");
								Log.i(TAG, elementTypeStr +"  - PROPERTY setting new value");
								
								switch( property.getValueType() ) {
								    case BOOLEAN :{
								    	property.setCurrentValue(true);
								    	break;
								    }
								    case BYTE: {
								    	property.setCurrentValue((byte) 1);
								    	break;
								    }
								    case DOUBLE:{
								    	property.setCurrentValue(123.45);
								    	break;
								    }
								    case INT: {
								    	property.setCurrentValue(123);
								    	break;
								    }
								    case LONG:{
								    	property.setCurrentValue(111222323L);
								    	break;
								    }
								    case SHORT: {
								    	property.setCurrentValue((short)12);
								    	break;
								    }
								    case STRING: {
								    	property.setCurrentValue("THE NEW PROP VALUE: " + Math.random());
								    	break;
								    }
								    case DATE: {
								    	Date dt = new PropertyWidget.Date();
							    		dt.setDay((short)29);
							    		dt.setMonth((short)11);
							    		dt.setYear((short)1947);
								    	property.setCurrentValue(dt);
								    	break;
								    }//DATE
								    case TIME: {
								    	Time time = new PropertyWidget.Time();
								    	time.setHour((short)23);
							    		time.setMinute((short)59);
							    		time.setSecond((short)59);
							    		property.setCurrentValue(time);
								    }//TIME
								}//switch
								
								Log.i(TAG, elementTypeStr + " - Test property: '" + property.getObjectPath() + "', test NEW PROPERTY VALUE, call: property.getCurrentValue(): '" + property.getCurrentValue() + "'");
							} catch (ControlPanelException cpe) {
								Log.e(TAG, "Failed happened in calling remote object: '" + cpe.getMessage() + "'");
							}
					  }//for :: elements
					 break;
				 }//PROPERTY_WIDGET
				 case ACTION_WIDGET: {
					  List<UIElement> elements = uiControls.get(UIElementType.ACTION_WIDGET);
					  for (UIElement element : elements) {
							ActionWidget action = (ActionWidget)element;
							try {
								Log.i(TAG, elementTypeStr + " - Test action: '" + action.getObjectPath() + "', Version: '" + action.getVersion() + "'");
								Log.i(TAG, elementTypeStr + " - Refreshing ACTION metadata");
								action.refreshProperties();
								Log.i(TAG, elementTypeStr + " - The refreshed property ENABLE is: '" + action.isEnabled() +  "'");
								if ( action.getAlertDialog() == null ) {
									Log.i(TAG, "AlertDialog is not found in the action, calling Action.exec");
									action.exec();
									Log.i(TAG, "Action.exec was called successfully");
								}
								else {
									Log.i(TAG, "The action: '" + action.getObjectPath() + "', has alertDialog, not calling exec");
								}
							} catch (ControlPanelException cpe) {
								Log.e(TAG, "Failed happened in calling remote object: '" + cpe.getMessage() + "'");
							}
					  }//for :: elements					 
					 break;
				 }//ACTION_WIDGET
				 case ALERT_DIALOG: {
					  List<UIElement> elements = uiControls.get(UIElementType.ALERT_DIALOG);
					  for (UIElement element : elements) {
							AlertDialogWidget alertDialog = (AlertDialogWidget)element;
							try {
								Log.i(TAG, elementTypeStr + " - Test alertDialog: '" + alertDialog.getObjectPath() + "', Version: '" + alertDialog.getVersion() + "'");
								Log.i(TAG, elementTypeStr + " - Refreshing AlertDialog");
								alertDialog.refreshProperties();
								Log.i(TAG, elementTypeStr + " - The refreshed AlertDialog ENABLE is: '" + alertDialog.isEnabled() +  "'");
								List<DialogButton> buttons = alertDialog.getExecButtons();
								Log.i(TAG, "The AlertDialog objPath: '" + alertDialog.getObjectPath() + "', has '" + buttons.size() + "' buttons");
								for (DialogButton button : buttons) {
									Log.i(TAG, "Found button '" + button.getLabel() + "', pressing");
									button.exec();
								}
							} catch (ControlPanelException cpe) {
								Log.e(TAG, "Failed happened in calling remote object: '" + cpe.getMessage() + "'");
							}
					  }//for :: elements					 
					 break;
				 }//ALERT_DIALOG
				 case LABEL_WIDGET: {
					  List<UIElement> elements = uiControls.get(UIElementType.LABEL_WIDGET);
					  for (UIElement element : elements) {
							LabelWidget label = (LabelWidget)element;
							try {
								Log.i(TAG, elementTypeStr + " - Test LabelWidget: '" + label.getObjectPath() + "', Version: '" + label.getVersion() + "'");
								Log.i(TAG, elementTypeStr + " - Refreshing LABEL_WIDGET");
								label.refreshProperties();
								Log.i(TAG, elementTypeStr + " - The refreshed property ENABLE is: '" + label.isEnabled() +  "'");					
							} catch (ControlPanelException cpe) {
								Log.e(TAG, "Failed happened in calling remote object: '" + cpe.getMessage() + "'");
							}
					  }//for :: elements					 
					 break;
				 }//LABEL_WIDGET
				 case LIST_PROPERTY_WIDGET: {
					  List<UIElement> elements = uiControls.get(UIElementType.LIST_PROPERTY_WIDGET);
					  for (UIElement element : elements) {
						  ListPropertyWidget listProperty = (ListPropertyWidget)element;
						  try {
							  testListProperty(listProperty);
						  }
						  catch(ControlPanelException cpe) {
							  Log.e(TAG, "Failed happened in calling remote object: '" + cpe.getMessage() + "'");
						  }
					  }//for :: elements
					 break;
				 }
				 case ERROR_WIDGET: {
					 break;
				 }
			}//switch :: elementType
		}//for::elementType
		
		Log.i(TAG, "==== TEST IS COMPLETED ==== ");
	}//runTestForUiControls
	
	/**
	 * Run test for ListPropertyWidget
	 * @param listProps
	 * @throws ControlPanelException
	 */
	private void testListProperty(ListPropertyWidget listProps) throws ControlPanelException {
		Random rand = new Random();
		
		Log.i(TAG, UIElementType.LIST_PROPERTY_WIDGET + " - Test ListProperty: '" + listProps.getObjectPath() + "'");
		
		List<Record> records = listProps.getValue();
		Log.i(TAG,  UIElementType.LIST_PROPERTY_WIDGET + " - ListProperty record values: '" + records + "'");
		
		Log.i(TAG,  UIElementType.LIST_PROPERTY_WIDGET + " - **** TEST ADD() ****");
		 
		//TEST ADD
		for(int i=1; i<=2; ++i) {
			boolean willCancel;
			
			if (i == 1) {
				willCancel = true;
			}
			else {
				willCancel = false;
			}
			
			Log.i(TAG, UIElementType.LIST_PROPERTY_WIDGET + " - TEST #" + i + "  Call Add() method and then Cancel: '" + willCancel + "'");
			ContainerWidget container = listProps.add();
			
			Log.i(TAG, "Handle the new Added form");
			uiControlsList.clear();
			handleContainer(container, true);
			runTestForUiControls(uiControlsList);
			
			if ( willCancel ) {
				Log.i(TAG, UIElementType.LIST_PROPERTY_WIDGET + " - Call Cancel() method");
				listProps.cancel();
			}
			else {
				Log.i(TAG, UIElementType.LIST_PROPERTY_WIDGET + " - Call Confirm() method");
				listProps.confirm();
			}
			
			int sizeBefore = records.size();
			records = listProps.getValue();
			int sizeAfter  = records.size();
			
			// If the Add() operation was confirmed the size should be greater
			if ( !willCancel && sizeAfter <= sizeBefore ) {
				Log.e(TAG, UIElementType.LIST_PROPERTY_WIDGET + " - A Possible error has happened, after the Add() operation the sizeAfter: '" + sizeAfter + "', should be greater than the sizeBefore: '" + sizeBefore + "'");
			}
			
			Log.i(TAG, UIElementType.LIST_PROPERTY_WIDGET + " -  Check the recordValues: '" + records + "', size before: '" + sizeBefore + "', sizeAfter: '" + sizeAfter + "'");
		}//for::Add
		//================================== END ADD ==============================//	

		Log.i(TAG,  UIElementType.LIST_PROPERTY_WIDGET + " - **** TEST UPDATE() ****");
		
		//TEST UPDATE
		for(int i=1; i<=2; ++i) {
			boolean willCancel;

			if (i == 1) {
				willCancel = true;
			}
			else {
				willCancel = false;
			}
			
			Log.i(TAG, UIElementType.LIST_PROPERTY_WIDGET + " - TEST #" + i + " Call Update() method and then Cancel: '" + willCancel + "'");
			
			if ( records.size() == 0 ) {
				Log.i(TAG, UIElementType.LIST_PROPERTY_WIDGET + " - No records in the list, adding a form...");
				ContainerWidget cont = listProps.add();
				uiControlsList.clear();
				handleContainer(cont, true);
				runTestForUiControls(uiControlsList);
				listProps.confirm();
			}
			
			records   = listProps.getValue();
			int size  = records.size();
			if ( size == 0 ) {
				Log.e(TAG, UIElementType.LIST_PROPERTY_WIDGET + " - No records were found, previously failed to add a new record");
				break;
			}
			
			int index     = rand.nextInt(size);
			Record record = records.get(index);
			
			Log.i(TAG, UIElementType.LIST_PROPERTY_WIDGET + " - Call Update() method on the record: '" + record + "' and then Cancel: '" + willCancel + "'");
			ContainerWidget cont = listProps.update(record.getRecordId());
			uiControlsList.clear();
			handleContainer(cont, true);
			runTestForUiControls(uiControlsList);
				
			if ( willCancel ) {
				Log.i(TAG, UIElementType.LIST_PROPERTY_WIDGET + " - Call Cancel() method");
				listProps.cancel();
			}
			else {
				Log.i(TAG, UIElementType.LIST_PROPERTY_WIDGET + " - Call Confirm() method");
				listProps.confirm();
			}
		}//for::Update
		//================================== END UPDATE ==============================//
		
		Log.i(TAG,  UIElementType.LIST_PROPERTY_WIDGET + " - **** TEST DELETE() ****");
		
		//TEST DELETE
		for(int i=1; i<=2; ++i) {
			boolean willCancel;

			if (i == 1) {
				willCancel = true;
			}
			else {
				willCancel = false;
			}
			
			Log.i(TAG, UIElementType.LIST_PROPERTY_WIDGET + " - TEST #" + i + " Call Delete() method and then Cancel: '" + willCancel + "'");
			
			if ( records.size() == 0 ) {
				Log.i(TAG, UIElementType.LIST_PROPERTY_WIDGET + " - No records in the list, adding a form...");
				ContainerWidget cont = listProps.add();
				uiControlsList.clear();
				handleContainer(cont, true);
				runTestForUiControls(uiControlsList);
				listProps.confirm();
			}
			
			records   = listProps.getValue();
			int size  = records.size();
			if ( size == 0 ) {
				Log.e(TAG, UIElementType.LIST_PROPERTY_WIDGET + " - No records were found, previously failed to add a new record");
				break;
			}
			
			int index     = rand.nextInt(size);
			Record record = records.get(index);
			
			Log.i(TAG, UIElementType.LIST_PROPERTY_WIDGET + " - Call Delete() method on the record: '" + record + "' and then Cancel: '" + willCancel + "'");
			listProps.delete(record.getRecordId());
				
			if ( willCancel ) {
				Log.i(TAG, UIElementType.LIST_PROPERTY_WIDGET + " - Call Cancel() method");
				listProps.cancel();
			}
			else {
				Log.i(TAG, UIElementType.LIST_PROPERTY_WIDGET + " - Call Confirm() method");
				listProps.confirm();
			}
			
			int sizeBefore = records.size();
			records = listProps.getValue();
			int sizeAfter  = records.size();
			
			// If the Delete() operation was confirmed the size after should be smaller than the size before
			if ( !willCancel && sizeAfter == sizeBefore ) {
				Log.e(TAG, UIElementType.LIST_PROPERTY_WIDGET + " - A Possible error has happened, after the Delete() operation the sizeAfter: '" + sizeAfter + "', should be smaller than the sizeBefore: '" + sizeBefore + "'");
			}
			
			Log.i(TAG, UIElementType.LIST_PROPERTY_WIDGET + " -  Check the recordValues: '" + records + "', size before: '" + sizeBefore + "', sizeAfter: '" + sizeAfter + "'");
		}//for::Delete
		//================================== END DELETE ==============================//

		
		Log.i(TAG,  UIElementType.LIST_PROPERTY_WIDGET + " - **** TEST VIEW() ****");
		
		//TEST VIEW
		Log.i(TAG, UIElementType.LIST_PROPERTY_WIDGET + " - Call View() method");
		
		if ( records.size() == 0 ) {
			Log.i(TAG, UIElementType.LIST_PROPERTY_WIDGET + " - No records in the list, adding a form...");
			ContainerWidget cont = listProps.add();
			uiControlsList.clear();
			handleContainer(cont, true);
			runTestForUiControls(uiControlsList);
			listProps.confirm();
		}
		
		records   = listProps.getValue();
		int size  = records.size();
		if ( size == 0 ) {
			Log.e(TAG, UIElementType.LIST_PROPERTY_WIDGET + " - No records were found, previously failed to add a new record");
			return;
		}
		
		int index     = rand.nextInt(size);
		Record record = records.get(index);
		
		Log.i(TAG, UIElementType.LIST_PROPERTY_WIDGET + " - Call View() method on the record: '" + record + "'");
		ContainerWidget cont = listProps.view(record.getRecordId());
		uiControlsList.clear();
		handleContainer(cont, true);
		runTestForUiControls(uiControlsList);
    	//================================== END VIEW ==============================//

	}//testListProperty


}
