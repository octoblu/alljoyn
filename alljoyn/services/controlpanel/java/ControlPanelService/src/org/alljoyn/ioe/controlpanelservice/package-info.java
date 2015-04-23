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
/**
 * The package includes classes that allow to start the control panel service {@link org.alljoyn.ioe.controlpanelservice.ControlPanelService}, 
 * and manage the list of controllable devices {@link org.alljoyn.ioe.controlpanelservice.ControllableDevice}. <br>
 * The list of controllable devices is managed by the {@link org.alljoyn.ioe.controlpanelservice.DeviceRegistry} interface
 * and its default implementation {@link org.alljoyn.ioe.controlpanelservice.DefaultDeviceRegistry} <br>  
 * For each controllable device there is a list of the device control panels {@link org.alljoyn.ioe.controlpanelservice.ui.DeviceControlPanel}
 * that is passed to the user when {@link org.alljoyn.ioe.controlpanelservice.ControllableDevice#startSession(DeviceEventsListener)}
 * is called. By using the {@link org.alljoyn.ioe.controlpanelservice.ui.DeviceControlPanel} a user may call
 * {@link org.alljoyn.ioe.controlpanelservice.ui.DeviceControlPanel#getRootElement(org.alljoyn.ioe.controlpanelservice.ui.ControlPanelEventsListener)} to retrieve the root
 * UI container
 */
package org.alljoyn.ioe.controlpanelservice;
