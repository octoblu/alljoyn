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

import java.util.Collection;

import org.alljoyn.ioe.controlpaneladapter.ControlPanelAdapter;
import org.alljoyn.ioe.controlpanelservice.ControlPanelCollection;
import org.alljoyn.ioe.controlpanelservice.ControlPanelException;
import org.alljoyn.ioe.controlpanelservice.ControlPanelService;
import org.alljoyn.ioe.controlpanelservice.ControllableDevice;
import org.alljoyn.ioe.controlpanelservice.DeviceEventsListener;
import org.alljoyn.ioe.controlpanelservice.ui.DeviceControlPanel;
import org.alljoyn.ns.sampleapp.IoeNotificationApplication;
import org.alljoyn.ns.sampleapp.NotificationServiceControlsActivity;
import org.alljoyn.ns.sampleapp.R;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

/**
 * Manages the Control Panel UI
 */
public class ControlPanelActivity extends Activity implements DeviceEventsListener {
	private static final String TAG = "ioe" + ControlPanelActivity.class.getSimpleName();
	
	private static enum UIState {
		LOAD_PANEL,
		LANGUAGE_FOUND,
		;
	}
	
	//===========================================//
	
	/**
	 * The application
	 */
	private IoeNotificationApplication myApp;
	
	/**
	 * The generic label
	 */
	private TextView label;
	
	/**
	 * Language spinner
	 */
	private Spinner languageSpinner;
	
	/**
	 * Progress bar
	 */
	private ProgressBar progressBar;
	
	/**
	 * The layout that includes all the Control Panel view objects
	 */
	private LinearLayout controlPanelLayout;
	
	/**
	 * The remote device unique name
	 */
	private String sender;
	
	/**
	 * The object path to retrieve the Control Panel
	 */
	private String objPath;

	/**
	 * The appId that sent the Notification with action
	 */
	private String appId;
	
	/**
	 * Control Panel Manager works with {@link ControlPanelAdapter} to create the control panel
	 */
	private ControlPanelManager panelManager;
	
	/**
	 * Control Panel Service
	 */
	private ControlPanelService controlService;
	
	/**
	 * The controllable device to be controlled
	 */
	private ControllableDevice device;
	
	/**
	 * The collection represents control panel per supported language
	 */
	private ControlPanelCollection controlPanelLangColl;
	
	/**
	 * The {@link View} of the Control Panel
	 */
	private View parentVeiw;
	
	/**
	 * {@link AlertDialog} Control panel
	 */
	private AlertDialog alertDialog;
	
