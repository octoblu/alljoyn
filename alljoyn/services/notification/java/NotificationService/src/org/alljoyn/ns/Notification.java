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

import java.util.List;
import java.util.Map;
import java.util.UUID;

import org.alljoyn.ns.transport.consumer.NotificationFeedback;

/**
 * The Notification that is sent over the network
 */
public class Notification {
	          
	/**
	 * version
	 */
	private int version;		
	
	/**
	 * Notification message Id
	 */
	private int messageId;				
	
	/**
	 * Notification message type
	 */
	private NotificationMessageType messageType;	
	
	/**
	 * App name
	 */
	private String appName;		
	
	/**
	 * App Id
	 */
	private UUID appId;	
	
	/**
	 * Device name
	 */
	private String deviceName;		
	
	/**
	 * Device Id
	 */
	private String deviceId;	
			
	/**
	 * Sender Id - name of the bus that the {@link Notification} was received from 
	 */
	private String sender;
	
	/**
	 * The sender Id of the Bus that originally sent the {@link Notification} message.
	 * If the {@link Notification} was received from a Super Agent, the sender field of the object is of the Super Agent, 
	 * and this field is the value of the {@link Notification} producer that has originally sent the message.    
	 */
	private String origSender;
	
	/**
	 * Map of customized array variable
	 */
	private Map<String, String> customAttributes;		
	
	/**
	 * list of NotificationText objects
	 */
	private List<NotificationText> text;			
	
	/**
	 * Rich notification icon URL
	 */
	private String richIconUrl;
	
	/**
	 * Rich notification audio url
	 */
	private List<RichAudioUrl> richAudioUrl;
	
	/**
	 * Rich notification icon object path
	 */
	private String richIconObjPath;
	
	/**
	 * Rich notification audio object path
	 */
	private String richAudioObjPath;
	
	/**
	 * Control Panel Service object path
	 */
	private String responseObjectPath;
	
	/**
	 * Constructor
	 * @param messageType The type of the sent or received Notification
	 * @param text The list of text messages to be sent or received
	 */
	public Notification(NotificationMessageType messageType, List<NotificationText> text) throws NotificationServiceException {
		setMessageType(messageType);
		setText(text);
	}//Notification

	/**
	 * @return The protocol version
	 */
	public int getVersion() {
		return version;
	}

	/**
	 * @return Notification message type
	 */
	public NotificationMessageType getMessageType() {
		return messageType;
	}

	/**
	 * @return Returns the application name
	 */
	public String getAppName() {
		return appName;
	}

	/**
	 * @return The application identifier
	 */
	public UUID getAppId() {
		return appId;
	}
	
	/**
	 * The device name that has sent the {@link Notification}
	 */
	public String getDeviceName() {
		return deviceName;
	}

	/**
	 * The device id that has sent the {@link Notification}
	 */
	public String getDeviceId() {
		return deviceId;
	}

	/**
	 * Sender Id - name of the bus that the {@link Notification} was received from
	 */
	public String getSenderBusName() {
		return sender;
	}
	
	/**
	 * The sender Id of the Bus that originally sent the {@link Notification} message.
	 * If the {@link Notification} was received from a Super Agent, the sender field of the object is of the Super Agent, 
	 * and this field is the value of the {@link Notification} producer that has originally sent the message.
	 * @return Bus name of the original {@link Notification} sender
	 */
	public String getOriginalSenderBusName() {
		return origSender;
	}
	
	/**
	 * Map of customized array variable
	 */
	public Map<String, String> getCustomAttributes() {
		return customAttributes;
	}

	/**
	 * List of {@link NotificationText} objects
	 */
	public List<NotificationText> getText() {
		return text;
	}

	/**
	 * {@link Notification} message id
	 */
	public int getMessageId() {
		return messageId;
	}

	/**
	 * @see org.alljoyn.ns.Notification#getRichIconUrl()
	 */
	public String getRichIconUrl() {
		return richIconUrl;
	}
	
	/**
	 * Rich notification icon URL
	 */
	public List<RichAudioUrl> getRichAudioUrl() {
		return richAudioUrl;
	}	
	
	/**
	 * Rich notification icon object path
	 */
	public String getRichIconObjPath() {
		return richIconObjPath;
	}
	
	/**
	 * Rich notification audio object path
	 */
	public String getRichAudioObjPath() {
		return richAudioObjPath;
	}
	
	/**
	 * The object identifier to be used to invoke methods of the sender.
	 * Usually used for Notification with action.
	 */
	public String getResponseObjectPath() {
		return responseObjectPath;
	}
	
	/**
	 * Set notification message type
	 * @param messageType
	 * @throws NotificationServiceException Is thrown if the message type is NULL
	 */
	public void setMessageType(NotificationMessageType messageType) throws NotificationServiceException {
		if ( messageType == null ) {
			throw new NotificationServiceException("MessageType must be set");
		}
		this.messageType = messageType;
	}//setMessageType
	
