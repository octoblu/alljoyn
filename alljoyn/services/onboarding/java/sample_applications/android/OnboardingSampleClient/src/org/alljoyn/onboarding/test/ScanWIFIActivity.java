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

import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import android.app.AlertDialog;
import android.app.ListActivity;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.net.NetworkInfo;
import android.net.NetworkInfo.State;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.text.InputType;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.EditText;
import android.widget.ListView;


/**
 * The ScanWIFIActivity enables the user to scan the networks which the device can connect to
 * and choose one to connect to.
 */
public class ScanWIFIActivity extends ListActivity {

	protected static final String TAG = OnboardingApplication.TAG;
	// ListAdapter
	private ScanWIFIAdapter m_adapter;
	// Lets the user kick off a scan
	private Button m_scanWIFIButton;
	// List of devices
	private ListView m_list;
	// Wi-Fi manager
	private IskWifiManager m_WifiManager;
	// UI feedback for long actions like scanning
	private ProgressDialog m_progressDialog;
	private ProgressDialog m_loadingPopup;
	// Dialog dismiss timer
	private Timer m_timer;
	private BroadcastReceiver m_receiver; 
	
	//=================================================================================
    /* (non-Javadoc)
     * @see android.app.Activity#onCreate(android.os.Bundle)
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        
    	super.onCreate(savedInstanceState);
    	
        setContentView(R.layout.scan_wifi_layout);
        m_WifiManager = ((OnboardingApplication)getApplication()).getIskWifiManager();
        m_list = (ListView) findViewById(android.R.id.list);
        m_adapter = new ScanWIFIAdapter(ScanWIFIActivity.this, R.id.wifi_name_row_textview);
		m_list.setAdapter(m_adapter);
		m_loadingPopup = new ProgressDialog(this);
    }
    
    //=================================================================================
	/* (non-Javadoc)
	 * @see android.app.Activity#onStart()
	 */
	@Override
	protected void onStart(){		
		super.onStart();
		m_scanWIFIButton = (Button)findViewById(R.id.scan_wifi);		
		m_scanWIFIButton.setOnClickListener(new OnClickListener() {			
			@Override
			public void onClick(View view) {
				
				m_progressDialog = ProgressDialog.show(ScanWIFIActivity.this, "", getString(R.string.wifi_activity_scanning));
				
				m_WifiManager.scanForWifi(getApplicationContext(), new WifiManagerListener() {						
					public void OnScanResultComplete(final List<ScanResult> results) {
						runOnUiThread(new Runnable() {
							public void run() {
								// reset the list to show up to date scan result
								m_adapter.clear();
								m_adapter.addAll(results);
								m_list.setAdapter(m_adapter);
								m_adapter.notifyDataSetChanged();	   	
							}
						});
						// set a timer for the progress dialog
						Timer timer = new Timer();
						timer.schedule(new TimerTask() {			            	
							public void run() {
								if (m_progressDialog!=null && m_progressDialog.isShowing())
								{								
									m_progressDialog.dismiss();
								};
							}			            				            				              
						},2000);
					}
					// filter out AP that don't start with "AJ_"
				}, "AJ_"); 
			}					
		});
	}
	
