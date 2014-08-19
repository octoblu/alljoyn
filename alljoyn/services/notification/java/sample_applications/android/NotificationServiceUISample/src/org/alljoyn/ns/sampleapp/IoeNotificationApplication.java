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

package org.alljoyn.ns.sampleapp;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.UUID;

import org.alljoyn.about.AboutKeys;
import org.alljoyn.about.AboutService;
import org.alljoyn.about.AboutServiceImpl;
import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.SessionOpts;
import org.alljoyn.bus.Status;
import org.alljoyn.bus.alljoyn.DaemonInit;
import org.alljoyn.ns.Notification;
import org.alljoyn.ns.NotificationMessageType;
import org.alljoyn.ns.NotificationReceiver;
import org.alljoyn.ns.NotificationSender;
import org.alljoyn.ns.NotificationService;
import org.alljoyn.ns.NotificationServiceException;
import org.alljoyn.ns.NotificationText;
import org.alljoyn.ns.RichAudioUrl;
import org.alljoyn.services.android.storage.Property;
import org.alljoyn.services.android.storage.PropertyStoreImpl;
import org.alljoyn.services.common.PropertyStore.Filter;
import org.alljoyn.services.common.PropertyStoreException;

import android.app.Application;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Intent;
import android.media.RingtoneManager;
import android.os.Vibrator;
import android.support.v4.app.NotificationCompat;
import android.util.Log;
import android.widget.Toast;


public class IoeNotificationApplication extends Application implements NotificationReceiver {
	private static final String TAG = "ioe" + IoeNotificationApplication.class.getSimpleName();
	
    static {
	     System.loadLibrary("alljoyn_java");
	}   
	 	
    /** 
     * The daemon should advertise itself "quietly" (directly to the calling port)
     * This is to reply directly to a TC looking for a daemon 
     */  
    private static final String DAEMON_NAME_PREFIX          = "org.alljoyn.BusNode.IoeService";
     
    /** 
     * The daemon should advertise itself "quietly" (directly to the calling port)
     * This is to reply directly to a TC looking for a daemon 
     */  
    private static final String DAEMON_QUIET_PREFIX         = "quiet@";

    /**
     * The {@link BusAttachment} to be used by the {@link NotificationService}
     */
    private BusAttachment bus;
     
    /**
     * About service
     */
    private AboutService aboutService;
     
	/**
	 * Device Name
	 */
	private static final String DEVICE_NAME      = "deviceName-Droid-XYZ";

	/**
	 * Rich content icon URL
	 */
	private static final String ICON_URL 	     = "http://richIcon.com?icon=alljoyn";
	
	/**
	 * Rich content icon Object Path
	 */
	private static final String ICON_OBJ_PATH 	 = "/OBJ/PATH/ICON";
	
	/**
	 * Rich content audio Object Path
	 */
	private static final String AUDIO_OBJ_PATH 	 = "/OBJ/PATH/AUDIO";
		
	/**
	 * For testers who don't want the app to filter notifications on the app name
	 */
	public static final String DEFAULT_APP_NAME  = "DISPLAY_ALL";
	
	/**
	 * App name
	 */
	private String appName = "";
	
	
	/**
	 * Reference to NotificationService object
	 */
	private NotificationService notificationService;
	
	/**
	 * Reference to NotificationSender object
	 */
	private NotificationSender notificationSender;
		
	/**
	 * Gets TRUE if the application running on the BG
	 */
	private boolean isBackground;
	
	/**
	 * Set to TRUE when the NotificationSender is started
	 */
	private boolean isSenderStarted;
	
	/**
	 * Set to TRUE when the NotificationReceiver is started
	 */
	private boolean isReceiverStarted;
	
	/**
	 * Vibration duration, milliseconds
	 */
	private static final long VIBRATION_DURATION = 1500;

	/**
	 * UI Activity 
	 */
	private NotificationServiceControlsActivity myActiv;
	
	
	/**
	 * Method called when the application started
	 */
	@Override	
	public void onCreate() {
		super.onCreate();
		Log.i(TAG,"Starting IoeNotificationApplication: calling createService");
		aboutService = AboutServiceImpl.getInstance();

		//Create my own BusAttachment
        DaemonInit.PrepareDaemon(this);
		notificationService = NotificationService.getInstance();
	}//onCreate

	/**
	 * 
	 * @param sampleAppActivity
	 */
	public void setSampleAppActivity(NotificationServiceControlsActivity sampleAppActivity) {
		this.myActiv = sampleAppActivity;
	}
	
	/**
	 * @param isBackground
	 */
	public void setBackground(boolean isBackground) {
		this.isBackground = isBackground;
	}

	/**
	 * @return {@link BusAttachment}
	 */
	public BusAttachment getBusAttachment() {
		return bus;
	}//getBusAttachment
	
