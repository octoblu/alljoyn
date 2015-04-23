package org.alljoyn.ioe.controlpaneladapter;
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

import org.alljoyn.ioe.controlpanelservice.ControlPanelException;
import org.alljoyn.ioe.controlpanelservice.ui.ActionWidget;
import org.alljoyn.ioe.controlpanelservice.ui.AlertDialogWidget.DialogButton;

import android.os.AsyncTask;
import android.util.Log;

/**
 * A Utility class that has 2 functions:
 * 1. Executing an action on the remote device, in a background process, to free the UI thread.
 * 2. Notifying the caller about the result. 
 */
abstract class ExecuteActionAsyncTask extends AsyncTask<Object, Void, ControlPanelException> {

	private final static String TAG = "cpapp" + ExecuteActionAsyncTask.class.getSimpleName();

	@Override
	/**
	 * This background processor assumes params[0] to be a ActionWidget or DialogButton.
	 */
	protected ControlPanelException doInBackground(Object... params) {
		try {
			if (params[0] instanceof ActionWidget) {
				ActionWidget actionWidget 	= (ActionWidget) params[0];
				Log.d(TAG, String.format("Executing action '%s'", actionWidget.getLabel()));
				actionWidget.exec();
				Log.d(TAG, "Action executed");
			} else if (params[0] instanceof DialogButton) {
				DialogButton dialogButton 	= (DialogButton) params[0];
				Log.d(TAG, String.format("Executing action '%s'", dialogButton.getLabel()));
				dialogButton.exec();
				Log.d(TAG, "Action executed");
			}				
		} catch (ControlPanelException e) {
			Log.e(TAG, "Failed executing the action, error in calling remote object: '" + e.getMessage() + "'");
			return e;
		}
		return null; // No exception...ok!
	}

	
	@Override
	/**
	 * Exception means failure. Otherwise - success.  
	 */
    protected void onPostExecute(ControlPanelException e) {
		if (e != null) {
			onFailure(e);
		} else {
			onSuccess();
		}
	}

	/**
	 * Notify the caller about an exception.  
	 */
    abstract protected void onFailure(ControlPanelException e);
	/**
	 * Notify the caller about success.  
	 */
    abstract protected void onSuccess();
	
}
