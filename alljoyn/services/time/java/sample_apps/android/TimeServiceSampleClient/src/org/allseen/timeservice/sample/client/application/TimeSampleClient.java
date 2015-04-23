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

import java.util.LinkedList;
import java.util.List;

import org.allseen.timeservice.sample.client.dataobjects.SignalMessageItem;
import org.allseen.timeservice.sample.client.ui.MainActivity;

import android.app.Application;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;

/**
 * Loads the alljoyn_java library upon application start. <Br>
 * Listens to internal broadcast messages sent by
 * <ul>
 * <li><{@link DeviceManager.TimeAuthorityHandlerImpl}
 * <li><{@link DeviceManager.TimerHandlerImpl}
 * <li><{@link DeviceManager.AlarmHandlerImpl}
 * </ul>
 * Stores the messages and updates the UI
 */
public class TimeSampleClient extends Application {
    private static final String TAG = "TimeServiceSampleClientApplication";

    public static final String TIMER_EVENT_SIGNAL_ACTION = "TimerEvent";
    public static final String ALARM_EVENT_SIGNAL_ACTION = "AlarmEvent";
    public static final String TIME_AUTHORITY_EVENT_SIGNAL_ACTION = "TimeAuthorityEvent";

    public static final String NEW_SIGNAL_MESSAGE_ACTION = "SignalMessageAction";
    public static final String CLEAR_SIGNAL_MESSAGE_ACTION = "ClearSignalMessageAction";
    public static final String NEW_SIGNAL_COUNT_KEY = "NewSignalCount";

    public static final String MESSAGE_KEY = "message";
    public static final String OBJECT_PATH_KEY = "objectpath";
    public static final String SERVICE_NAME_KEY = "serivcename";

    private final ProtocolManager protocolManager = new ProtocolManager();
    private final DeviceManager deviceManager = new DeviceManager();
    static {
        try {
            System.loadLibrary("alljoyn_java");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private final BroadcastReceiver signalMessagesReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            synchronized (TAG) {
                Bundle bundle = intent.getExtras();
                if (bundle != null) {
                    String message = bundle.getString(MESSAGE_KEY);
                    String object_path = bundle.getString(OBJECT_PATH_KEY);
                    String servicename = bundle.getString(SERVICE_NAME_KEY);
                    Log.d(TAG, "received from [" + object_path + "] " + message);
                    SignalMessageItem item = new SignalMessageItem();
                    item.message = message;
                    item.objectpath = object_path;
                    item.servicename = servicename;
                    item.timestamp = System.currentTimeMillis();
                    int count = 0;
                    for (SignalMessageItem signalMessageItem : signalMessages) {
                        if (!signalMessageItem.seen) {
                            count++;
                        }
                    }
                    ((LinkedList<SignalMessageItem>) signalMessages).addFirst(item);

                    Intent sendIntent = new Intent(context, MainActivity.class);
                    sendIntent.setAction(NEW_SIGNAL_MESSAGE_ACTION);
                    sendIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                    sendIntent.putExtra(NEW_SIGNAL_COUNT_KEY, Integer.valueOf(count + 1));
                    context.startActivity(sendIntent);

                }
            }
        }
    };

    @Override
    public void onCreate() {
        super.onCreate();
        protocolManager.init(this, deviceManager);
        deviceManager.init(this);
        IntentFilter filter = new IntentFilter();
        filter.addAction(TIMER_EVENT_SIGNAL_ACTION);
        filter.addAction(ALARM_EVENT_SIGNAL_ACTION);
        filter.addAction(TIME_AUTHORITY_EVENT_SIGNAL_ACTION);

        LocalBroadcastManager.getInstance(this).registerReceiver(signalMessagesReceiver, filter);
    }

    public ProtocolManager getProtocolManager() {
        return protocolManager;
    }

    public DeviceManager getDeviceManager() {
        return deviceManager;
    }

    private final List<SignalMessageItem> signalMessages = new LinkedList<SignalMessageItem>();

    public List<SignalMessageItem> getSignalMessages() {
        synchronized (TAG) {
            return signalMessages;
        }

    }

}