	/**
	 * Returns App Name
	 * @return
	 */
	public String getAppName() {
		return appName;
	}//getAppName
	

	/**
	 * Set the AppName
	 * @param appName
	 */
	public void setAppName(String appName) {
		this.appName = appName;
	}//setAppName
	
	/**
	 * called when checking the Producer checkbox
	 * @throws NotificationServiceException
	 */
	public void startSender() {
		try {
			
			if ( bus == null ) {
				Log.i(TAG, "Initializing AllJoyn");
				prepareAJ();
			}
			
			//////start about
            Map<String, Object> config  = new HashMap<String, Object>();
			PropertyStoreImpl propertyStore = new PropertyStoreImpl(this);
			propertyStore.readAll(Property.NO_LANGUAGE, Filter.READ, config);
			
			String deviceName = (String)config.get(AboutKeys.ABOUT_DEVICE_NAME);
	   	    if ( deviceName == null || deviceName.length() == 0 ) {
	   	    	propertyStore.setValue(AboutKeys.ABOUT_DEVICE_NAME, DEVICE_NAME, Property.NO_LANGUAGE);
	   	    }
	   	    
	        propertyStore.setValue(AboutKeys.ABOUT_APP_NAME, appName, Property.NO_LANGUAGE);
	   	    
            try {
                aboutService.startAboutServer((short)1080, propertyStore, bus);
            }
            catch (Exception e) {
                Log.e(TAG, "AboutConfigService failed, Error: " + e.getMessage());
            }
			////////end about
			
            notificationSender = notificationService.initSend(bus, propertyStore);
            isSenderStarted    = true;
		}
		catch (PropertyStoreException pse) {
			Log.e(TAG,"Failed on startSender - Failed to store sender settings in the PropertyStore, Error: '" + pse.getMessage() + "'");
			showToast("Failed to start sender");
			vibrate();
			Log.d(TAG, "Set the UI as shutdown");
			myActiv.onShutdownClicked(false);
		}
		catch (NotificationServiceException nse) {
			Log.e(TAG,"Failed on startSender - can't create a notificationSender error: " + nse.getMessage());
			showToast("Failed to start sender");
			vibrate();
			Log.d(TAG, "Set the UI as shutdown");
			myActiv.onShutdownClicked(false);
		}
	}//startSender
	
	/**
	 * called when checking the Consumer checkbox
	 */
	public void startReceiver() {
		try {
			
			if ( bus == null ) {
				Log.i(TAG, "Initializing AllJoyn");
				prepareAJ();
			}
			
			aboutService.startAboutClient(bus);
			
			notificationService.initReceive(bus, this);
			isReceiverStarted = true;
			
		} catch (NotificationServiceException nse) {
			Log.e(TAG,"Failed on startReceiver - can't present notifications error: " + nse.getMessage());
			showToast("Failed to start receiver");
			vibrate();
		}
		catch (Exception e) {
			Log.e(TAG, "Unexpected error occurred, failed to start the NotificationReceiver, Error: '" + e.getMessage() + "'");
			showToast("Failed to start the NotificationReceiver");
			vibrate();
		}
	}//startReceiver
	
	/**
	 * delete a notification by a given message type
	 * @param messageType
	 */
	public void delete(String messageType) {
		Log.d(TAG, "Received delete, executing");
		if (notificationSender != null) {
			try {
				notificationSender.deleteLastMsg(NotificationMessageType.valueOf(messageType));
			} catch (NotificationServiceException nse) {
				Log.e(TAG,"Failed on deleteLastMsg - can't delete notifications error: " + nse.getMessage());
				showToast("Failed to delete notification");
				vibrate();
			}
		} else {
			Log.e(TAG,"NotificationSender is not ready");
		}
	}//delete
	
