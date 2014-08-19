/******************************************************************************
 * Copyright (c) 2013-2014, AllSeen Alliance. All rights reserved.
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

import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.UUID;

import org.alljoyn.bus.Variant;
import org.alljoyn.ns.commons.GenericLogger;
import org.alljoyn.ns.commons.NativePlatformFactory;
import org.alljoyn.ns.commons.NativePlatformFactoryException;
import org.alljoyn.ns.transport.Transport;
import org.alljoyn.ns.transport.TransportNotificationText;
import org.alljoyn.ns.transport.TransportRichAudioUrl;
import org.alljoyn.ns.transport.interfaces.NotificationTransport;


/**
 * Mediate between variables structure sent over AJ network to Notification object structure and vice versa   
 */
public class PayloadAdapter {
		
	private enum ArgumentKey {
		RICH_NOTIFICATION_ICON_URL(0),
		RICH_NOTIFICATION_AUDIO_URL(1),
		RICH_NOTIFICATION_ICON_OBJECT_PATH(2),
		RICH_NOTIFICATION_AUDIO_OBJECT_PATH(3),
		RESPONSE_OBJECT_PATH(4),
		ORIGINAL_SENDER_NAME(5)
		;

		/**
		 * The argument key id
		 */
		public final int ID;
		
		/**
		 * Constructor
		 * @param id The key id
		 */
		private ArgumentKey(int id) {
			ID = id;
		}
		
		/**
		 * Returns Argument key by id or null if not found
		 * @param id Id to look for
		 * @return 
		 */
		public static ArgumentKey getArgumentKeyById(int id) {
			ArgumentKey[] args = ArgumentKey.values();
			ArgumentKey retVal = null;
			for (ArgumentKey arg : args) {
				if ( arg.ID == id ) {
					retVal = arg;
					break;
				}
			}
			return retVal;
		}//getArgumentKeyById
	}//ArgumentKeys
	//==========================================
	
	private static final String TAG = "ioe" + PayloadAdapter.class.getSimpleName();
	
	
	/**
	 * Random number to start msg Id 
	 */ 
	private static int msgId = (int)(Math.random()*10000);
	
	/**
	 * Convert the sent data into the format of {@link NotificationTransport#notify(int, int, short, String, String, byte[], String, Map, Map, TransportNotificationText[])}  
	 * @param deviceId device Id
	 * @param deviceName device name
	 * @param appId app Id
	 * @param appName app name
	 * @param messageType Notification message type
	 * @param text list of NotificationText objects
	 * @param customAttributes map of customized array variables
	 * @param ttl Notification message TTL
	 * @param richIconUrl may be null
	 * @param richAudioUrl list by language may be null
	 * @param richIconObjPath may be null
	 * @param richAudioObjPath list by language may be null
	 * @param responseObjectPath CPS object path
	 * @throws NotificationServiceException
	 */
	public static void sendPayload(String deviceId, String deviceName, UUID appId, String appName, NotificationMessageType messageType, List<NotificationText> text, Map<String, String> customAttributes, int ttl, String richIconUrl, List<RichAudioUrl> richAudioUrl, String richIconObjPath, String richAudioObjPath, String responseObjectPath) throws NotificationServiceException {
		GenericLogger logger;
		
		try {
			logger = NativePlatformFactory.getPlatformObject().getNativeLogger();
		} catch (NativePlatformFactoryException npfe) {
			throw new NotificationServiceException(npfe.getMessage());
		}
		
		logger.debug(TAG, "Preparing message for sending...");
		
		//Prepare attributes Map
		Map<Integer, Variant> attributes = new HashMap<Integer, Variant>();
		
		if ( richIconUrl != null ) {
			logger.debug(TAG, "Preparing to send richIconUrl...");			
			attributes.put(ArgumentKey.RICH_NOTIFICATION_ICON_URL.ID, new Variant(richIconUrl, "s"));
		}//if :: richIconUrl
		
		if ( richAudioUrl != null ) {
			logger.debug(TAG, "Preparing to send richAudioUrl...");			
			//Prepare TransportRichAudioUrl array
			int u = 0;
			TransportRichAudioUrl[] audioUrl = new TransportRichAudioUrl[richAudioUrl.size()];
			for ( RichAudioUrl au : richAudioUrl ) {
				audioUrl[u] = TransportRichAudioUrl.buildInstance(au);
				++u;
			}
			attributes.put(ArgumentKey.RICH_NOTIFICATION_AUDIO_URL.ID, new Variant(audioUrl, "ar"));
		}//if :: richAudioUrl
		
		if ( richIconObjPath != null ) {
			logger.debug(TAG, "Preparing to send richIconObjPath...");			
			attributes.put(ArgumentKey.RICH_NOTIFICATION_ICON_OBJECT_PATH.ID, new Variant(richIconObjPath, "s"));
		}//if :: richIconUrl
		
		if ( richAudioObjPath != null ) {
			logger.debug(TAG, "Preparing to send richAudioObjPath...");			
			attributes.put(ArgumentKey.RICH_NOTIFICATION_AUDIO_OBJECT_PATH.ID, new Variant(richAudioObjPath, "s"));
		}//if :: richAudioUrl
		
		if ( responseObjectPath != null ) {
			logger.debug(TAG, "Preparing to sendCPS Ocject Path...");			
			attributes.put(ArgumentKey.RESPONSE_OBJECT_PATH.ID, new Variant(responseObjectPath, "s"));
		}//if :: responseObjectPath	
		
		//Add the notification producer sender name to the sent attributes
		String sender = Transport.getInstance().getBusAttachment().getUniqueName();
		attributes.put(ArgumentKey.ORIGINAL_SENDER_NAME.ID, new Variant(sender, "s"));
		
		//Process customAttributes
		if ( customAttributes != null ) {
			logger.debug(TAG, "Preparing customAttributes...");
			
			for(String key : customAttributes.keySet()) {
				if (key == null) {
					throw new NotificationServiceException("The key of customAttributes can't be null");
				}
				
				String customVal = customAttributes.get(key);
				if (customVal == null) {
					throw new NotificationServiceException("The value of customAttributes for key: '" + key + "' can't be null");
				}
			}//for :: customAttributes
			
		}//if :: customAttributes
		else {
			// Create an empty customAttributes to prevent sending NULL
			customAttributes = new HashMap<String, String>();
		}
		
		//Prepare TransportNotificationText array
		int i = 0;
		TransportNotificationText[] sentText = new TransportNotificationText[text.size()];
		for ( NotificationText nt : text ) {
			sentText[i] = TransportNotificationText.buildInstance(nt);
			++i;
		}
		
		Transport.getInstance().sendNotification(NotificationService.PROTOCOL_VERSION,
												 genMsgId(),
												 messageType,
												 deviceId,
												 deviceName,
												 uuidToByteArray(appId), 
												 appName,												 
												 attributes,
												 customAttributes,
												 sentText, 
												 ttl
		);
	}//sendPayload
	
