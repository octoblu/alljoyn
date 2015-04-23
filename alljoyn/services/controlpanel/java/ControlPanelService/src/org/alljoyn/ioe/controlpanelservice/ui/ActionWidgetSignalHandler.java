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
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.ActionControl;
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.ActionControlSecured;

import android.util.Log;

/**
 * The class is a signal handler that is responsible to receive signals of ActionControl interface. <br>
 *  Signals: <br>
 *  - MetadataChanged
 */
public class ActionWidgetSignalHandler implements ActionControl, ActionControlSecured {
	private static final String TAG = "cpan" + ActionWidgetSignalHandler.class.getSimpleName();

    /**
     * Container widget to be notified about signal receiving
     */
    private ActionWidget actionWidget;
	
	/**
	 * Constructor
	 * @param actionWidget
	 */
	public ActionWidgetSignalHandler(ActionWidget actionWidget) {
		this.actionWidget = actionWidget;
	}

	/**
	 * @see org.alljoyn.ioe.controlpanelservice.communication.interfaces.ActionControl#getVersion()
	 */
	@Override
	public short getVersion() throws BusException {
		return 0;
	}

	@Override
	public int getStates() throws BusException {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public Map<Short, Variant> getOptParams() throws BusException {
		// TODO Auto-generated method stub
		return null;
	}	
	
	/**
	 * @see org.alljoyn.ioe.controlpanelservice.communication.interfaces.ActionControl#Exec()
	 */
	@Override
	public void Exec() {
	}

	/**
	 * @see org.alljoyn.ioe.controlpanelservice.communication.interfaces.ActionControl#MetadataChanged()
	 */
	@Override
	public void MetadataChanged() {
       
        String msg = "Device: '" + actionWidget.device.getDeviceId() +
                "', ActionWidget: '" + actionWidget.objectPath + "', received METADATA_CHANGED signal";
        
        Log.d(TAG, msg);
        
        final ControlPanelEventsListener eventsListener = actionWidget.controlPanel.getEventsListener();
        try{
        	actionWidget.refreshProperties();
        }
        catch(ControlPanelException cpe) {
            msg += ", but failed to refresh the ActionWidget properties";
            Log.e(TAG, msg);
            eventsListener.errorOccurred(actionWidget.controlPanel, msg);
            return;
        }

        //Delegate to the listener on a separate thread
        TaskManager.getInstance().execute( new Runnable() {
			@Override
			public void run() {
				eventsListener.metadataChanged(actionWidget.controlPanel, actionWidget);
			}
		});
	}//MetadataChanged

}
