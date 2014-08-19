/******************************************************************************
 * Copyright (c) 2014, AllSeen Alliance. All rights reserved.
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

import org.alljoyn.ioe.controlpaneladapter.ControlPanelAdapter;
import org.alljoyn.ioe.controlpaneladapter.ControlPanelExceptionHandler;
import org.alljoyn.ioe.controlpanelservice.ControlPanelException;
import org.alljoyn.ioe.controlpanelservice.ui.AlertDialogWidget;
import org.alljoyn.ioe.controlpanelservice.ui.ContainerWidget;
import org.alljoyn.ioe.controlpanelservice.ui.ControlPanelEventsListener;
import org.alljoyn.ioe.controlpanelservice.ui.DeviceControlPanel;
import org.alljoyn.ioe.controlpanelservice.ui.UIElement;
import org.alljoyn.ioe.controlpanelservice.ui.UIElementType;
import org.alljoyn.ns.sampleapp.IoeNotificationApplication;

import android.app.AlertDialog;
import android.util.Log;
import android.view.View;

/**
 * Manages the control panel creation and works with {@link ControlPanelAdapter}
 */
class ControlPanelManager implements ControlPanelExceptionHandler, ControlPanelEventsListener {
	private static final String TAG = "ioe" + ControlPanelManager.class.getSimpleName();
	
	/**
	 * This application object
	 */
	private IoeNotificationApplication myApp;
	
	/**
	 * Control Panel Activity
	 */
	private ControlPanelActivity panelActivity;
	
	/**
	 * Control panel adapter creates the panel UI
	 */
	private ControlPanelAdapter panelAdapter;

	/**
	 * The asynchronous task that is called to retrieve the root element container  
	 */
	private RootElementRequest rootElementRequester;
	
	/**
	 * Gets TRUE when the object is initialized and FALSE when the object is cleaned.
	 * This should prevent usage of this object after it was cleaned by the clear method
	 */
	private volatile boolean isObjValid;
	
	/**
	 * Constructor
	 * @param panelActivity Control Panel Activity
	 */
	public ControlPanelManager(ControlPanelActivity panelActivity) {
		
		this.panelActivity   = panelActivity;
		isObjValid           = true;
		myApp                = (IoeNotificationApplication) panelActivity.getApplication();
		panelAdapter         = new ControlPanelAdapter(panelActivity, this);
	}
	
	/**
	 * Cleans this object
	 */
	public void clear() {

		isObjValid    = false;
		myApp         = null;
		panelActivity = null;
		panelAdapter  = null;
		
		if ( rootElementRequester != null && !rootElementRequester.isCancelled() ) {
			rootElementRequester.cancel(true);
			rootElementRequester = null;
		}
	}//clean
	
	/**
	 * Retrieves the Control Panel {@link UIElement} and use {@link ControlPanelAdapter} to build the UI  
	 */
	public void buildPanel(DeviceControlPanel panel) {
		rootElementRequester = new RootElementRequest(this);
		rootElementRequester.execute(panel);
	}//buildPanel
	
	
	/**
	 * @see org.alljoyn.ioe.controlpaneladapter.ControlPanelExceptionHandler#handleControlPanelException(org.alljoyn.ioe.controlpanelservice.ControlPanelException)
	 */
	@Override
	public void handleControlPanelException(ControlPanelException cpe) {
		
		if ( !isObjValid ) {
			Log.w(TAG, "The object has already been cleaned, not valid to be used anymore, returning");
			return;
		}
		
		Log.d(TAG, "A fail has happened in ControlPanelAgapter: '" + cpe.getMessage() + "'");
		myApp.showToast("Oops, failed to execute an action");
	}//handleControlPanelException
	

