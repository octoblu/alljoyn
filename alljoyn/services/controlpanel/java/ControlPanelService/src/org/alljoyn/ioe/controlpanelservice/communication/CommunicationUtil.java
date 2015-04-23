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

package org.alljoyn.ioe.controlpanelservice.communication;

import java.lang.reflect.Method;

import org.alljoyn.bus.SessionOpts;
import org.alljoyn.bus.Variant;
import org.alljoyn.ioe.controlpanelservice.ControlPanelException;
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.ActionControlSuper;
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.AlertDialogSuper;
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.ContainerSuper;
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.ControlPanel;
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.HTTPControl;
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.Label;
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.ListPropertyControlSuper;
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.NotificationAction;
import org.alljoyn.ioe.controlpanelservice.communication.interfaces.PropertyControlSuper;

import android.util.Log;

/**
 * Communication utilities class 
 */
public class CommunicationUtil {
	private static final String TAG = "cpan" + CommunicationUtil.class.getSimpleName(); 	
	
	/**
	 * @return MetadataChanged reflection for container 
	 */
	public static Method getContainerMetadataChanged(String name) {
		try {
			return ContainerSuper.class.getMethod(name);
		} catch (NoSuchMethodException nsme) {
			Log.e(TAG, "Not found reflection of " + name + " method");
		}
		return null; 
	}//getContainerMetadataChanged

	/**
	 * @return MetadataChanged reflection for property metadata changed signal
	 */
	public static Method getPropertyMetadataChanged(String name) {
		try {
			return PropertyControlSuper.class.getMethod(name);
		} catch (NoSuchMethodException nsme) {
			Log.e(TAG, "Not found reflection of " + name + " method");
		}
		return null; 
	}//getContainerMetadataChanged

	/**
	 * @return MetadataChanged reflection for property value changed signal
	 */
	public static Method getPropertyValueChanged(String name) {
		try {
			return PropertyControlSuper.class.getMethod(name,
								     	     		    Variant.class
			);
		} catch (NoSuchMethodException nsme) {
			Log.e(TAG, "Not found reflection of " + name + " method");
		}
		return null; 
	}//getContainerMetadataChanged
	
	/**
	 * @return MetadataChanged reflection for action metadata changed signal
	 */
	public static Method getActionMetadataChanged(String name) {
		try {
			return ActionControlSuper.class.getMethod(name);
		} catch (NoSuchMethodException nsme) {
			Log.e(TAG, "Not found reflection of " + name + " method");
		}
		return null; 
	}//getContainerMetadataChanged

	/**
	 * @return reflection for alert dialog metadata changed signal
	 */
	public static Method getAlertDialogMetadataChanged(String name) {
		try {
			return AlertDialogSuper.class.getMethod(name);
		} catch (NoSuchMethodException nsme) {
			Log.e(TAG, "Not found reflection of " + name + " method");
		}
		return null; 
	}//getAlertDialogMetadataChanged

	/**
	 * @return reflection for label widget metadata changed signal
	 */
	public static Method getLabelWidgetMetadataChanged(String name) {
		try {
			return Label.class.getMethod(name);
		} catch (NoSuchMethodException nsme) {
			Log.e(TAG, "Not found reflection of " + name + " method");
		}
		return null; 
	}//getAlertDialogMetadataChanged


	/**
	 * @return reflection for {@link ListPropertyControlSuper} signals
	 */
	public static Method getListPropertyWidgetSignal(String name) {
		try {
			return ListPropertyControlSuper.class.getMethod(name);
		} catch (NoSuchMethodException nsme) {
			Log.e(TAG, "Not found reflection of " + name + " method");
		}
		return null; 
	}//getListPropertyWidgetSignal

	/**
	 * @return reflection for {@link NotificationAction} signal
	 */
	public static Method getNotificationActionDismissSignal(String name) {
		try {
			return NotificationAction.class.getMethod(name);
		} catch (NoSuchMethodException nsme) {
			Log.e(TAG, "Not found reflection of " + name + " method");
		}
		return null; 
	}//getListPropertyWidgetSignal
	
	/**
     * Creates SessionOpts
     * @return SessionOpts to be used to advertise service and ot join session
     */
	public static SessionOpts getSessionOpts(){
		SessionOpts sessionOpts   = new SessionOpts();
	    sessionOpts.traffic       = SessionOpts.TRAFFIC_MESSAGES;   // Use reliable message-based communication to move data between session endpoints
	    sessionOpts.isMultipoint  = false;                          // A session is multi-point if it can be joined multiple times 
	    sessionOpts.proximity     = SessionOpts.PROXIMITY_ANY;      // Holds the proximity for this SessionOpt
	    sessionOpts.transports    = SessionOpts.TRANSPORT_ANY;      // Holds the allowed transports for this SessionOpt
	    return sessionOpts;
	}//getSessionOpts
	
	/**
	 * Test the given interfaces list <br>
	 * If an interface from the given interfaces list is a top level interface then adds it to a resultant mask
	 * @param interfaces The interfaces to test
	 * @return Interfaces resultant mask
	 */
	public static int getInterfaceMask(String... interfaces) {
		int resMask = 0;
		
		if ( interfaces == null ) {
			return  resMask;
		}
		
		for (String iface : interfaces) {
			if ( iface.equals(ControlPanel.IFNAME) ) {
				resMask |= ControlPanel.ID_MASK;
			}
			else if ( iface.equals(HTTPControl.IFNAME) ) {
				resMask |= HTTPControl.ID_MASK;
			}
			else if ( iface.equals(NotificationAction.IFNAME) ) {
				resMask |= NotificationAction.ID_MASK;
			}
		}//for :: interfaces
		
		return resMask;
	}//parseInterfaces

	/**
	 * Checks whether the given numToTest includes the mask
	 * @param numToTest
	 * @param mask
	 * @return TRUE if includes
	 */
	public static boolean maskIncludes(int numToTest, int mask) {
		return (numToTest & mask) == mask;
	}//maskIncludes
	
	/**
	 * Parses the objectPath of the form of: /ControlPanel/{unitId}/{panelId}
	 * @param objPath The objPath to parse
	 * @return Array(0 => unitId, 1 => panelId) 
	 */
	public static String[] parseObjPath(String objPath) throws ControlPanelException {
		
		String[] segments = objPath.split("/");
		int segLength     = segments.length; 
		if ( segLength == 0 ) {
			throw new ControlPanelException("Received a broken object path: '" + objPath + "'");
		}
		
		String panelId = segments[segLength - 1];  // The name of the control panel
		String unitId;                             // The name of the functional unit
		if ( (segLength - 2) >= 0 ) {
			unitId = segments[segLength - 2];
		}
		else {
			unitId = "unknown";
		}
		
		return new String[]{unitId, panelId};
	}//parseObjPath
	
}
