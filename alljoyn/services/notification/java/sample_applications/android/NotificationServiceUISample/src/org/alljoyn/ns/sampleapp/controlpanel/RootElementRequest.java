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

package org.alljoyn.ns.sampleapp.controlpanel;

import org.alljoyn.ioe.controlpanelservice.ControlPanelException;
import org.alljoyn.ioe.controlpanelservice.ui.DeviceControlPanel;

import android.os.AsyncTask;

/**
 * Calls {@link DeviceControlPanel#getRootElement(org.alljoyn.ioe.controlpanelservice.ui.ControlPanelEventsListener)}
 */
public class RootElementRequest extends AsyncTask<DeviceControlPanel, Void, Object> {

	/**
	 * Control Panel manager
	 */
	private final ControlPanelManager manager;

	/**
	 * Constructor
	 */
	public RootElementRequest(ControlPanelManager manager) {
		super();
		this.manager = manager;
	}

	/**
	 * @see android.os.AsyncTask#doInBackground(java.lang.Object[])
	 */
	@Override
	protected Object doInBackground(DeviceControlPanel... controlPanel) {
		
		try {
			return controlPanel[0].getRootElement(manager);
		} catch (ControlPanelException cpe) {
			return cpe;
		}
	}//doInBackground

	/**
	 * @see android.os.AsyncTask#onPostExecute(java.lang.Object)
	 */
	@Override
	protected void onPostExecute(Object result) {
		manager.onReadyRootElementRequest(result);
	}//onPostExecute
	
}
