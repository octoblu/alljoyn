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

import java.util.List;
import java.util.UUID;

import org.alljoyn.ns.Notification;

import android.os.AsyncTask;
import android.util.Log;

/**
 * Handle the received Dismiss signal asynchronously. <br>
 * If the {@link VisualNotification} hasn't dismissed previously and its notificationId and appId equals to the given
 * notificationId and appId, then mark the {@link VisualNotification} as dismissed and call {@link VisualNotificationAdapter#notifyDataSetChanged()}
 */
public class DismissSignalHandler extends AsyncTask<Void, Void, VisualNotification> {
	private static final String TAG = "ioe" + DismissSignalHandler.class.getSimpleName();
		
	/**
	 * Notification id
	 */
	private final int notifId;
	
	/**
	 * Application id
	 */
	private final UUID appId;
	
	/**
	 * List of {@link VisualNotification} objects to update
	 */
	private final List<VisualNotification> notificationList;
	
	/**
	 * Update view when the {@link VisualNotification} state changed 
	 */
	private final VisualNotificationAdapter notificationAdapter;
	
	/**
	 * Constructor
	 * @param notifId
	 * @param appId
	 * @param notificationList
	 * @param notificationAdapter
	 */
	public DismissSignalHandler(int notifId, UUID appId, List<VisualNotification> notificationList, VisualNotificationAdapter notificationAdapter) {
		super();
		this.notifId 			 = notifId;
		this.appId 		         = appId;
		this.notificationList    = notificationList;
		this.notificationAdapter = notificationAdapter;
	}
	
	/**
	 * @see android.os.AsyncTask#doInBackground(java.lang.Object[])
	 */
	@Override
	protected VisualNotification doInBackground(Void... args) {

		VisualNotification toDismiss = null;
		
		for ( VisualNotification vn : notificationList ) {
			
			if ( vn.isDismissed() ) {
				continue;
			}
			
			Notification notif = vn.getNotification();
			if ( notif.getAppId().equals(appId)  && notif.getMessageId() == notifId ) {
				toDismiss = vn;
				break;
			}
		}
		
		return toDismiss;
	}//doInBackground
	
	/**
	 * @see android.os.AsyncTask#onPostExecute(java.lang.Object)
	 */
	@Override
	protected void onPostExecute(VisualNotification toDismiss) {
		
		if ( toDismiss != null && !toDismiss.isDismissed() ) {
			Log.d(TAG, "DismissHandler has found the notification to be marked as Dismissed, appId: '" + appId + "', notifId: '" + notifId + "'");
			toDismiss.setDismissed(true);
			notificationAdapter.notifyDataSetChanged();
		}
		else {
			Log.d(TAG, "DismissHandler HAS NOT found the notification to be marked as Dismissed, appId: '" + appId + "', notifId: '" + notifId + "'");
		}
	}//onPostExecute
	
}
