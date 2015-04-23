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

import android.content.Context;
import android.database.DataSetObserver;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiManager;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ListAdapter;
import android.widget.TextView;

/**
 * This class is an adapter to display the list of
 * all the networks we scanned.
 */
public class ScanWIFIAdapter extends ArrayAdapter<ScanResult> implements ListAdapter{

	private static final String TAG = "ScanWIFIAdapter";
	static class ViewHolder {
		public TextView textWifiName;
		public TextView textWifiCapabiliteis;
		public TextView textWifiLevel;
	}
	
	private LayoutInflater m_layoutInflater;
	
	//====================================================================
	/**
	 * Creates the adapter given a context and a text view resource id.
	 * @param context
	 * @param textViewResourceId
	 */
	public ScanWIFIAdapter(Context context, int textViewResourceId) {
		super(context, textViewResourceId);
		m_layoutInflater = (LayoutInflater)context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);  
	}
	
	//====================================================================
	/* (non-Javadoc)
	 * @see android.widget.ArrayAdapter#getCount()
	 */
	@Override
	public int getCount() {
		int c = super.getCount();
		return c;
	}
	//====================================================================
	/* (non-Javadoc)
	 * @see android.widget.BaseAdapter#getItemViewType(int)
	 */
	@Override
	public int getItemViewType(int position) {
		return 1;
	}
	//====================================================================
	/* (non-Javadoc)
	 * @see android.widget.ArrayAdapter#getView(int, android.view.View, android.view.ViewGroup)
	 */
	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		
		   
		View rowView = convertView;
		if (rowView == null) {		    
			rowView = m_layoutInflater.inflate(R.layout.scan_wifi_property, null);
			ViewHolder viewHolder = new ViewHolder();
			viewHolder.textWifiName = (TextView) rowView.findViewById(R.id.wifi_name_row_textview);				
			viewHolder.textWifiCapabiliteis = (TextView) rowView.findViewById(R.id.wifi_capabilities);				
			viewHolder.textWifiLevel = (TextView) rowView.findViewById(R.id.wifi_level);		      		     
			rowView.setTag(viewHolder);
		}
		
		Log.i(TAG, "Get view [ "+position +" ] "+ " SSID "+getItem(position).SSID + " level " + getItem(position).level);
		ViewHolder holder = (ViewHolder) rowView.getTag();
		holder.textWifiName.setText(getItem(position).SSID);
		holder.textWifiCapabiliteis.setText(getItem(position).capabilities);

		holder.textWifiLevel.setText(Integer.toString(   WifiManager.calculateSignalLevel(getItem(position).level, 100)+1)+"%");		  
		return rowView;		  			
	}
	//====================================================================
	/* (non-Javadoc)
	 * @see android.widget.BaseAdapter#getViewTypeCount()
	 */
	@Override
	public int getViewTypeCount() {
		return 1;
	}
	//====================================================================
	/* (non-Javadoc)
	 * @see android.widget.BaseAdapter#registerDataSetObserver(android.database.DataSetObserver)
	 */
	@Override
	public void registerDataSetObserver(DataSetObserver arg0) {
		
	}
	//====================================================================
	/* (non-Javadoc)
	 * @see android.widget.BaseAdapter#unregisterDataSetObserver(android.database.DataSetObserver)
	 */
	@Override
	public void unregisterDataSetObserver(DataSetObserver arg0) {
		
	}
	//====================================================================
	/* (non-Javadoc)
	 * @see android.widget.BaseAdapter#areAllItemsEnabled()
	 */
	@Override
	public boolean areAllItemsEnabled() {
		return true;
	}
	//====================================================================
	/* (non-Javadoc)
	 * @see android.widget.BaseAdapter#isEnabled(int)
	 */
	@Override
	public boolean isEnabled(int arg0) {
		return true;
	}
	//====================================================================
}
