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

import org.alljoyn.ns.Notification;
import org.alljoyn.ns.NotificationText;

/**
 * Handled the received {@link Notification} object
 */
public class VisualNotification {
	
	/**
	 * The received {@link Notification} 
	 */
	private Notification notification;
	
	/**
	 * Marker whether the {@link Notification} has been selected
	 */
	private boolean isChecked;
	
	/**
	 * The counter is incremented when a {@link VisualNotification} object gets isChecked state
	 * as TRUE and it's decremented when it's FALSE
	 */
	private static int checkedCounter;
	
	/**
	 * The language the user preferred to view the {@link NotificationText}
	 */
	private String userPreferredLang;
	
	/**
	 * The text to be presented to the user in the userPreferredLang
	 */
	private String userPreferredText;
	
	/**
	 * Gets TRUE when the {@link VisualNotification} is dismissed
	 */
	private volatile boolean isDismissed;
	
	/**
	 * Constructor
	 * @param notification The received notification
	 */
	public VisualNotification(Notification notification, String userPreferredLang) {
		this.notification      = notification;
		this.userPreferredLang = userPreferredLang;
	}

	/**
	 * @return {@link Notification}
	 */
	public Notification getNotification() {
		return notification;
	}//getNotification

	/**
	 * @return Marker whether the {@link Notification} has been checked
	 */
	public boolean isChecked() {
		return isChecked;
	}

	/**
	 * @param isChecked Sets the {@link VisualNotification} checked state <br>
	 * The state of the isChecked influences the static checkedCounter variable. <br>
	 * If isChecked is TRUE the checkedCounter is incremented by one otherwise it's decremented
	 */
	public void setChecked(boolean isChecked) {
		this.isChecked = isChecked;
		
		if ( isChecked ) {
			++checkedCounter;
		}
		else if ( checkedCounter > 0 ){
			--checkedCounter;
		}
	}//setChecked

	/**
	 * @return The current checkedCounter value
	 */
	public static int getCheckedCounter() {
		return checkedCounter;
	}//getCheckedCounter

	/**
	 * @return The language string, the user preferred to see the {@link NotificationText}
	 */
	public String getUserPreferredLang() {
		return userPreferredLang;
	}

	/**
	 * @return The text to be presented to the user, when this {@link VisualNotification} is rendered
	 */
	public String getUserPreferredText() {
		return userPreferredText;
	}

	/**
	 * @param userPreferredText The text to be presented to the user, when this {@link VisualNotification} is rendered
	 */
	public void setUserPreferredText(String userPreferredText) {
		this.userPreferredText = userPreferredText;
	}
	
	/**
	 * @return Whether the Notification is dismissed
	 */
	public boolean isDismissed() {
		return isDismissed;
	}

	/**
	 * @param isDismissed Whether the Notification is dismissed
	 */
	public void setDismissed(boolean isDismissed) {
		this.isDismissed = isDismissed;
	}
	
		
}
