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

package org.alljoyn.ns;


/**
 * NotificationText. Creates object of the NotificationText class
 */
public class NotificationText {

	/**
	 * Reference to language string <br>
	 * The language must comply with the IETF standard 
	 */
	private String language;
	
	/**
	 * Notification text
	 */
	private String text;
	
	/**
	 * Constructor
	 */
	public NotificationText() {}
	
	/**
	 * Constructor
	 * @param language The language must comply with the IETF standard 
	 * @param text the notification text
	 * @throws NotificationServiceException Thrown if language is null text is null or empty
	 */
	public NotificationText(String language, String text) throws NotificationServiceException {
		setLanguage(language);
		setText(text);
	}

	/**
	 * @return String Returns notification language. The language must comply with the IETF standard 
	 */
	public String getLanguage() {
		return language;
	}
	
	/**
	 * Set the language
	 * @param language The language must comply with the IETF standard 
	 * @throws NotificationServiceException Thrown if language is null
	 */
	public void setLanguage(String language) throws NotificationServiceException {
		if ( language == null || language.length() == 0 ) {
			throw new NotificationServiceException("Language is undefined");
		}
		this.language = language;
	}//setLanguage
	
	/**
	 * @return the notification text
	 */
	public String getText(){
		return text;
	}
	
	/**
	 * Set the notification text
	 * @param text the notification text
	 * @throws NotificationServiceException Thrown if text is null or is empty
	 */
	public void setText(String text) throws NotificationServiceException {
		if ( text == null || text.length() == 0 ) {
			throw new NotificationServiceException("Text is undefined");
		}
		this.text = text;
	}//setText

	
	/**
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return "NotificationText [language='" + language + "', text='" + text + "']";
	}
	
}//NotificationText
