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
package org.allseen.timeservice.sample.client.application;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.List;
import java.util.Map;
import java.util.UUID;

import org.alljoyn.about.AboutKeys;
import org.alljoyn.bus.AboutListener;
import org.alljoyn.bus.AboutObjectDescription;
import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.BusException;
import org.alljoyn.bus.Variant;
import org.alljoyn.services.common.utils.TransportUtil;
import org.allseen.timeservice.client.Alarm;
import org.allseen.timeservice.client.Alarm.AlarmHandler;
import org.allseen.timeservice.client.Clock;
import org.allseen.timeservice.client.Clock.TimeAuthorityHandler;
import org.allseen.timeservice.client.TimeServiceClient;
import org.allseen.timeservice.client.Timer;
import org.allseen.timeservice.client.Timer.TimerHandler;

import android.content.Context;
import android.content.Intent;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;

/**
 * Manages the devices 'seen' by the application.
 * 
 * Implements the AnnouncementHandler to receive Announcement of timer service
 * type {@link ProtocolManager#TIMER_INTERFACE}
 * 
 * 
 */
public class DeviceManager implements AboutListener {

    private static final String TAG = "DeviceManager";

    /**
     * stores the context.
     */
    private Context context;

    /**
     * store the devices in a map for quick retrieval.
     */
    private final Map<UUID, Device> devicesMap = new Hashtable<UUID, Device>();

    /**
     * Base class to wrap signal received from TimerService objects.
     */
    public static abstract class SignalObject {

        protected Context context;
        protected Object signalParent;
        protected String serviceName;

        public SignalObject(Context context, Object signalParent, String serviceName) {
            this.context = context;
            this.signalParent = signalParent;
            this.serviceName = serviceName;
        }

        public Object getSignalParent() {
            return signalParent;
        }
    }

    /**
     * Wrapper class to handle Timer signals {@link TimerHandler} Sends internal
     * broadcast messages .
     */
    public static class TimerHandlerImpl extends SignalObject implements TimerHandler {

        public TimerHandlerImpl(Context context, Object signalParent, String serviceName) {
            super(context, signalParent, serviceName);
        }

        @Override
        public void handleTimerEvent(Timer timer) {
            Log.d(TAG, "Received handleTimerEvent from" + timer.getObjectPath());
            Intent t = new Intent(TimeSampleClient.TIMER_EVENT_SIGNAL_ACTION);
            t.putExtra(TimeSampleClient.MESSAGE_KEY, "Timer  has expired");
            t.putExtra(TimeSampleClient.OBJECT_PATH_KEY, timer.getObjectPath());
            t.putExtra(TimeSampleClient.SERVICE_NAME_KEY, serviceName);
            LocalBroadcastManager.getInstance(context).sendBroadcast(t);
        }

        @Override
        public void handleRunStateChanged(Timer timer, boolean isRunning) {
            Log.d(TAG, "Received handleRunStateChanged from" + timer.getObjectPath());
            Intent t = new Intent(TimeSampleClient.TIMER_EVENT_SIGNAL_ACTION);
            t.putExtra(TimeSampleClient.MESSAGE_KEY, "Timer  changed state to " + isRunning);
            t.putExtra(TimeSampleClient.OBJECT_PATH_KEY, timer.getObjectPath());
            t.putExtra(TimeSampleClient.SERVICE_NAME_KEY, serviceName);
            LocalBroadcastManager.getInstance(context).sendBroadcast(t);
        }

    }

    /**
     * Wrapper class to handle Alarm signals {@link AlarmHandler} Sends internal
     * broadcast messages .
     */
    public static class AlarmHandlerImpl extends SignalObject implements AlarmHandler {

        public AlarmHandlerImpl(Context context, Object signalParent, String serviceName) {
            super(context, signalParent, serviceName);
        }

        @Override
        public void handleAlarmReached(Alarm alarm) {
            Log.d(TAG, "Received handleAlarmReached from" + alarm.getObjectPath());
            Intent t = new Intent(TimeSampleClient.ALARM_EVENT_SIGNAL_ACTION);
            t.putExtra(TimeSampleClient.MESSAGE_KEY, "Alarm  has expired");
            t.putExtra(TimeSampleClient.OBJECT_PATH_KEY, alarm.getObjectPath());
            t.putExtra(TimeSampleClient.SERVICE_NAME_KEY, serviceName);
            LocalBroadcastManager.getInstance(context).sendBroadcast(t);
        }
    }

    /**
     * Wrapper class to handle TimeAuthority signals
     * {@link TimeAuthorityHandlerImpl} Sends internal broadcast messages .
     */
    public static class TimeAuthorityHandlerImpl extends SignalObject implements TimeAuthorityHandler {

        public TimeAuthorityHandlerImpl(Context context, Object signalParent, String serviceName) {
            super(context, signalParent, serviceName);
        }

        @Override
        public void handleTimeSync(Clock clock) {
            Log.d(TAG, "Received handleTimeSync from" + clock.getObjectPath());
            Intent t = new Intent(TimeSampleClient.TIME_AUTHORITY_EVENT_SIGNAL_ACTION);
            t.putExtra(TimeSampleClient.MESSAGE_KEY, "Clock has been received");
            t.putExtra(TimeSampleClient.OBJECT_PATH_KEY, clock.getObjectPath());
            t.putExtra(TimeSampleClient.SERVICE_NAME_KEY, serviceName);
            LocalBroadcastManager.getInstance(context).sendBroadcast(t);
        }
    }

    /**
     * Stores the device information.
     */
    public static class Device {
        public String serviceName;
        public short port;
        public UUID appId;
        public String deviceId;
        public String deviceName;
        public long lastSeen;

        /**
         * instance of the TimeServiceClient.
         */
        public TimeServiceClient timeServiceClient;