	/**
	 * @see android.app.Activity#onCreate(android.os.Bundle)
	 */
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_control_panel);
		
		myApp 			= (IoeNotificationApplication) getApplication();
		controlService  = ControlPanelService.getInstance();
		
		Intent intent = getIntent();
		objPath = intent.getStringExtra("OBJ_PATH");
		sender  = intent.getStringExtra("SENDER");
		appId   = intent.getStringExtra("APP_ID");
	}//onCreate
	
	/**
	 * @see android.app.Activity#onCreateOptionsMenu(android.view.Menu)
	 */
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.control_panel, menu);
		return true;
	}

	/**
	 * @see android.app.Activity#onStart()
	 */
	@Override
	protected void onStart() {
		super.onStart();
		
		setUI();
		
		panelManager = new ControlPanelManager(this);
		
		try {
			controlService.init(myApp.getBusAttachment());
			device = controlService.getControllableDevice(appId, sender);
			device.startSession(this);
		} catch (ControlPanelException cpe) {
			Log.e(TAG, cpe.getMessage());
			backOnError("Failed to initialize Control Panel");
		}
	}//onStart

	/**
	 * @see android.app.Activity#onStop()
	 */
	@Override
	protected void onStop() {
		super.onStop();
		cleanResources();
	}//onStop

	//=================================================//
	//            DEVICE EVENTS LISTENER               //
	//=================================================//	
	
	/**
	 * @see org.alljoyn.ioe.controlpanelservice.DeviceEventsListener#errorOccurred(org.alljoyn.ioe.controlpanelservice.ControllableDevice, java.lang.String)
	 * Error in {@link ControllableDevice} functionality 
	 */
	@Override
	public void errorOccurred(ControllableDevice device, String reason) {
		Log.d(TAG, "Control Panel Error: '" + reason + "'");
		backOnError("Oops, Control Panel Error");
	}//errorOccurred
	
	/**
	 * @see org.alljoyn.ioe.controlpanelservice.DeviceEventsListener#sessionEstablished(org.alljoyn.ioe.controlpanelservice.ControllableDevice, java.util.Collection)
	 */
	@Override
	public void sessionEstablished(ControllableDevice device, Collection<ControlPanelCollection> controlPanels) {
		try {
			
			controlPanelLangColl = this.device.createNotificationAction(objPath);
			if ( controlPanelLangColl.getControlPanels().size() == 0 ) {
				backOnError("No Control Panel was retreived from the remote device");
			}
			else {
				
				runOnUiThread( new Runnable() {
					@Override
					public void run() {
						createLangSpinner();
						toggleUI(UIState.LANGUAGE_FOUND, R.string.label_language);
					}
				});
			}//else
		} catch (ControlPanelException cpe) {
			Log.d(TAG, "Failed to establish the session, Error: '" + cpe.getMessage() + "'");
			backOnError("Failed to establish connection with the device");
		}
	}//sessionEstablished
	
	/**
	 * @see org.alljoyn.ioe.controlpanelservice.DeviceEventsListener#sessionLost(org.alljoyn.ioe.controlpanelservice.ControllableDevice)
	 */
	@Override
	public void sessionLost(ControllableDevice device) {
		Log.d(TAG, "Session Lost received");
		backOnError("The session with the controllable device has been lost");
	}//sessionLost
	
	//==================================================//
	
	/**
	 * Initialize UI
	 */
	private void setUI() {
		
		label 			    = (TextView) findViewById(R.id.textLabel);
		languageSpinner     = (Spinner) findViewById(R.id.languageSpinner);
		progressBar         = (ProgressBar) findViewById(R.id.progressBar);
		controlPanelLayout  = (LinearLayout) findViewById(R.id.controlpanel_layout);
		
		toggleUI(UIState.LOAD_PANEL, R.string.label_loading);
	}//setUI

	/**
	 * Change UI state
	 * @param state The UI state 
	 * @param labelTextId the resource id of the label string
	 */
	private void toggleUI(UIState state, int labelTextId) {
		
		if ( state == UIState.LOAD_PANEL ) {
			progressBar.setVisibility(View.VISIBLE);
			languageSpinner.setVisibility(View.GONE);
		}
		else if ( state == UIState.LANGUAGE_FOUND ) {
			progressBar.setVisibility(View.GONE);
			languageSpinner.setVisibility(View.VISIBLE);
		}
		
		label.setText(labelTextId);
	}//toggleUI
	
	/**
	 * If an error happens, show {@link Toast} with a given msg and go back to the previous {@link NotificationServiceControlsActivity}
	 * @param msg The error message to be presented in {@link Toast}
	 */
	private void backOnError(String msg) {
		myApp.showToast(msg);
		finish();
	}//backOnError
	
	/**
	 * Build and show language spinner
	 */
	private void createLangSpinner() {
		
		String[] langs = controlPanelLangColl.getLanguages().toArray(new String[0]);
		
		ArrayAdapter<CharSequence> adapter = new  ArrayAdapter<CharSequence>(this, android.R.layout.simple_spinner_item);
		adapter.add("LANGUAGE");
		adapter.addAll(langs);
		
		languageSpinner.setAdapter(adapter);
		languageSpinner.setSelection(0);
		languageSpinner.setOnItemSelectedListener( new OnItemSelectedListener() {
			
			@Override
			public void onItemSelected(AdapterView<?> parentView, View selectedItem, int pos, long id) {
				
				if ( pos == 0 ) {
					Log.d(TAG, "Nothing selected");
					return;
				}
				
				String selectedLang = parentView.getSelectedItem().toString();
				Log.d(TAG, "The selected language is: '" + selectedLang + "'");
				
				toggleUI(UIState.LOAD_PANEL, R.string.label_loading);
				findPanel(selectedLang);
			}//onItemSelected

			@Override
			public void onNothingSelected(AdapterView<?> parentView) {
			}
		});
		
	}//createLangSpinner
	
	/**
	 * Find the panel in the received ControlPanelLangCollection <br>
	 * Calls to build the panel
	 * @param lang
	 */
	private void findPanel(String lang) {
		
		for ( DeviceControlPanel panel : controlPanelLangColl.getControlPanels() ) {
			if ( lang.equals(panel.getLanguage()) ) {
				panelManager.buildPanel(panel);
				break;
			}
		}//for
		
	}//findPanel
	
	/**
	 * Is called by the {@link ControlPanelManager} when the Control Panel {@link View} is ready
	 * @param view The control panel {@link View} object to be inflated in the Control Panel layout <br>
	 * If is NULL it means that a failure occurred in retrieving the Control Panel elements
	 */
	void onReadyPanelView(final View view) {
		
		if ( view == null ) {
			backOnError("Oops, Failed to load the panel elements");
			return;
		}
		
		parentVeiw = view;
		
		runOnUiThread( new Runnable() {
			@Override
			public void run() {
				controlPanelLayout.removeAllViews();
				controlPanelLayout.addView(view);
				toggleUI(UIState.LANGUAGE_FOUND, R.string.label_language);
			}
		});
	}//onReadyPanelView
	
	/**
	 * Is called by the {@link ControlPanelManager} when the Control Panel {@link AlertDialog} is ready
	 * @param dialog The {@link AlertDialog} to show
	 */
	void onReadyPanelDialog(AlertDialog dialog) {
		
		this.alertDialog = dialog;
		alertDialog.setCancelable(false);
		alertDialog.setCanceledOnTouchOutside(false);
		
		runOnUiThread( new Runnable() {
			@Override
			public void run() {
				toggleUI(UIState.LANGUAGE_FOUND, R.string.label_language);
				alertDialog.show();
			}
		});
	}//onRadyPanelDialog
	
	/**
	 * Is called when the Control Panel should be dismissed
	 */
	void onPanelDismissed() {
		
		if ( parentVeiw != null ) {
			controlPanelLayout.removeAllViews();
			parentVeiw = null;
		}
		else if ( alertDialog != null ) {
			alertDialog.dismiss();
			alertDialog = null;
		}
	}//onPanelDismissed
	
	/**
	 * Close all the opened {@link ControlPanelService} resources 
	 */
	private void cleanResources() {
		
		Log.d(TAG, "Shutdown Control Panel Service");
		
		panelManager.clear();
		panelManager = null;
		
		if ( device != null ) {
			
			if ( controlPanelLangColl != null ) {
				device.removeNotificationAction(controlPanelLangColl);
			}
			
			controlService.stopControllableDevice(device);
			device = null;
		}
		
		parentVeiw = null;
		
		if ( alertDialog != null ) {
			alertDialog.dismiss();
			alertDialog = null;
		}
		
		controlService.shutdown();
	}//closeResources

}