	/**
	 * Set customAttributes map
	 * @param customAttributes
	 */
	public void setCustomAttributes(Map<String, String> customAttributes) {
		this.customAttributes = customAttributes;
	}
	
	/**
	 * Set list of NotificationText
	 * @param text Is thrown if the text is NULL or is empty
	 */
	public void setText(List<NotificationText> text) throws NotificationServiceException {
		if ( text == null || text.size() == 0 ) {
			throw new NotificationServiceException("The text argument must be set and may not be an empty list");
		}
		this.text = text;
	}//setText

	/**
	 * Set the icon url for displaying rich content
	 * @param richIconUrl 
	 */
	public void setRichIconUrl(String richIconUrl) {
		this.richIconUrl = richIconUrl;
	}
	
	/**
	 * Set audio url to send rich content
	 * @param richAudioUrl - A list of url per language
	 */
	public void setRichAudioUrl(List<RichAudioUrl> richAudioUrl) {
		this.richAudioUrl = richAudioUrl;
	}
	
	/**
	 * Set the icon Object Path for displaying rich content
	 * @param richIconObjPath 
	 */
	public void setRichIconObjPath(String richIconObjPath) {
		this.richIconObjPath = richIconObjPath;
	}
	
	/**
	 * Set audio Object Path to send rich content
	 * @param richAudioObjPath - An object path for the rich audio object
	 */
	public void setRichAudioObjPath(String richAudioObjPath) {
		this.richAudioObjPath = richAudioObjPath;
	}

	/**
	 * Set Control Panel Service Object Path
	 * @param responseObjectPath
	 */
	public void setResponseObjectPath(String responseObjectPath) {
		this.responseObjectPath = responseObjectPath;
	}

	
	//=======================================//
	
	/**
	 * When the notification message is dismissed, it is first of all deleted by the Notification Producer
	 * and then a dismiss signal is sent to update other Notification Consumers that the {@link Notification} 
	 * has been dismissed   
	 */
	public void dismiss() {
		try {
			new NotificationFeedback(this).dismiss();
		} catch (NotificationServiceException nse) {
			System.out.println("Failed to call the dismiss method, Error: '" + nse.getMessage() + "'");
		}
	}//dismiss

	/**
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		
		StringBuilder sb = new StringBuilder("Notification: ");
		sb.append("Id: '").append(messageId).append("'")
		.append(", MsgType: '").append(messageType).append("'")
		.append(", DeviceId: '").append(deviceId).append("'")
		.append(", DeviceName: '").append(deviceName).append("'")
		.append(", Version: '").append(version).append("'")
		.append(", Sender: '").append(sender).append("'")
		.append(", OrigSender: '").append(origSender).append("'")
		.append(", AppId: '").append(appId).append("'")
		.append(", AppName: '").append(appName).append("'")
		.append(", CustomAttributes: '").append(customAttributes).append("'")
		.append(", TextMessage: '").append(text).append("'");
		
		if ( richIconUrl != null && richIconUrl.length() > 0 ) {
			sb.append(", RichIconURL: '").append(richIconUrl).append("'");
		}
		
		if ( richIconObjPath != null && richIconObjPath.length() > 0 ) {
			sb.append(", RichIconObjPath: '").append(richIconObjPath).append("'");
		}
		
		if ( richAudioUrl != null && richAudioUrl.size() > 0 ) {
			sb.append(", RichAudioUrl: '").append(richAudioUrl).append("'");
		}
		
		if ( richAudioObjPath != null && richAudioObjPath.length() > 0 ) {
			sb.append(", RichAudioObjPath: '").append(richAudioObjPath).append("'");
		}
		
		if ( responseObjectPath != null && responseObjectPath.length() > 0 ) {
			sb.append(", ResponseObjPath: '").append(responseObjectPath).append("'");
		}
		
		return sb.toString();
	}//toString
	
	//=======================================//
	


	/**
	 * Set message signature version
	 * @param version
	 */
	void setVersion(int version) {
		this.version = version;
	}
		
	/**
	 * Set message Id
	 * @param messageId
	 */
	void setMessageId(int messageId) {
		this.messageId = messageId;
	}
	
	/**
	 * Set app name
	 * @param appName
	 */
	void setAppName(String appName) {
		this.appName = appName;
	}
	
	/**
	 * Set app Id
	 * @param appId
	 */
	void setAppId(UUID appId) {
		this.appId = appId;
	}
	
	/**
	 * Set device name
	 * @param appName
	 */
	void setDeviceName(String deviceName) {
		this.deviceName = deviceName;
	}
	
	/**
	 * Set device Id
	 * @param deviceId
	 */
	void setDeviceId(String deviceId) {
		this.deviceId = deviceId;
	}
	
	/**
	 * Set sender Id
	 * @param sender
	 */
	void setSender(String sender) {
		this.sender = sender;
	}
	
	/**
	 * Set the original sender name
	 * @param origSender
	 */
	void setOrigSender(String origSender) {
		this.origSender = origSender;
	}
	
}//NotificationImpl
