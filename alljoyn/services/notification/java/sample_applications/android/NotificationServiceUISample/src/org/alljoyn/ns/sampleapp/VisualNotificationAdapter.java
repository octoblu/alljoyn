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

package org.alljoyn.ns.sampleapp;

import java.util.Arrays;
import java.util.Collection;
import java.util.List;

import org.alljoyn.ns.Notification;
import org.alljoyn.ns.NotificationText;
import org.alljoyn.ns.RichAudioUrl;

import android.content.Context;
import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.CheckBox;
import android.widget.TextView;

public class VisualNotificationAdapter extends ArrayAdapter<VisualNotification> {
	
	/**
	 * The item view cache class
	 */
	static class VisualNotificationView {
		TextView msgType;
		TextView itemText;
		TextView richAudioUrl;
		TextView richAudioObjPath;
		TextView richIconUrl;
		TextView richIconObjPath;
		CheckBox checkbox;
	}
	
	//====================================================//
	
	/**
	 * Id of the item resource
	 */
	private int notifViewResourceId;

	/**
	 * Inflater that is used to create the item object
	 */
	private LayoutInflater layoutInflater=null;
	
	/**
	 * The notifications to be rendered
	 */
	private List<VisualNotification> notificationsList;
	
	/**
	 * The context of the activity that manages the {@link VisualNotificationAdapter}
	 */
	private Context activityContext;
	
	/**
	 * Constructor 
	 * @param context The application context
	 * @param notifViewResourceId The item resource id
	 * @param notificationsList The list of notifications to present
	 */
	public VisualNotificationAdapter(Context context, int notifViewResourceId, List<VisualNotification> notificationsList) {
		super(context, notifViewResourceId, notificationsList);
		
		this.notifViewResourceId	= notifViewResourceId;
		this.notificationsList      = notificationsList;
		this.activityContext        = context;
		
		layoutInflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
	}

	/**
	 * @see android.widget.ArrayAdapter#getCount()
	 */
	@Override
	public int getCount() {
		return notificationsList == null ? 0 : notificationsList.size();
	}//getCount


	/**
	 * @see android.widget.ArrayAdapter#clear()
	 */
	@Override
	public void clear() {
		super.clear();
		
		if ( notificationsList != null ) {
			notificationsList.clear();
		}
	}//clear
	
	/**
	 * @see android.widget.ArrayAdapter#getItem(int)
	 */
	@Override
	public VisualNotification getItem(int position) {
        if ( position >= getCount() ) {
             return null;
        }
        return  notificationsList.get(position);
    }//getItem

	/**
	 * @see android.widget.ArrayAdapter#add(java.lang.Object)
	 */
	@Override
	public void add(VisualNotification object) {
		if ( notificationsList != null ) {
			notificationsList.add(object);
		}
	}//add

	/**
	 * @see android.widget.ArrayAdapter#addAll(java.util.Collection)
	 */
	@Override
	public void addAll(Collection<? extends VisualNotification> collection) {
		if ( notificationsList != null ) {
			notificationsList.addAll(collection);
		}
	}//addAll

	/**
	 * @see android.widget.ArrayAdapter#addAll(java.lang.Object[])
	 */
	@Override
	public void addAll(VisualNotification... items) {
		if ( notificationsList != null ) {
			notificationsList.addAll(Arrays.asList(items));
		}
	}//addAll

	/**
	 * @see android.widget.ArrayAdapter#getView(int, android.view.View, android.view.ViewGroup)
	 */
	@Override
	public View getView(final int position, View convertView, ViewGroup parent) {

		View row = convertView;
		VisualNotificationView visualNotificationView = null;
		
		if ( row == null ) {
			
			row  = layoutInflater.inflate(notifViewResourceId, parent, false);
			
			//Bind the new row UI components to its view - visualNotifView
			visualNotificationView          = new VisualNotificationView();
			visualNotificationView.msgType  = (TextView) row.findViewById(R.id.tv_nitem_msgtype);
			visualNotificationView.itemText = (TextView) row.findViewById(R.id.tv_nitem_text);
			
			visualNotificationView.richAudioUrl     = (TextView) row.findViewById(R.id.tv_nitem_rich_audio);
			visualNotificationView.richAudioObjPath = (TextView) row.findViewById(R.id.tv_nitem_rich_audio_ObjPath);
			visualNotificationView.richIconUrl      = (TextView) row.findViewById(R.id.tv_nitem_rich_icon);
			visualNotificationView.richIconObjPath  = (TextView) row.findViewById(R.id.tv_nitem_rich_icon_ObjPath);
			
			visualNotificationView.checkbox         = (CheckBox) row.findViewById(R.id.chb_selector);
			
			//Associate the visualNotifView with the created row
			row.setTag(visualNotificationView);
		}
		else {
			//For the already existent row, retrieve the object to update the UI 
			visualNotificationView = (VisualNotificationView)row.getTag();
		}
		
		//Set the default background color
		row.setBackgroundColor(Color.WHITE);
		
		//Set the default checkbox state
		visualNotificationView.checkbox.setEnabled(true);
		
		//Bind the notification data to the UI
		bindData(visualNotificationView, notificationsList.get(position));
		
		final VisualNotification visualNotification = notificationsList.get(position);
		
		if ( !visualNotification.isDismissed() ) {
			
			visualNotificationView.checkbox.setChecked(visualNotification.isChecked());
			visualNotificationView.checkbox.setOnClickListener( new OnClickListener() {
				@Override
				public void onClick(View v) {
					CheckBox chb = (CheckBox) v;
					//Update the VisualNotification checked state
					visualNotification.setChecked(chb.isChecked());
					updateReceiverControlButtonsState();
				}//onClick
					
			}); //checkbox OnClickListener
		
		}//if :: !isDismissed
		else {
			// VisualNotification has been dismissed
			if ( visualNotification.isChecked() ) {
				visualNotification.setChecked(false);
			}
			visualNotificationView.checkbox.setChecked(false);
			visualNotificationView.checkbox.setEnabled(false);
			row.setBackgroundColor(Color.rgb(255, 248, 177));
		}
		
		//Decide what is the UI state of the Dismiss|Action buttons
		updateReceiverControlButtonsState();

		return row;
	}//getView
	
