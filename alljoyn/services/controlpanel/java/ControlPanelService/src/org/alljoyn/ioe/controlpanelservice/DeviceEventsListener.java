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

package org.alljoyn.ioe.controlpanelservice;

import java.util.Collection;

import org.alljoyn.ioe.controlpanelservice.ui.DeviceControlPanel;

/**
 *  The interface to be implemented in order to receive {@link ControllableDevice} relevant events
 */
public interface DeviceEventsListener {
	
	/**
	 * Notify the interface implementer about session establishment with the remote device
	 * @param device The {@link ControllableDevice}
	 * @param controlPanelContainer Used to request the Root UI Container and a relevant to it information
	 * @see DeviceControlPanel 
	 */
	public void sessionEstablished(ControllableDevice device, Collection<ControlPanelCollection> controlPanelContainer);
	
	/**
	 * Notify the interface implementer about loosing session with the remote device
	 * @param device The {@link ControllableDevice} where the session has lost
	 */
	public void sessionLost(ControllableDevice device);
	
	/**
	 * Notify the interface implementer about an error in the device activities
	 * @param device The {@link ControllableDevice} where the error has occurred
	 * @param reason The error reason
	 */
	public void errorOccurred(ControllableDevice device, String reason);
}