	/**
	 * Sending notification
	 * @param messageType	    messageType
	 * @param customAttributes	fontText matching
	 * @param textList		    holds text and language
	 * @param ttl               Notification message ttl
	 */
	public void send(String messageType, List<NotificationText> textList, Map<String, String> customAttributes, int ttl, boolean isIcon, boolean isAudio, boolean isIconObjPath, boolean isAudioObjPath) {
		Log.d(TAG, "Received send, executing");
		
		if(notificationSender != null) {
			Log.d(TAG,"Trying to send notification");
			
			String richIconUrl = null;	
			List<RichAudioUrl> audioUrl = null;
			if ( isIcon ) {
				richIconUrl = ICON_URL;
			}
			if ( isAudio ) {
				try {
					audioUrl = new LinkedList<RichAudioUrl>();
					audioUrl.add(new RichAudioUrl("en", "http://iamaaudiourl.com?lang=en"));
					audioUrl.add(new RichAudioUrl("he", "http://iamaaudiourl.com?lang=he"));
				} catch (NotificationServiceException e) {
					Log.d(TAG, "failed to create audio url: " + e);
				}
			}
			
			String richIconObjPath = null;	
			if ( isIconObjPath ) {
				richIconObjPath = ICON_OBJ_PATH;
			}
			
			String richAudioObjPath = null;
			if ( isAudioObjPath ) {
				richAudioObjPath = AUDIO_OBJ_PATH;
			}
						
			try {
				    Notification notif = new Notification(NotificationMessageType.valueOf(messageType), textList);
				    notif.setCustomAttributes(customAttributes);
				    notif.setRichIconUrl(richIconUrl);
				    notif.setRichAudioUrl(audioUrl);
				    notif.setRichIconObjPath(richIconObjPath);
				    notif.setRichAudioObjPath(richAudioObjPath);
				    
					notificationSender.send(notif, ttl);
					
			} catch (NotificationServiceException nse) {
				Log.e(TAG,"Failed to send notification error: " + nse.getMessage());
				showToast("Failed to send notification");
				vibrate();
			}
		} else {
			Log.e(TAG,"NotificationSender is not ready");
		}
	}//send

	/**
	 * Called by NotificationService to receive notifications
	 * @param notification received from NotificationService
	 */
	@Override
	public void receive(Notification notification) {
		
		Log.d(TAG, "Receveid new " + notification);

		String notifAppName = notification.getAppName();
		//If received notification application name isn't equals to my application name, ignore the notification
		if ( (!notifAppName.equals(appName)) && (!appName.equals(DEFAULT_APP_NAME))) {
			Log.d(TAG, "The received Message Id: '" + notification.getMessageId() +  "', belongs to app: '" + notifAppName + "', my app is '" + appName + "', ignoring");
			return;
		}

		renderNotification(notification);
	}//receive

	/**
	 * @see org.alljoyn.ns.NotificationReceiver#dismiss(int, java.util.UUID)
	 */
	@Override
	public void dismiss(int notifId, UUID appId) {
		Log.d(TAG, " !!!!! DISMISS RECEIVED !!!! : '" + notifId + "', appId: '" + appId + "'");
		myActiv.handleDismiss(notifId, appId);
	}//dismiss
	
	/**
	 * Send the notification message to be presented
	 * @param notification
	 */
	public void renderNotification(Notification notification) {
        myActiv.showNotification(notification);
		if ( isBackground ) {
			Log.d(TAG, "Found the app in BG, sending Android Notification");
			showNotification(null);
		}
	}//renderNotification
	
	/**
	 * Stops sender service
	 */
	public void stopSender() {
		Log.d(TAG, "Stopping sender service");
		try {
			notificationService.shutdownSender();
			aboutService.stopAboutServer();
			isSenderStarted = false;
		}
		catch (NotificationServiceException nse) {
			Log.e(TAG,"Failed on stopping the sender service, Error: " + nse.getMessage());
			showToast("Failed to stop Notification sender service");
			vibrate();
		}
		catch(Exception e) {
			Log.e(TAG, "Unexpected error occurred, failed to stop the NotificationSender, Error: '" + e.getMessage() + "'");
		}
	}//stopSender
	
	/**
	 * Stops receiver service
	 */
	public void stopReceiver() {
		Log.d(TAG, "Stopping receiver service");
		try {
			notificationService.shutdownReceiver();
			AboutServiceImpl.getInstance().stopAboutClient();
			isReceiverStarted = false;
		}
		catch (NotificationServiceException nse) {
			Log.e(TAG,"Failed on stopping the receiver service, Error: " + nse.getMessage());
			showToast("Failed to stop the Notification receiver service");
			vibrate();
		}
		catch(Exception e) {
			Log.e(TAG, "Unexpected error occurred, failed to stop the NotificationReceiver");
			showToast("Failed to stop the Notification receiver service");
			vibrate();
		}
	}//stopReceiver
	
	/**
	 * Calling to Shutdown Notification Service
	 */
	public void shutdown() {
		Log.d(TAG, "Received shutdown, executing");
		try {
			notificationService.shutdown();
			
			if ( isSenderStarted ) {
				aboutService.stopAboutServer();
			}
			if ( isReceiverStarted ) {
				aboutService.stopAboutClient();
			}
			
			bus.disconnect();
			bus.release();
			bus = null;
			Log.d(TAG, "Shutdown was done");
			
			isReceiverStarted = false;
			isSenderStarted   = false;
		} catch (NotificationServiceException nse) {
			Log.e(TAG,"Failed on shutting down the service, Error: " + nse.getMessage());
			showToast("Failed to stop Notification Service");
			vibrate();
		}
		catch (Exception e) {
			Log.e(TAG, "Unexpected error occurred, failed to shutdown the NotificationService");
		}
	}//shutdown
	
