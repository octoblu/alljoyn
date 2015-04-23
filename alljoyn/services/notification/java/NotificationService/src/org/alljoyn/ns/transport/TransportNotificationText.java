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

package org.alljoyn.ns.transport;

import org.alljoyn.bus.annotation.Position;
import org.alljoyn.ns.NotificationText;

/* 
 * The utility class used to wrap NotificationText object into format of TransportNotificationText that 
 * is sent over AJ network
 */
public class TransportNotificationText {
	
	/**
	 * Text language
	 */
	@Position(0)
	public String language;
	
	/**
	 * Message text
	 */
	@Position(1)
	public String text;

	/**
	 * Constructor
	 */
	public TransportNotificationText() {}
	
	/**
	 * Constructor
	 * @param language
	 * @param text
	 */
	public TransportNotificationText(String language, String text) {
		this.language = language;
		this.text     = text;
	}

	/**
	 * Creates object of TransportNotificationText from NotificationText object
	 * @param notifText reference to NotificationText object
	 * @return TransportNotificationText
	 */
	public static TransportNotificationText buildInstance(NotificationText notifText) {
		TransportNotificationText trNotTxt = new TransportNotificationText(notifText.getLanguage(),
		     															   notifText.getText()
		);
		return trNotTxt;
	}//buildInstance

}
