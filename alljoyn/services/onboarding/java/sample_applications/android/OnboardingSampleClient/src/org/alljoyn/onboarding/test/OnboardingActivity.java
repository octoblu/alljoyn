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

package org.alljoyn.onboarding.test;

import java.util.Arrays;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import org.alljoyn.onboarding.OnboardingService.AuthType;
import org.alljoyn.onboarding.transport.MyScanResult;
import org.alljoyn.onboarding.transport.OBLastError;
import org.alljoyn.onboarding.transport.ScanInfo;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.os.AsyncTask;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.util.Pair;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;

/**
 * The OnboardingActivity displays all the onboarding data of the
 * device and enable the user to do all the onboarding service actions.
 * (Display the onboarding version, state, last error and enables the following actions:
 * Get the scanInfo from the device, do onboarding (configure a network and connect to it) and offboarding
 */
public class OnboardingActivity extends Activity {

	//General
	protected static final String TAG = OnboardingApplication.TAG;
	private OnboardingApplication m_application;
	private BroadcastReceiver m_receiver;
	private SoftAPDetails m_device;
	private Timer m_timer;
	private int m_tasksToPerform = 0;
	private ProgressDialog m_loadingPopup;
	private AlertDialog m_passwordAlertDialog;

	//Current network
	private TextView m_currentNetwork;
	
	//Version and other properties
	private TextView m_onbaordingVersion;
	private TextView m_lastErrorCodeValue;
	private TextView m_lastErrorMsgValue;
	private TextView m_stateValue;

	//Scan info
	private Spinner m_scanInfoData;
	private ArrayAdapter<MyScanResult> m_scanInfoAdapter;
	private TextView m_scanInfoAge;
	private OnItemSelectedListener m_scanInfoListener;
	
	//Network items
	private String m_networkName;
	private String m_networkPassword;
	private short m_networkAuthType;
	private EditText m_networkNameEditText;
	private EditText m_networkPasswordEditText;
	private Spinner m_authTypeSpinner;
	private ArrayAdapter<AuthType> m_authTypeAdapter;
	private Button m_configureButton;
	private Button m_connectButton;


	//====================================================================
	/* (non-Javadoc)
	 * @see android.app.Activity#onCreate(android.os.Bundle)
	 */
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		
		super.onCreate(savedInstanceState);
		
		//General
		setContentView(R.layout.onboarding_layout);
		String deviceId = getIntent().getStringExtra(Keys.Extras.EXTRA_DEVICE_ID);
		m_application = (OnboardingApplication)getApplication();
		m_device = m_application.getDevice(deviceId);
		if(m_device == null){
			closeScreen();
			return;
		}
		
		startOnboardingSession();//Start the onboarding client and crate a session with it.
		
		m_loadingPopup = new ProgressDialog(this);
		
		//Current Network 
		m_currentNetwork = (TextView) findViewById(R.id.current_network_name);
		String ssid = m_application.getIskWifiManager().getCurrentNetworkSSID();
		m_currentNetwork.setText(getString(R.string.current_network, ssid));
		
		//Version and other properties
		m_onbaordingVersion = (TextView)findViewById(R.id.onboarding_version_value);
		m_lastErrorCodeValue = (TextView)findViewById(R.id.last_error_code_value);
		m_lastErrorMsgValue = (TextView)findViewById(R.id.last_error_msg_value);
		m_stateValue = (TextView)findViewById(R.id.state_value);
		
		//Scan info
		m_scanInfoAdapter = new ArrayAdapter<MyScanResult>(OnboardingActivity.this, android.R.layout.simple_spinner_item);
		m_scanInfoAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		m_scanInfoData = (Spinner) findViewById(R.id.scan_info_data_value);
		m_scanInfoAge = (TextView) findViewById(R.id.scan_info_age_value);
		
		//Network elements
		m_networkNameEditText = (EditText) findViewById(R.id.network_name);
		m_networkPasswordEditText = (EditText)findViewById(R.id.network_password);
		m_authTypeSpinner = (Spinner) findViewById(R.id.auth_type);
		
		TextWatcher textWatcher = new TextWatcher() {
			
			@Override
			public void onTextChanged(CharSequence s, int start, int before, int count) {
			}
			
			@Override
			public void beforeTextChanged(CharSequence s, int start, int count, int after) {
			}
			
			@Override
			public void afterTextChanged(Editable s) {
				if(m_networkNameEditText.getText().length() == 0)
					m_configureButton.setEnabled(false);
				else
					m_configureButton.setEnabled(true);
			}
		};
		m_networkNameEditText.addTextChangedListener(textWatcher);
		