   /**
    * Show the Android toast message
    * @param msg
    */
    public void showToast(final String msg) {
         Log.d(TAG, "Showing Toast: '" + msg + "'");
         
         if ( myActiv != null ) {
        	 
	         myActiv.runOnUiThread( new Runnable() {             // Run the Toast on the Activity UI thread
				@Override
				public void run() {
					Toast toast = Toast.makeText(IoeNotificationApplication.this, msg, Toast.LENGTH_LONG);
					toast.show();
				}
			 }); 
         }
         else {
     		Toast toast = Toast.makeText(this, msg, Toast.LENGTH_LONG);
			toast.show();
         }
    }//showToast
    
   /**
    * Vibrate notification
    */
    public void vibrate() {
         Vibrator v = (Vibrator) getSystemService(VIBRATOR_SERVICE);
         v.vibrate(VIBRATION_DURATION);             // Vibrate time in milli seconds
    }//vibrate

    /**
     * Present notification as Android notification
     * @param msg if is NULL or empty string the default will be used 
     */
    private void showNotification(String msg) {
         long[] VIBRATION_PATTERN = {1L, 250L, 100L, 250L, 100L, 250L};
         NotificationManager notificationManager = (NotificationManager)getSystemService(NOTIFICATION_SERVICE);
         
         final String deflMsg = "New notification received";
         
         if ( msg == null || msg.length() == 0 ) {
        	 msg = deflMsg;
         }
         
         Intent notifyIntent = new Intent(this, NotificationServiceControlsActivity.class);
         notifyIntent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP);
         
          android.app.Notification notification = new NotificationCompat.Builder(this)
            .setContentTitle(deflMsg)
            .setContentText(msg)
            .setSmallIcon(R.drawable.ic_launcher)
            .setVibrate(VIBRATION_PATTERN)
            .setAutoCancel(true)
            .setTicker(msg)
            .setWhen(System.currentTimeMillis())
            .setSound(RingtoneManager.getActualDefaultRingtoneUri(this, RingtoneManager.TYPE_NOTIFICATION))
            .setContentIntent(PendingIntent.getActivity(this, 2, notifyIntent, PendingIntent.FLAG_UPDATE_CURRENT))
            .build();
            notificationManager.notify(1, notification);
     }//showNotification

    
    //================================================//
    
    /**
     * Performs all the preparation before starting the service
     */
    private void prepareAJ() throws NotificationServiceException {

        Log.d(TAG, "Create the BusAttachment");
        bus = new BusAttachment("NotificationService", BusAttachment.RemoteMessage.Receive);

        //For verbose AJ logging use the following lines
        //bus.setDaemonDebug("ALL", 7);
        //bus.setLogLevels("ALLJOYN=7");
        //bus.setLogLevels("SESSIONLESS=2");
        //bus.setLogLevels("ALL=7");
        //bus.useOSLogging(true);

        Status conStatus = bus.connect();
        if ( conStatus != Status.OK ) {
            Log.e(TAG, "Failed connect to bus, Error: '" + conStatus + "'");
            throw new NotificationServiceException("Failed connect to bus, Error: '" + conStatus + "'");
        }

        //Advertise the daemon so that the thin client can find it
        advertiseDaemon();
    }//prepareAJ

    /**
     * Advertise the daemon so that the thin client can find it
     * @param logger
     */
   private void advertiseDaemon() throws NotificationServiceException {
       int flag = BusAttachment.ALLJOYN_REQUESTNAME_FLAG_DO_NOT_QUEUE;
       
       String daemonName = DAEMON_NAME_PREFIX + ".G" + bus.getGlobalGUIDString();
       
       //request the name   
       Status reqStatus = bus.requestName(daemonName, flag);
        if (reqStatus == Status.OK) {
        	
            //advertise the name with a quite prefix for TC to find it
            Status adStatus = bus.advertiseName(DAEMON_QUIET_PREFIX + daemonName, SessionOpts.TRANSPORT_ANY);
            
            if (adStatus != Status.OK){
            	
                bus.releaseName(daemonName); 
                Log.e(TAG, "Failed to advertise daemon name " + daemonName + ", Error: '" + adStatus + "'"); 
                throw new NotificationServiceException("Failed to advertise daemon name '" + daemonName + "', Error: '" + adStatus + "'"); 
            } 
            else{ 
                Log.d(TAG, "Succefully advertised daemon name: '" + daemonName + "', BusName: '" + bus.getUniqueName() + "'"); 
            }
        }
        else {
        	Log.d(TAG, "Failed to request the daemon name: '" + daemonName + "', Error: '" + reqStatus + "'");
        	throw new NotificationServiceException("Failed to request the DaemonName: '" + daemonName + "', Error: '" + reqStatus + "'");
        }
        
    }//advertiseDaemon

}//IoeNotificationApplication
