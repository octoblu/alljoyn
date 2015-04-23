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
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.Label;

import android.util.Log;

public class LabelWidgetSignalHandler implements Label {
    private static final String TAG = "cpan" + LabelWidgetSignalHandler.class.getSimpleName();
    
    /**
     * Container widget to be notified about signal receiving
     */
    private LabelWidget labelWidget;

    /**
     * Constructor
     * @param labelWidget
     */
    public LabelWidgetSignalHandler(LabelWidget labelWidget) {
        this.labelWidget = labelWidget;
    }

	@Override
	public short getVersion() throws BusException {
		// TODO Auto-generated method stub
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
	
	@Override
	public String getLabel() throws BusException {
		// TODO Auto-generated method stub
		return null;
	}
	
	/**
	 * @see org.alljoyn.ioe.controlpanelservice.communication.interfaces.Label#MetadataChanged()
	 */
	@Override
	public void MetadataChanged() throws BusException {
        
        String msg = "Device: '" + labelWidget.device.getDeviceId() +
                "', labelWidget: '" + labelWidget.objectPath + "', received METADATA_CHANGED signal";

        Log.d(TAG, msg);

        final ControlPanelEventsListener eventsListener = labelWidget.controlPanel.getEventsListener();
        try {
            labelWidget.refreshProperties();
        } catch (ControlPanelException cpe) {
            msg += ", but failed to refresh the LabelWidget properties";
            Log.e(TAG, msg);
            eventsListener.errorOccurred(labelWidget.controlPanel, msg);
            return;
        }

        //Delegate to the listener on a separate thread
        TaskManager.getInstance().execute( new Runnable() {
			@Override
			public void run() {
				eventsListener.metadataChanged(labelWidget.controlPanel, labelWidget);
			}
		});
	}//MetadataChanged

}
