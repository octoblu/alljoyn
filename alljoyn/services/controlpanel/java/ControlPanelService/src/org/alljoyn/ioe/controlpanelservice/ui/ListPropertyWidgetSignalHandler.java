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

import java.util.List;
import java.util.Map;

import org.alljoyn.bus.BusException;
import org.alljoyn.bus.Variant;
import org.alljoyn.ioe.controlpanelservice.ControlPanelException;
import org.alljoyn.ioe.controlpanelservice.communication.TaskManager;
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.ListPropertyControl;
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.ListPropertyControlSecured;
import org.alljoyn.ioe.controlpanelservice.ui.ListPropertyWidget.Record;
import org.alljoyn.ioe.controlpanelservice.ui.ajstruct.ListPropertyWidgetRecordAJ;

import android.util.Log;

/**
 * The class is a signal handler that is responsible to receive signals of ListPropertyControl interface. <br>
 *  Signals: <br>
 *  - ValueChanged    <br>
 *  - MetadataChanged
 */

public class ListPropertyWidgetSignalHandler implements ListPropertyControl, ListPropertyControlSecured {
    private static final String TAG = "cpan" + ListPropertyWidgetSignalHandler.class.getSimpleName();

    /**
     * The {@link ListPropertyWidget} to be notified about the receiving the signals
     */
    private ListPropertyWidget listPropertyWidget;
	
	/**
	 * Constructor
	 * @param listPropertyWidget
	 */
	public ListPropertyWidgetSignalHandler(ListPropertyWidget listPropertyWidget) {
		this.listPropertyWidget = listPropertyWidget;
	}

	/**
	 * @see org.alljoyn.ioe.controlpanelservice.communication.interfaces.ListPropertyControl#getVersion()
	 */
	@Override
	public short getVersion() throws BusException {
		return 0;
	}

	/**
	 * @see org.alljoyn.ioe.controlpanelservice.communication.interfaces.ListPropertyControl#getStates()
	 */
	@Override
	public int getStates() throws BusException {
		return 0;
	}

	/**
	 * @see org.alljoyn.ioe.controlpanelservice.communication.interfaces.ListPropertyControl#getOptParams()
	 */
	@Override
	public Map<Short, Variant> getOptParams() throws BusException {
		return null;
	}

	/**
	 * @see org.alljoyn.ioe.controlpanelservice.communication.interfaces.ListPropertyControl#getValue()
	 */
	@Override
	public ListPropertyWidgetRecordAJ[] getValue() throws BusException {
		return null;
	}

	/**
	 * @see org.alljoyn.ioe.controlpanelservice.communication.interfaces.ListPropertyControl#Add()
	 */
	@Override
	public void Add() throws BusException {
	}

	/**
	 * @see org.alljoyn.ioe.controlpanelservice.communication.interfaces.ListPropertyControl#Delete(short)
	 */
	@Override
	public void Delete(short recordId) throws BusException {
	}

	/**
	 * @see org.alljoyn.ioe.controlpanelservice.communication.interfaces.ListPropertyControl#View(short)
	 */
	@Override
	public void View(short recordId) throws BusException {
	}

	/**
	 * @see org.alljoyn.ioe.controlpanelservice.communication.interfaces.ListPropertyControl#Update(short)
	 */
	@Override
	public void Update(short recordId) throws BusException {
	}

	/**
	 * @see org.alljoyn.ioe.controlpanelservice.communication.interfaces.ListPropertyControl#Confirm()
	 */
	@Override
	public void Confirm() throws BusException {
	}

	/**
	 * @see org.alljoyn.ioe.controlpanelservice.communication.interfaces.ListPropertyControl#Cancel()
	 */
	@Override
	public void Cancel() throws BusException {
		// TODO Auto-generated method stub

	}

	/**
	 * @see org.alljoyn.ioe.controlpanelservice.communication.interfaces.ListPropertyControl#ValueChanged()
	 */
	@Override
	public void ValueChanged() throws BusException {
		
        ControlPanelEventsListener eventsListener = listPropertyWidget.controlPanel.getEventsListener();
            
        String msg = "Device: '" + listPropertyWidget.device.getDeviceId() +
                "', ListProperty: '" + listPropertyWidget.objectPath + "', received VALUE_CHANGED signal";
    
        Log.d(TAG, msg);
       
        try {
            List<Record> records = listPropertyWidget.getValue();
            eventsListener.valueChanged(listPropertyWidget.controlPanel, listPropertyWidget, records);
        } catch (ControlPanelException cpe) {
            msg += ", but failed to read the new values, Error: '" + cpe.getMessage() + "'";
            Log.e(TAG, msg);
            eventsListener.errorOccurred(listPropertyWidget.controlPanel, msg);
        }
	}//ValueChanged

	/**
	 * @see org.alljoyn.ioe.controlpanelservice.communication.interfaces.ListPropertyControl#MetadataChanged()
	 */
	@Override
	public void MetadataChanged() throws BusException {
        
        String msg = "Device: '" + listPropertyWidget.device.getDeviceId() +
                "', ListProperty: '" + listPropertyWidget.objectPath + "', received METADATA_CHANGED signal";

        Log.d(TAG, msg);

        final ControlPanelEventsListener eventsListener = listPropertyWidget.controlPanel.getEventsListener();

        try {
            listPropertyWidget.refreshProperties();
        }
        catch(ControlPanelException cpe) {
             msg += ", but failed to refresh the widget properties";
             Log.e(TAG, msg);
             eventsListener.errorOccurred(listPropertyWidget.controlPanel, msg);
             return;
        }

        //Delegate to the listener on a separate thread
        TaskManager.getInstance().execute( new Runnable() {
			@Override
			public void run() {
				eventsListener.metadataChanged(listPropertyWidget.controlPanel, listPropertyWidget);
			}
		});
        
	}//MetadataChanged

}