	/**
	 * Is called by the {@link RootElementRequest} when the request is completed
	 * @param result May be either {@link ControlPanelException} or {@link UIElement}
	 */
	void onReadyRootElementRequest(Object result) {
		
		if ( !isObjValid ) {
			Log.w(TAG, "The object has already been cleaned, not valid to be used anymore, returning");
			return;
		}
		
		if ( result instanceof ControlPanelException ) {
			
			ControlPanelException cpe = (ControlPanelException) result;
			Log.d(TAG, "Failed to retrieve control panel elements, Error: '" + cpe.getMessage() + "'");
			panelActivity.onReadyPanelView(null);
			return;
		}
		
		UIElement uiElement = (UIElement) result;
		if ( uiElement.getElementType() == UIElementType.CONTAINER ) {
			
			View view = panelAdapter.createContainerView( (ContainerWidget)uiElement );
			panelActivity.onReadyPanelView(view);
		}
		else if ( uiElement.getElementType() == UIElementType.ALERT_DIALOG ) {
			
			AlertDialog alertDialog = panelAdapter.createAlertDialog( (AlertDialogWidget)uiElement );
			panelActivity.onReadyPanelDialog(alertDialog);
		}
	}//onRootElementRequestReady
	
	
	//=================================================//
	//            CONTROL PANEL EVENTS LISTENER        //
	//=================================================//	
	
	/**
	 * @see org.alljoyn.ioe.controlpanelservice.ui.ControlPanelEventsListener#errorOccurred(org.alljoyn.ioe.controlpanelservice.ui.DeviceControlPanel, java.lang.String)
	 */
	@Override
	public void errorOccurred(DeviceControlPanel panel, String reason) {
		
		if ( !isObjValid ) {
			Log.w(TAG, "The object has already been cleaned, not valid to be used anymore, returning");
			return;
		}
		
		Log.d(TAG, "Control panel error has occurred, Error: '" + reason + "'");
		myApp.showToast("Oops, Control Panel error has occurred");
	}//errorOccurred

	
	/**
	 * @see org.alljoyn.ioe.controlpanelservice.ui.ControlPanelEventsListener#metadataChanged(org.alljoyn.ioe.controlpanelservice.ui.DeviceControlPanel, org.alljoyn.ioe.controlpanelservice.ui.UIElement)
	 */
	@Override
	public void metadataChanged(DeviceControlPanel panel, final UIElement uiElement) {
		
		if ( !isObjValid ) {
			Log.w(TAG, "The object has already been cleaned, not valid to be used anymore, returning");
			return;
		}
		
		panelActivity.runOnUiThread( new Runnable() {
			@Override
			public void run() {
				panelAdapter.onMetaDataChange(uiElement);
			}
		});
	}//metadataChanged

	/**
	 * @see org.alljoyn.ioe.controlpanelservice.ui.ControlPanelEventsListener#notificationActionDismiss(org.alljoyn.ioe.controlpanelservice.ui.DeviceControlPanel)
	 */
	@Override
	public void notificationActionDismiss(DeviceControlPanel device) {
		
		if ( !isObjValid ) {
			Log.w(TAG, "The object has already been cleaned, not valid to be used anymore, returning");
			return;
		}
		
		panelActivity.runOnUiThread( new Runnable() {
			@Override
			public void run() {
				panelActivity.onPanelDismissed();
			}
		});
	}//notificationActionDismiss
	
	/**
	 * @see org.alljoyn.ioe.controlpanelservice.ui.ControlPanelEventsListener#valueChanged(org.alljoyn.ioe.controlpanelservice.ui.DeviceControlPanel, org.alljoyn.ioe.controlpanelservice.ui.UIElement, java.lang.Object)
	 */
	@Override
	public void valueChanged(DeviceControlPanel panel, final UIElement uiElement, final Object value) {
		
		if ( !isObjValid ) {
			Log.w(TAG, "The object has already been cleaned, not valid to be used anymore, returning");
			return;
		}
		
		panelActivity.runOnUiThread( new Runnable() {
			@Override
			public void run() {
				panelAdapter.onValueChange(uiElement, value);
			}
		});
	}//valueChanged
	
}