    //=================================================================================
    /* (non-Javadoc)
     * @see android.app.ListActivity#onListItemClick(android.widget.ListView, android.view.View, int, long)
     */
    @Override
    protected void onListItemClick(ListView l, View v, int position, long id) {
    	
		final ScanResult scanItem = m_adapter.getItem(position);
		if (scanItem != null){
			
			if(!isSsidEquals(scanItem.SSID, m_WifiManager.getCurrentNetworkSSID())){

				// show a dialog for receiving password
				AlertDialog.Builder alert = new AlertDialog.Builder(this);
				alert.setTitle(R.string.enter_wifi_password_title); 
				alert.setMessage(R.string.enter_wifi_password_message); 
				
				View view = getLayoutInflater().inflate(R.layout.password_type_popup, null);
				final EditText input = (EditText)view.findViewById(R.id.passwordEditText);
				final CheckBox showPassword = (CheckBox)view.findViewById(R.id.showPasswordCheckBox);
				alert.setView(view);
				
				showPassword.setOnCheckedChangeListener(new OnCheckedChangeListener() {
					@Override
					public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
						
						if(isChecked)
							input.setInputType(InputType.TYPE_CLASS_TEXT);
						else
							input.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_PASSWORD);
					}
				});
				
				alert.setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
					
					public void onClick(DialogInterface dialog, int whichButton) {
						
						showLoadingPopup(getString(R.string.connecting_to, scanItem.SSID));						
						String pass = input.getText().toString();
						
						//connect to the selected scan item. 
						boolean succeeded = m_WifiManager.connectToAP(scanItem.SSID, pass, scanItem.capabilities);
						if(succeeded){
							goToDeviceListActivity(scanItem.SSID);
						}
						else{
							//show an error dialog
							dismissLoadingPopup();
							AlertDialog.Builder alert = new AlertDialog.Builder(ScanWIFIActivity.this);
							alert.setTitle(R.string.failed_to_connect_to_wifi_title); 
							alert.setMessage(getString(R.string.failed_to_connect_to_wifi_message,scanItem.SSID));
							alert.setNeutralButton(android.R.string.ok, new DialogInterface.OnClickListener() {
								public void onClick(DialogInterface dialog, int whichButton) {
								}
							});
							alert.show();
						}
					}
				});
				alert.setNegativeButton(android.R.string.cancel, new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int whichButton) {
					}
				});
				alert.show();
			}
			else{
				//already connected. no need to enter password.
				goToDeviceListActivity(scanItem.SSID);
			}
		}
    }
    //=================================================================================
    
    // Move to the next screen.
    private void goToDeviceListActivity(final String requestedSSID){
    	
    	m_receiver = new BroadcastReceiver() {

			@Override
			public void onReceive(Context context, Intent intent) {

				if(WifiManager.NETWORK_STATE_CHANGED_ACTION.equals(intent.getAction())){

					NetworkInfo networkInfo = intent.getParcelableExtra(WifiManager.EXTRA_NETWORK_INFO);

					if(networkInfo.getState().equals(State.CONNECTED)){
						WifiInfo wifiInfo = intent.getParcelableExtra(WifiManager.EXTRA_WIFI_INFO);
						String currentSSID = wifiInfo.getSSID();
						Log.d(TAG, "check if "+requestedSSID+" equals "+currentSSID);
						if(isSsidEquals(requestedSSID, currentSSID)){
							unregisterReceiver(this);
							Intent next = new Intent(ScanWIFIActivity.this, DeviceListActivity.class);
							dismissLoadingPopup();
							startActivity(next);
						}
					}
				}
			}
    	};
		IntentFilter filter = new IntentFilter();
		filter.addAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);
		registerReceiver(m_receiver, filter);
    	
    }
    //=================================================================================
    // Android SSID may include '"', which would makes equals() fail.
    private boolean isSsidEquals(String ssid1, String ssid2)
	{
		if (ssid1 == null || ssid1.length() == 0 || ssid2 == null || ssid2.length() == 0){
			return false;
		}
		ssid1 = ssid1.replace("\"", "");
		ssid2 = ssid2.replace("\"", "");
		return ssid1.equals(ssid2);
	}
    //=================================================================================
    /* (non-Javadoc)
     * @see android.app.Activity#onConfigurationChanged(android.content.res.Configuration)
     */
    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
    }
    //=================================================================================
    
    // Display a progress dialog with the given msg.
	// If the dialog is already showing - it will update its message to the given msg.
	// The dialog will dismiss after 5 seconds if no response has returned. 
    private void showLoadingPopup(String msg)
	{
		if (m_loadingPopup !=null){
			if(!m_loadingPopup.isShowing()){
				m_loadingPopup = ProgressDialog.show(this, "", msg, true);
			}
			else{
				m_loadingPopup.setMessage(msg);
			}
		}
		m_timer = new Timer();
		m_timer.schedule(new TimerTask() {                
			public void run() {
				if (m_loadingPopup !=null && m_loadingPopup.isShowing()){
					m_loadingPopup.dismiss();
				};
			}                                

		},5*1000);
	}
	//=================================================================================
	
    // Dismiss the progress dialog (only if it is showing).
    private void dismissLoadingPopup()
	{
		if (m_loadingPopup != null){
			m_loadingPopup.dismiss();
			if(m_timer != null){
				m_timer.cancel();
			}
		}
	}
	//=================================================================================
    @Override
    protected void onDestroy() {
    	
    	super.onDestroy();
    	if(m_receiver != null){
    		try{
    			unregisterReceiver(m_receiver);
    		} catch (Exception e) {
			}
    	}
    	m_WifiManager.unregisterWifiManager();
    }
	//=================================================================================
}

