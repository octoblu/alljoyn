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


/**
 *  The interface to be implemented in order to receive {@link DeviceControlPanel} relevant events
 */
public interface ControlPanelEventsListener {
	
	/**
	 * Notify the interface implementer about the changed value in the {@link UIElement}
	 * @param panel {@link DeviceControlPanel} where the value has changed
	 * @param uielement The {@link UIElement} where the change has occurred
	 * @param newValue The new value
	 */
	public void valueChanged(DeviceControlPanel panel, UIElement uielement, Object newValue);
	
	/**
	 * Notify the interface implementer about the metadata changed in the {@link UIElement}
	 * @param panel {@link DeviceControlPanel} where the metadata has changed
	 * @param uielement The {@link UIElement} where the change has occurred
	 */
	public void metadataChanged(DeviceControlPanel panel, UIElement uielement);
	
	/**
	 * Dismiss the NotificationAction of the given {@link DeviceControlPanel} 
	 * @param panel The NotificationAction control panel to dismissed
	 */
	public void notificationActionDismiss(DeviceControlPanel panel);
	
	/**
	 * Notify the interface implementer about an error in the {@link DeviceControlPanel} activities
	 * @param panel {@link DeviceControlPanel} where the error has occurred
	 * @param reason The error reason
	 */
	public void errorOccurred(DeviceControlPanel panel, String reason);
	
}