	/**
	 * Update the state of the Dismiss|Action buttons in depend on the {@link VisualNotification#getCheckedCounter()}
	 */
	private void updateReceiverControlButtonsState() {
		
		int checkedCounter = VisualNotification.getCheckedCounter();
		if ( checkedCounter == 0 ) {        // Call to disable the Dismiss|Action button
			((NotificationServiceControlsActivity) activityContext).enableReceiverControlButtons(false);
		}
		else if ( checkedCounter == 1 ) {   // Call to enable the Dismiss|Action button
			((NotificationServiceControlsActivity) activityContext).enableReceiverControlButtons(true);
		}
	}//updateDeleteButtonsState

	/**
	 * Applies the {@link VisualNotification} data on the {@link VisualNotificationView}
	 * @param view
	 * @param data
	 */
	private void bindData(VisualNotificationView view, VisualNotification data) {
		 
		Notification notif = data.getNotification();
		
		//Set text
		view.itemText.setText(findTextToPresent(data));
		
		//Set message type
		view.msgType.setText(notif.getMessageType().toString());
		
		//Set rich icon url
		String richIconUrl = notif.getRichIconUrl();
		if ( richIconUrl != null && richIconUrl.length() > 0 ) {
			view.richIconUrl.setText(richIconUrl);
			view.richIconUrl.setVisibility(View.VISIBLE);
		}else{
			view.richIconUrl.setVisibility(View.GONE);
		}
			
		//Set rich icon object path
		String richIconObjPath = notif.getRichIconObjPath();
		if ( richIconObjPath != null && richIconObjPath.length() > 0 ) {
			view.richIconObjPath.setText(richIconObjPath);
			view.richIconObjPath.setVisibility(View.VISIBLE);
		}else{
			view.richIconObjPath.setVisibility(View.GONE);
		}
		
		//Set rich audio url
		List<RichAudioUrl> richAudioUrlList = notif.getRichAudioUrl();
		if ( richAudioUrlList != null && richAudioUrlList.size() > 0 ) {
			String richAudioUrl = richAudioUrlList.get(0).getUrl();
			view.richAudioUrl.setText(richAudioUrl);
			view.richAudioUrl.setVisibility(View.VISIBLE);
		}else{
			view.richAudioUrl.setVisibility(View.GONE);
		}
		
		//Set rich audio object path
		String richAudioObjPath = notif.getRichAudioObjPath();
		if ( richAudioObjPath != null && richAudioObjPath.length() > 0 ) {
			 view.richAudioObjPath.setText(richAudioObjPath);
			 view.richAudioObjPath.setVisibility(View.VISIBLE);
		}else{
			view.richAudioObjPath.setVisibility(View.GONE);
		}
	  		
	}//bindData
	
	/**
	 * Find the text to be rendered according to the user preferred language, or the default language 
	 * @param data The object to reach {@link NotificationText}
	 * @return The text to be rendered
	 */
	private String findTextToPresent(VisualNotification data) {
		
		String usrPrefLangStr   = data.getUserPreferredLang();
		String msgToShow        = data.getUserPreferredText(); 

		// Check whether the message to show has been previously found for the given VisualNotification
		if ( msgToShow != null && msgToShow.length() > 0 ) {
			return msgToShow;
		}
		
        //Stores default message in English
        String recvDefltMsg     = ""; 
    
        // Search notification in the preferred language, if not found - use English
        for (NotificationText ntObj : data.getNotification().getText() ) {
        	
            if ( ntObj.getLanguage().startsWith(usrPrefLangStr) ) { 
                msgToShow = ntObj.getText();
                break;
            }   
            else if ( ntObj.getLanguage().startsWith(LangEnum.English.INT_NAME) ) { // The default English language must be sent  
                recvDefltMsg = ntObj.getText();
            }
            
        }//for :: languages

        // The Notification in the desired language was not found
        if ( msgToShow == null || msgToShow.length() == 0 ) { 
            msgToShow = recvDefltMsg;
        }   
    	
        data.setUserPreferredText(msgToShow);
        
		return msgToShow;
	}//findTextToPresent
	
}
