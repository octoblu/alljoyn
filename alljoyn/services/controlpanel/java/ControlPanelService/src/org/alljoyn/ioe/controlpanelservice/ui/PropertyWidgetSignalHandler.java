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

import java.util.Map;

import org.alljoyn.bus.BusException;
import org.alljoyn.bus.Variant;
import org.alljoyn.ioe.controlpanelservice.ControlPanelException;
import org.alljoyn.ioe.controlpanelservice.communication.TaskManager;
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.PropertyControl;
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.PropertyControlSecured;

import android.util.Log;

/**
 * The class is a signal handler that is responsible to receive signals of PropertyControl interface. <br>
 *  Signals: <br>
 *  - ValueChanged    <br>
 *  - MetadataChanged
 */
public class PropertyWidgetSignalHandler implements PropertyControlSecured, PropertyControl {
	private static final String TAG = "cpan" + PropertyWidgetSignalHandler.class.getSimpleName();

	
	/**
	 * Property widget to be notified about signal receiving
	 */
	private PropertyWidget propertyWidget;

	/**
	 * @param propertyWidget
	 */
	public PropertyWidgetSignalHandler(PropertyWidget propertyWidget) {
		this.propertyWidget = propertyWidget;
	}

	/**
	 * @see org.alljoyn.ioe.controlpanelservice.communication.interfaces.PropertyControl#getVersion()
	 */
	@Override
	public short getVersion() throws BusException {
		return 0;
	}

	/**
	 * @see org.alljoyn.ioe.controlpanelservice.communication.interfaces.PropertyControl#getValue()
	 */
	@Override
	public Variant getValue() throws BusException {
		return null;
	}

	/**
	 * @see org.alljoyn.ioe.controlpanelservice.communication.interfaces.PropertyControl#setValue(org.alljoyn.bus.Variant)
	 */
	@Override
	public void setValue(Variant value) throws BusException {
	}


	@Override
	public int getStates() throws BusException {
		return 0;
	}

	@Override
	public Map<Short, Variant> getOptParams() throws BusException {
		return null;
	}
	

	/**
	 * @see org.alljoyn.ioe.controlpanelservice.communication.interfaces.PropertyControl#ValueChanged(org.alljoyn.bus.Variant)
	 */
	@Override
	public void ValueChanged(Variant value) throws BusException {
		
	    ControlPanelEventsListener eventsListener = propertyWidget.controlPanel.getEventsListener();
	    
	    String msg = "Device: '" + propertyWidget.device.getDeviceId() +
                "', PropertyWidget: '" + propertyWidget.objectPath + "', received VALUE_CHANGED signal";
    
	    Log.d(TAG, msg);
	    
	    try {
			Object unmarshVal = propertyWidget.unmarshalCurrentValue(value);
			Log.d(TAG, "The new property: '" + propertyWidget.objectPath + "' value is: '" + unmarshVal +  "'");
			eventsListener.valueChanged(propertyWidget.controlPanel, propertyWidget, unmarshVal);
		} catch (ControlPanelException cpe) {
			msg += ", but failed to unmarshal the received data, Error: '" + cpe.getMessage() + "'";
			Log.e(TAG, msg);
			eventsListener.errorOccurred(propertyWidget.controlPanel, msg);
		}
	}//ValueChanged

	/**
	 * @see org.alljoyn.ioe.controlpanelservice.communication.interfaces.PropertyControl#MetadataChanged()
	 */
	@Override
	public void MetadataChanged() throws BusException {
        
        String msg = "Device: '" + propertyWidget.device.getDeviceId() +
                "', PropertyWidget: '" + propertyWidget.objectPath + "', received METADATA_CHANGED signal";

        Log.d(TAG, msg);
        
        final ControlPanelEventsListener eventsListener = propertyWidget.controlPanel.getEventsListener();
        
        try {
        	propertyWidget.refreshProperties();
        }
        catch(ControlPanelException cpe) {
        	 msg += ", but failed to refresh the widget properties";
        	 Log.e(TAG, msg);
             eventsListener.errorOccurred(propertyWidget.controlPanel, msg);
             return;
        }
        
        //Delegate to the listener on a separate thread
        TaskManager.getInstance().execute( new Runnable() {
			@Override
			public void run() {
				eventsListener.metadataChanged(propertyWidget.controlPanel, propertyWidget);
			}
		});
	}//MetadataChanged
	
}