        /**
         * stores the a map of TimerSerivce elements objectPath to
         * SignalWrapper. used to extract information about TimerSerivce
         * elements and store information whether the TimerSerivce element is
         * registered to receive a signal .
         */
        private final Map<String, SignalObject> signallHandlerMap = new HashMap<String, SignalObject>();

        public void registerSignalHandler(String objectPath, SignalObject signalObject) {
            signallHandlerMap.put(objectPath, signalObject);
        }

        public SignalObject getSignalObject(String objectPath) {
            return signallHandlerMap.get(objectPath);
        }

        public void unRegisterSignalHandler(String objectPath) {
            signallHandlerMap.remove(objectPath);
        }

        public void clearSignalHandlerList() {
            signallHandlerMap.clear();
        }
    }

    /**
     * stores list of {@link DeviceManagerNotifier} to be notified about changes
     * in {@link DeviceManager#devicesMap}
     */
    private final List<DeviceManagerNotifier> deviceManagerNotifierList = new ArrayList<DeviceManagerNotifier>();

    public synchronized void registerDeviceManagerNotifier(DeviceManagerNotifier pNotifier) {
        if (!deviceManagerNotifierList.contains(pNotifier)) {
            deviceManagerNotifierList.add(pNotifier);
        }
    }

    public synchronized void unRegisterDeviceManagerNotifier(DeviceManagerNotifier pNotifier) {
        if (deviceManagerNotifierList.contains(pNotifier)) {
            deviceManagerNotifierList.remove(pNotifier);
        }
    }

    /**
     * 
     * @param context
     */
    public void init(Context context) {
        this.context = context;
    }

    /**
     * Handles announced call back of AboutListener.
     * 
     * Extract from the About the following
     * <ul>
     * <li>AppID {@link AboutKeys#ABOUT_APP_ID}
     * <li>DeviceID {@link AboutKeys#ABOUT_DEVICE_ID}
     * <li>DeviceName {@link AboutKeys#ABOUT_DEVICE_NAME}
     * </ul>
     * 
     * If AppID exists in {@link DeviceManager#devicesMap} call
     * {@link TimeServiceClient#release()} on the device's timerServiceClient
     * ,replace with new {@link TimeServiceClient}. <br>
     * If AppID doesn't exists in {@link DeviceManager#devicesMap} create new
     * {@link DeviceManager.Device} create new {@link TimeServiceClient} inside
     * the {@link DeviceManager.Device}.
     */
    @Override
    public void announced(final String serviceName, final int version, final short port, final AboutObjectDescription[] objectDescriptions, final Map<String, Variant> serviceMetadata) {
        Log.i(TAG, "onAnnouncement ");
        UUID appId;
        String deviceName;
        String deviceId;
        try {
            Map<String, Object> fromVariantMap = TransportUtil.fromVariantMap(serviceMetadata);
            if (fromVariantMap == null) {
                Log.e(TAG, "onAnnouncement: serviceMetadata map = null !! ignoring.");
                return;
            }
            appId = (UUID) fromVariantMap.get(AboutKeys.ABOUT_APP_ID);
            deviceId = (String) fromVariantMap.get(AboutKeys.ABOUT_DEVICE_ID);
            deviceName = (String) fromVariantMap.get(AboutKeys.ABOUT_DEVICE_NAME);

            BusAttachment busAttachment = ((TimeSampleClient) context.getApplicationContext()).getProtocolManager().getBusAttachment();
            Device device = devicesMap.get(appId);

            if (device == null) {
                device = new Device();
            } else {
                device.timeServiceClient.release();
                device.clearSignalHandlerList();
            }

            device.appId = appId;
            device.deviceId = deviceId;
            device.deviceName = deviceName;
            device.serviceName = serviceName;
            device.lastSeen = System.currentTimeMillis();
            device.timeServiceClient = new TimeServiceClient(busAttachment, serviceName, deviceId, appId, objectDescriptions);
            devicesMap.put(appId, device);

            for (int i = 0; i < deviceManagerNotifierList.size(); i++) {
                deviceManagerNotifierList.get(i).devicesHaveBeenChanged();
            }

            Log.i(TAG, "onAnnouncement: ServiceName = " + serviceName + " port = " + port + " deviceId = " + appId.toString() + " deviceName = " + deviceName);
        } catch (BusException e) {
            Log.e(TAG, "Error in onAnnouncement", e);
        }
    }

    /**
     * Handles onDeviceLost callback.
     * 
     * @param servicename
     *            used to compare with {@link DeviceManager.Device#serviceName}
     *            while traversing {@link DeviceManager#devicesMap} Delete from
     *            {@link DeviceManager#devicesMap} ,notify
     *            {@link DeviceManager#deviceManagerNotifierList}
     */
    public synchronized void onDeviceLost(String servicename) {

        Log.i(TAG, "onDeviceLost = " + servicename);
        Map<UUID, DeviceManager.Device> map = ((TimeSampleClient) context.getApplicationContext()).getDeviceManager().getDevicesMap();
        boolean isfound = false;
        for (Map.Entry<UUID, DeviceManager.Device> entry : map.entrySet()) {
            UUID key = entry.getKey();
            DeviceManager.Device currentDevice = entry.getValue();
            if (currentDevice.serviceName.equals(servicename)) {
                devicesMap.remove(key);
                currentDevice.clearSignalHandlerList();
                currentDevice.timeServiceClient.release();
                isfound = true;
            }
        }

        if (isfound) {
            for (int i = 0; i < deviceManagerNotifierList.size(); i++) {
                deviceManagerNotifierList.get(i).devicesHaveBeenChanged();
            }
        }

    }

    public Map<UUID, Device> getDevicesMap() {
        return devicesMap;
    }
}