	/**
	 * Convert the received data into the format of {@link Notification} object
	 * @param version The version of the message signature
	 * @param msgId notifId notification Id
	 * @param sender The unique name of the sender end point, may be used for session establishment
	 * @param messageType Message type id
	 * @param deviceId Device Id
	 * @param deviceName Device Name
	 * @param bAppId app id
	 * @param appName app Name
	 * @param attributes Notification metadata 
	 * @param customAttributes customAttributes
	 * @param text List of NotificationText objects
	 * @throws NotificationServiceException if failed to parse the received notification 
	 */
	public static void receivePayload(int version, int msgId, String sender, short messageType, String deviceId, String deviceName, byte[] bAppId, String appName, Map<Integer, Variant> attributes, Map<String, String> customAttributes, TransportNotificationText[] text) throws NotificationServiceException {
		try {
				GenericLogger logger = NativePlatformFactory.getPlatformObject().getNativeLogger();
				logger.debug(TAG, "Received notification Id: '" + msgId + "', parsing...");

				// Decode message type
				NotificationMessageType msgType = NotificationMessageType.getMsgTypeById(messageType); 
				if ( msgType == null ) {
					throw new NotificationServiceException("Received unknown message type id: '" + messageType + "'");
				}
				
				//Build List of NotificationText
				List<NotificationText> textList = new LinkedList<NotificationText>();
				for (TransportNotificationText trNt : text) {
					textList.add(new NotificationText(trNt.language, trNt.text));
				}				
				
				// Create the Notification object
				Notification notif = new Notification(msgType, textList);
				
				//convert appId received as a byte[] to a string
				UUID appId = byteArrayToUUID(bAppId);
				
				if ( appId == null ) {
					String msg = "Received a bad length of byte array that doesn't represent UUID of an appId";
			        logger.error(TAG, msg);
			        throw new NotificationServiceException(msg);
				}
				
				notif.setVersion(version);
				notif.setMessageId(msgId);
				notif.setDeviceId(deviceId);
				notif.setDeviceName(deviceName);
				notif.setSender(sender);
				notif.setAppId(appId);
				notif.setAppName(appName);
								
				//parse attributes
				for ( Integer key : attributes.keySet() ) {
					Variant vObj        = attributes.get(key);
					ArgumentKey attrKey = ArgumentKey.getArgumentKeyById(key);
					
					if ( attrKey == null ) {
						logger.warn(TAG, "An unknown attribute key: '" + key + "' received, ignoring the key");
						continue;
					}
					
					switch(attrKey) {
						case RICH_NOTIFICATION_ICON_URL: {
							String iconUrl = vObj.getObject(String.class);
							logger.debug(TAG, "Received rich icon url: '" + iconUrl + "'");
							notif.setRichIconUrl(iconUrl);
							break;
						}//RICH_NOTIFICATION_ICON_URL
						case RICH_NOTIFICATION_AUDIO_URL: {
							TransportRichAudioUrl[] audioUrl = vObj.getObject(TransportRichAudioUrl[].class);
							if (audioUrl.length != 0) {
								List<RichAudioUrl> audioUrlList = new ArrayList<RichAudioUrl>(audioUrl.length);
								logger.debug(TAG, "Received rich audio url");
								for (TransportRichAudioUrl trRichAudioUrl : audioUrl) {
									audioUrlList.add(new RichAudioUrl(trRichAudioUrl.language, trRichAudioUrl.url));
								}
								notif.setRichAudioUrl(audioUrlList);
							}	
							break;
						}//RICH_NOTIFICATION_AUDIO_URL
						case RICH_NOTIFICATION_ICON_OBJECT_PATH: {
							String iconObjPath = vObj.getObject(String.class);
							logger.debug(TAG, "Received rich icon object path: '" + iconObjPath + "'");
							notif.setRichIconObjPath(iconObjPath);
							break;
						}
						case RICH_NOTIFICATION_AUDIO_OBJECT_PATH: {
							String audioObjPath = vObj.getObject(String.class);  
							logger.debug(TAG, "Received rich audio object path");
							notif.setRichAudioObjPath(audioObjPath);
							break;
						}
						case RESPONSE_OBJECT_PATH: {
							String responseObjectPath = vObj.getObject(String.class);
							logger.debug(TAG, "Received a Response ObjectPath object path: '" + responseObjectPath + "'");
							notif.setResponseObjectPath(responseObjectPath);
							break;
						}
						case ORIGINAL_SENDER_NAME: {
							String origSender = vObj.getObject(String.class);
							logger.debug(TAG, "Received an original sender: '" + origSender + "'");
							notif.setOrigSender(origSender);
							break;
						}
						
					}//switch
				}//for :: attributes
				
				logger.debug(TAG, "Set the custom attributes");
				// set the custom attributes
				notif.setCustomAttributes(customAttributes);				
				
				Transport.getInstance().onReceivedNotification(notif);
		}//try
		catch (Exception e) {
			throw new NotificationServiceException("Failed to parse received notification payload, Error: " + e.getMessage());
		}
	}//receivePayload
	