		m_configureButton = (Button)findViewById(R.id.configure_button);
		m_connectButton = (Button)findViewById(R.id.connect_button);
		m_connectButton.setEnabled(false);
		
		initPasswordAlertDialog();
		
		//************************** Version, LastError, State, ScanInfo **************************
		m_tasksToPerform = 4;
		getVersion();
		getLastError();
		getState();
		getScanInfo();
		
		//************************** Get Scan Info **************************
		m_scanInfoListener = new ScanInfoListener();
		m_scanInfoData.setOnItemSelectedListener(m_scanInfoListener);

		//************************** AuthType Spinner **************************
		m_authTypeAdapter = new ArrayAdapter<AuthType>(this, R.layout.my_simple_spinner_item_white);
		m_authTypeAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		AuthType[] authTypes = AuthType.values();
		List<AuthType> temp = Arrays.asList(authTypes);
		m_authTypeAdapter.addAll(temp);
		m_authTypeSpinner.setAdapter(m_authTypeAdapter);
		
		//************************** Buttons **************************
		m_configureButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				configure();
			}
		});
		
		m_connectButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				connect();
			}
		});
		
		//************************** receiver **************************
		//This receiver get notified when a new alljoyn device is found or lost,
		//add when the network state has changes (connected or disconnected, connecting, etc.)
		m_receiver = new BroadcastReceiver() {
			
			@Override
			public void onReceive(Context context, Intent intent) {

				if(Keys.Actions.ACTION_PASSWORD_IS_INCORRECT.equals(intent.getAction())){
					if(m_passwordAlertDialog != null && !m_passwordAlertDialog.isShowing())
						m_passwordAlertDialog.show();
				}
				else if(Keys.Actions.ACTION_ERROR.equals(intent.getAction())){
					String error = intent.getStringExtra(Keys.Extras.EXTRA_ERROR);
					m_application.showAlert(OnboardingActivity.this, error);
				}
				else if(Keys.Actions.ACTION_CONNECTED_TO_NETWORK.equals(intent.getAction())){

					String ssid = intent.getStringExtra(Keys.Extras.EXTRA_NETWORK_SSID);
					m_currentNetwork.setText(getString(R.string.current_network, ssid));
				}
			}
		};
		IntentFilter filter = new IntentFilter();
		filter.addAction(Keys.Actions.ACTION_PASSWORD_IS_INCORRECT);
		filter.addAction(Keys.Actions.ACTION_ERROR);
		filter.addAction(Keys.Actions.ACTION_CONNECTED_TO_NETWORK);
		registerReceiver(m_receiver, filter);
	}

	//====================================================================
	private void startOnboardingSession() {
		
		final AsyncTask<Void, Void, Void> task = new AsyncTask<Void, Void, Void>(){
			
			@Override
			protected void onPreExecute() {
				Log.d(TAG, "startSession: onPreExecute");
			}
			
			@Override
			protected Void doInBackground(Void... params) {
				m_application.startSession(m_device);
				return null;
			}

			@Override
			protected void onPostExecute(Void result) {
				Log.d(TAG, "startSession: onPostExecute");
			}
		};
		task.execute();	
	}
	//====================================================================
	
	// Gets the device onboarding version and put it on the screen	
	private void getVersion() {
		
		final AsyncTask<Void, Void, Short> task = new AsyncTask<Void, Void, Short>(){
			
			@Override
			protected void onPreExecute() {
				Log.d(TAG, "getOnboardingVersion: onPreExecute");
				showLoadingPopup("getting onboarding version");				
			}
			
			@Override
			protected Short doInBackground(Void... params){				
		    	return m_application.getOnboardingVersion();
			}

			@Override
			protected void onPostExecute(Short result){				
				short version = result.shortValue();
				m_onbaordingVersion.setText(String.valueOf(version));
				Log.d(TAG, "getOnboardingVersion: onPostExecute");
				m_tasksToPerform--;
				dismissLoadingPopup();
			}
		};
		task.execute();	
	}
	//====================================================================
	
	// Gets the device last error and put it on the screen	
	private void getLastError() {
		
		final AsyncTask<Void, Void, OBLastError> task = new AsyncTask<Void, Void, OBLastError>(){
			
			@Override
			protected void onPreExecute() {
				Log.d(TAG, "getLastError: onPreExecute");
				showLoadingPopup("getting last error version");				
			}
			
			@Override
			protected OBLastError doInBackground(Void... params){				
		    	return m_application.getLastError();
			}

			@Override
			protected void onPostExecute(OBLastError result){				
				
				m_application.makeToast("get last error done");
				String msg = result.getErrorMessage();
				m_lastErrorCodeValue.setText(result.getErrorCode()+"");
				m_lastErrorMsgValue.setText("".equals(msg) ? "No error msg" : msg);
				Log.d(TAG, "getLastError: onPostExecute");
				m_tasksToPerform--;
				dismissLoadingPopup();
			}
		};
		task.execute();	
	}
	//====================================================================
	
	// Gets the device state and put it on the screen
	private void getState() {
		
		final AsyncTask<Void, Void, Short> task = new AsyncTask<Void, Void, Short>(){
			
			@Override
			protected void onPreExecute() {
				Log.d(TAG, "getState: onPreExecute");
				showLoadingPopup("getting state");				
			}
			
			@Override
			protected Short doInBackground(Void... params){				
		    	return m_application.getState();
			}

			@Override
			protected void onPostExecute(Short result){
				
				m_application.makeToast("get state done");
				short version = result.shortValue();
				m_stateValue.setText(String.valueOf(version));
				Log.d(TAG, "getState: onPostExecute");
				m_tasksToPerform--;
				dismissLoadingPopup();
			}
		};
		task.execute();	
	}
	//====================================================================
	
	// Gets the device scan info and put it on the screen
	private void getScanInfo() {
		
		final AsyncTask<Void, Void, ScanInfo> task = new AsyncTask<Void, Void, ScanInfo>(){
			
			@Override
			protected void onPreExecute() {
				Log.d(TAG, "getScanInfo: onPreExecute");
				showLoadingPopup("getting scan info");				
			}
			
			@Override
			protected ScanInfo doInBackground(Void... params){				
		    	return m_application.getScanInfo();
			}

			@Override
			protected void onPostExecute(ScanInfo scan){
	
				m_application.makeToast("get scan info done");
				//Display the given scan result. if there are no scan result
				//we display "no results" to the user.
				if(scan != null && scan.getScanResults() != null){
					
					MyScanResult[] scanInfo = scan.getScanResults();
					m_scanInfoAdapter.clear();
					int age = scan.m_age;
					m_scanInfoAge.setText(age+"");
					
					for (MyScanResult scanResult : scanInfo) {
						if(!scanResult.m_ssid.startsWith("AJ_")){
							m_scanInfoAdapter.add(new MyScanResultWrapper(scanResult));
						}
					}
					if(m_scanInfoAdapter.getCount() == 0){
						MyScanResult sr = new MyScanResult();
						sr.m_authType = 0;
						sr.m_ssid = "No results";
						m_scanInfoAdapter.add(new MyScanResultWrapper(sr));
						m_scanInfoData.setEnabled(false);
					}
					m_scanInfoData.setAdapter(m_scanInfoAdapter);
				}
				else{
					//No scan info results
					MyScanResult sr = new MyScanResult();
					sr.m_authType = 0;
					sr.m_ssid = "No results";
					m_scanInfoAdapter.add(new MyScanResultWrapper(sr));
					m_scanInfoData.setAdapter(m_scanInfoAdapter);
					
					m_scanInfoAge.setText("No results");
				}
				Log.d(TAG, "getScanInfo: onPostExecute");
				m_tasksToPerform--;
				dismissLoadingPopup();
			}
		};
		task.execute();	
	}
	//====================================================================
	
	// Configure the device network.
	private void configure(){
		
		final AsyncTask<Void, Void, Void> task = new AsyncTask<Void, Void, Void>(){

			private String finalPassword;

			@Override
			protected void onPreExecute() {

				//Take the user parameters from the UI:
				m_networkName = m_networkNameEditText.getText().toString();
				m_networkPassword = m_networkPasswordEditText.getText().toString();
				AuthType selectedAuthType = (AuthType) m_authTypeSpinner.getSelectedItem();
				m_networkAuthType = selectedAuthType.getTypeId();
				
				//In case password is WEP and its format is HEX - leave it in HEX format.
				//otherwise convert it from ASCII to HEX
				finalPassword = m_networkPassword;
				if(AuthType.WEP.equals(selectedAuthType)){
					
					Pair<Boolean, Boolean> wepCheckResult = m_application.getIskWifiManager().checkWEPPassword(finalPassword);
					if (!wepCheckResult.first) {//Invalid WEP password
		                Log.i(TAG, "Auth type = WEP: password " + finalPassword + " invalid length or charecters");
		                
		            }
					else{
						Log.i(TAG, "configure wifi [WEP] using " + (!wepCheckResult.second ? "ASCII" : "HEX"));
						if (!wepCheckResult.second) {//ASCII. Convert it to HEX
							finalPassword = m_application.getIskWifiManager().toHexadecimalString(finalPassword);
						}
					}
				}
				else{//Other auth type than WEP -> convert password to HEX
					finalPassword = m_application.getIskWifiManager().toHexadecimalString(finalPassword);
				}
				
				m_connectButton.setEnabled(true);
				Log.d(TAG, "configure: onPreExecute");
				showLoadingPopup("configuring network");				
			}

			@Override
			protected Void doInBackground(Void... params){
				
				m_application.configureNetwork(m_networkName, finalPassword, m_networkAuthType);
				return null;
			}

			@Override
			protected void onPostExecute(Void result){
				m_application.makeToast("Configure network done");
				Log.d(TAG, "configure: onPostExecute");
				m_tasksToPerform--;
				dismissLoadingPopup();
			}
		};
		m_tasksToPerform = 1;
		task.execute();	
	}
	//====================================================================
	
	// Connect to the last configured network
	private void connect(){
		
		final AsyncTask<Void, Void, Void> task = new AsyncTask<Void, Void, Void>(){

			@Override
			protected void onPreExecute() {
				Log.d(TAG, "connect: onPreExecute");
				showLoadingPopup("connect network");				
			}

			@Override
			protected Void doInBackground(Void... params){				
				m_application.connectNetwork();
				return null;
			}

			@Override
			protected void onPostExecute(Void result){
			    Log.d(TAG, "connect: onPostExecute");
				m_application.makeToast("Connect network done");
				new Timer().schedule(new TimerTask() {
                                    @Override
                                    public void run() {
                                        m_application.getIskWifiManager().connectToAP(m_networkName, m_networkPassword, (short)m_networkAuthType);
                                        m_tasksToPerform--;
                                        dismissLoadingPopup();
                                    }
                                }, 3*1000);
			}
		};
		m_tasksToPerform = 1;
		task.execute();	
	}
	//====================================================================
	
	// Offboard the device network
	private void offboard(){
		
		final AsyncTask<Void, Void, Void> task = new AsyncTask<Void, Void, Void>(){

			@Override
			protected void onPreExecute() {

				Log.d(TAG, "offboard: onPreExecute");
				showLoadingPopup("offboarding");				
			}

			@Override
			protected Void doInBackground(Void... params){				
				m_application.offboard();
				return null;
			}

			@Override
			protected void onPostExecute(Void result){
				m_application.makeToast("Offboard done");
				Log.d(TAG, "offboard: onPostExecute");
				m_tasksToPerform--;
				dismissLoadingPopup();
			}
		};
		m_tasksToPerform = 1;
		task.execute();	
	}
	//====================================================================
	/* (non-Javadoc)
	 * @see android.app.Activity#onDestroy()
	 */
	@Override
	protected void onDestroy() {		
		super.onDestroy();
		m_application.endSession();
		if(m_receiver != null){
			try{
				unregisterReceiver(m_receiver);
			} catch (IllegalArgumentException e) {}
		}	
	}
	//====================================================================
	/* (non-Javadoc)
	 * @see android.app.Activity#onCreateOptionsMenu(android.view.Menu)
	 */
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.onboarding_menu, menu);
		return true;
	}
	//====================================================================
	/* (non-Javadoc)
	 * @see android.app.Activity#onOptionsItemSelected(android.view.MenuItem)
	 */
	@Override
	public boolean onOptionsItemSelected(MenuItem item) {

		switch (item.getItemId()) {
		case R.id.menu_onboarding_refresh:
			
			m_tasksToPerform = 4;
			m_scanInfoData.setOnItemSelectedListener(new ScanInfoListener());
			getVersion();
			getLastError();
			getState();
			getScanInfo();
			break;
		
		case R.id.menu_offboard:
			offboard();
			break;
		}
		return true;
	}
	//====================================================================
	/* (non-Javadoc)
	 * @see android.app.Activity#onConfigurationChanged(android.content.res.Configuration)
	 */
	@Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
	}
	//====================================================================

	//Let the user know the device not found and we cannot move to this screen
	//Extis this screen after the user pressed OK.
	private void closeScreen() {
		
		AlertDialog.Builder alert = new AlertDialog.Builder(this);
		alert.setTitle("Error");
		alert.setMessage("Device was not found");

		alert.setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int whichButton) {
				dialog.dismiss();
				finish();
			}
		});
		alert.show();
	}
	//====================================================================
	
	// Initialize the pop-up requesting the user to enter its password.
	private void initPasswordAlertDialog() {
		
		AlertDialog.Builder alert = new AlertDialog.Builder(OnboardingActivity.this);
		alert.setTitle("Your password is incorrect. Please enter the correct one");
		alert.setCancelable(false);

		final EditText input = new EditText(OnboardingActivity.this);
		input.setText("");
		alert.setView(input);

		alert.setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int whichButton) {

				String devicePassword = input.getText().toString();
				m_device.password = devicePassword.toCharArray();//Update the device password
				dialog.dismiss();
			}
		});

		alert.setNegativeButton(android.R.string.cancel, new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int whichButton) {
			}
		});
		m_passwordAlertDialog = alert.create();
	}
	//====================================================================

	
	// Display a progress dialog with the given msg.
	// If the dialog is already showing - it will update its message to the given msg.
	// The dialog will dismiss after 30 seconds if no response has returned. 
	private void showLoadingPopup(String msg)
	{
		if (m_loadingPopup !=null){
			if(!m_loadingPopup.isShowing()){
				m_loadingPopup = ProgressDialog.show(this, "", msg, true);
				Log.d(TAG, "showLoadingPopup with msg = "+msg);
			}
			else{
				m_loadingPopup.setMessage(msg);
				Log.d(TAG, "setMessage with msg = "+msg);
			}
		}
		m_timer = new Timer();
		m_timer.schedule(new TimerTask() {                
			public void run() {
				if (m_loadingPopup !=null && m_loadingPopup.isShowing()){
					Log.d(TAG, "showLoadingPopup dismissed the popup");
					m_loadingPopup.dismiss();
				};
			}                                

		},30*1000);
	}
	//====================================================================
	
	// Dismiss the progress dialog (only if it is showing).
	private void dismissLoadingPopup()
	{
		if(m_tasksToPerform == 0){
			if (m_loadingPopup != null){
				Log.d(TAG, "dismissLoadingPopup dismissed the popup");
				m_loadingPopup.dismiss();
				if(m_timer != null){
					m_timer.cancel();
				}
			}
		}
	}
	//====================================================================
	
	// A listener called when an item is selected from the spinner
	private class ScanInfoListener implements OnItemSelectedListener{

		@Override
		public void onItemSelected(AdapterView<?> adapterView, View view, int position, long l) {

			//Set the selected ssid to be the network name
			MyScanResult item = (MyScanResult)m_scanInfoData.getSelectedItem();
			m_networkNameEditText.setText(item.m_ssid);

			//Set the selected authType (given from the selected scan result)
			AuthType authType = AuthType.getAuthTypeById(item.m_authType);
			if(authType == null){
				authType = AuthType.ANY; 
			}
			
			// Search the authType in the list and make it the first selection.
			int authTypePosition = 0;
			AuthType[] values = AuthType.values();
			for (int i = 0; i < values.length; i++){
				if (values[i].equals(authType)){
					authTypePosition = i;
					break;
				}
			}				
			m_authTypeSpinner.setSelection(authTypePosition);
		}

		@Override
		public void onNothingSelected(AdapterView<?> adapterView) {
		}
	}
	//====================================================================
	//Exist only to override toString() 
	private class MyScanResultWrapper extends MyScanResult{
		
		public MyScanResultWrapper(MyScanResult scanResult) {
			super();
			this.m_ssid = scanResult.m_ssid;
			this.m_authType = scanResult.m_authType;
		}
		@Override
		public String toString() {			
			return this.m_ssid;
		}
	}
	//====================================================================
}
