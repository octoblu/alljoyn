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

import java.util.Map;

/**
 * Used as a callback interface to inform the interface implementer about new discovered devices
 * and changes in their reachability state
 */
public interface DeviceRegistry {
	
	/**
	 * Called when a new controllable device was found in the control panel service proximity <br>
	 * @param device
	 */
	public void foundNewDevice(ControllableDevice device);
	
	/**
	 * Called when a controllable device left the control panel service proximity <br>
	 * The method implementation should be thread safe 
	 * @param device The controllable device
	 * @param isReachable Whether the device is in reachable state
	 */
	public void reachabilityChanged(ControllableDevice device, boolean isReachable);
	
	/**
	 * Remove a device from registry <br>
	 * When a device is removed from the registry it will be inserted back after foundDevice method is called 
	 * @param device
	 */
	public void removeDevice(ControllableDevice device);
	
	/**
	 * @return Returns collection of controllable devices
	 */
	public Map<String, ControllableDevice> getDevices();
	
}