	/**
	 * Generate a notification id string using random string and msgIdAI
	 * @return Notification id in format: random number concatenated to msgAI(Auto incremented)
	 */
	public static int genMsgId() {				
		if ( msgId == Integer.MAX_VALUE ) {
			msgId = 0;
		}		
		return  ++msgId;
	}
	
	/**
	 * Convert the byte array to a UUID
	 * @param bAppId Byte Array to be converted to {@link UUID}
	 * @return UUID {@link UUID} object or NULL if failed to create
	 */
    public static UUID byteArrayToUUID(byte[] bAppId) {

    	long msUuid = 0;
    	long lsUuid = 0;
    	
    	if ( bAppId.length != 16) {
            return null;
    	}
    	
    	for (int i = 0; i < 8; i++) {
    		msUuid = (msUuid << 8) | (bAppId[i] & 0xff);
    	}
    	
    	for (int i = 8; i < 16; i++) {
    		lsUuid = (lsUuid << 8) | (bAppId[i] & 0xff);
    	}
    	
    	UUID result = new UUID(msUuid, lsUuid);

    	return result;
    }//byteArrayToUUID
    
	
	/**
	 * Convert from UUID object into a byte array
	 * @param uuid
	 * @return byte array
	 */
    public static byte[] uuidToByteArray(UUID uuid) {
    	long msUuid = uuid.getMostSignificantBits();
    	long lsUuid = uuid.getLeastSignificantBits();
    	byte[] byteArrayUuid = new byte[16];

    	for (int i = 0; i < 8; i++) {
    		byteArrayUuid[i] = (byte) (msUuid >>> 8 * (7 - i));
    	}
    	for (int i = 8; i < 16; i++) {
    		byteArrayUuid[i] = (byte) (lsUuid >>> 8 * (7 - i));
    	}

    	return byteArrayUuid;
    }//uuidToByteArray
    
}//PayloadAdapter
