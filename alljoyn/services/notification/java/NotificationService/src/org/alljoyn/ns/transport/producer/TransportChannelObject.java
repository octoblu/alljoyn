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

package org.alljoyn.ns.transport.producer;

import java.util.Map;
import java.util.concurrent.locks.ReentrantLock;

import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.SignalEmitter;
import org.alljoyn.bus.Status;
import org.alljoyn.bus.Variant;
import org.alljoyn.ns.NotificationMessageType;
import org.alljoyn.ns.NotificationServiceException;
import org.alljoyn.ns.commons.GenericLogger;
import org.alljoyn.ns.commons.NativePlatform;
import org.alljoyn.ns.transport.TransportNotificationText;
import org.alljoyn.ns.transport.interfaces.NotificationTransport;

/**
 * Utility class used to store and manage sending notification messages
 */
class TransportChannelObject {
    private static final String TAG = "ioe" + TransportChannelObject.class.getSimpleName();

    /**
     * The object thread lock
     */
    private final ReentrantLock LOCK;

    /**
     * Notification message type
     */
    private final NotificationMessageType messageType;

    /**
     * The object path of the BusObject that is used to send the Notification
     * signals
     */
    private final String servicePath;

    /**
     * Signal emitter that generated the transportChannel
     */
    private SignalEmitter emitter;

    /**
     * NotificationTransport object that is registered on the bus
     */
    private NotificationTransport transportObj;

    /**
     * Reference to Native platform object
     */
    private final NativePlatform nativePlatform;

    /**
     * The serialId of the last sent message
     */
    private volatile Integer lastMsgSerialId;

    /**
     * The notification id of the last sent message
     */
    private volatile Integer lastNotifId;

    /**
     * Constructor
     * 
     * @param messageType
     * @param busAttachment
     * @throws NotificationServiceException
     */
    public TransportChannelObject(NotificationMessageType messageType, BusAttachment busAttachment, NativePlatform nativePlatform) throws NotificationServiceException {
        this.messageType = messageType;
        this.nativePlatform = nativePlatform;
        GenericLogger logger = nativePlatform.getNativeLogger();

        this.LOCK = new ReentrantLock(true);
        this.transportObj = new NotificationTransportProducer();
        this.lastMsgSerialId = null;

        servicePath = NotificationTransportProducer.getServicePath().get(messageType);

        // Perform registerBusObject with a given messageType
        Status status = busAttachment.registerBusObject(transportObj, servicePath);

        if (status != Status.OK) {
            logger.debug(TAG, "Failed to registerBusObject status: '" + status + "'");
            throw new NotificationServiceException("Failed to prepare sending channel");
        }

        // Initializing sessionless signal manager for sending signals later.
        logger.debug(TAG, "Initializing signal emitter for sessionless signal, MessageType: '" + messageType + "'");
        emitter = new SignalEmitter(transportObj, SignalEmitter.GlobalBroadcast.Off);
        emitter.setSessionlessFlag(true);
    }// Constructor

    /**
     * Acquires the lock of the object <br>
     * Very important to call the releaseLock method to release the lock
     */
    public void acquireLock() {
        LOCK.lock();
    }// acquireLock

    /**
     * Releases the lock of the object
     */
    public void releaseLock() {
        LOCK.unlock();
    }// releaseLock

    /**
     * Returns the notification id of the last sent message
     */
    public Integer getNotificationId() {
        return lastNotifId;
    }// getNotificationId

    /**
     * Called when we need to send a signal
     * 
     * @param version
     *            The version of the message signature
     * @param msgId
     *            Notification Id the id of the sent signal
     * @param messageType
     *            Message type id
     * @param deviceId
     *            Device id
     * @param deviceName
     *            Device name
     * @param appId
     *            App id
     * @param appName
     *            App name
     * @param attributes
     *            All the notification metadata
     * @param customAttributes
     *            The customAttributes
     * @param text
     *            Array of texts to be sent
     * @param ttl
     *            Notification message TTL
     * @throws NotificationServiceException
     */
    public void sendNotification(int version, int msgId, short messageType, String deviceId, String deviceName, byte[] appId, String appName, Map<Integer, Variant> attributes,
            Map<String, String> customAttributes, TransportNotificationText[] text, int ttl) throws NotificationServiceException {
        GenericLogger logger = nativePlatform.getNativeLogger();
        logger.debug(TAG, "Sending notification message for messageType: '" + messageType + "' message id: '" + msgId + "', ttl: '" + ttl + "'");
        emitter.setTimeToLive(ttl);
        NotificationTransport transportChannel = emitter.getInterface(NotificationTransport.class);

        LOCK.lock(); // The lock protects the lastMsgSerialId
        try {
            transportChannel.notify(version, msgId, messageType, deviceId, deviceName, appId, appName, attributes, customAttributes, text);
            lastMsgSerialId = emitter.getMessageContext().serial;
            lastNotifId = msgId;
            logger.debug(TAG, "The message was sent successfully. messageType: '" + messageType + "' message id: '" + msgId + "' SerialId: '" + lastMsgSerialId + "'");
        } catch (Exception e) {
            logger.error(TAG, "Failed to call notify to send notification");
            throw new NotificationServiceException("Failed to send notification", e);
        } finally {
            LOCK.unlock();
        }

    }// sendNotification

    /**
     * Called when we need to delete a signal
     */
    public void deleteNotification() {
        GenericLogger logger = nativePlatform.getNativeLogger();

        LOCK.lock(); // The lock protects the lastMsgSerialId
        try {

            if (lastMsgSerialId == null) {
                logger.warn(TAG, "Unable to delete last message for  messageType: '" + messageType + "'. No message was previously sent from this object, lastMsgSerialId is NULL");
                return;
            }

            logger.debug(TAG, "Deleting last notification message for messageType: '" + messageType + "', MsgSerialId: '" + lastMsgSerialId + "'");

            Status status = emitter.cancelSessionlessSignal(lastMsgSerialId);
            if (status == Status.OK) {
                logger.debug(TAG, "The notification message deleted successfully, messageType: '" + messageType + "', MsgSerialId: '" + lastMsgSerialId + "', lastNotifId: '" + lastNotifId + "'");
                lastMsgSerialId = null;
                lastNotifId = null;
            } else {
                logger.warn(TAG, "Failed to delete last message for  messageType: '" + messageType + "'. Status: '" + status + "'");
            }
        } finally {
            LOCK.unlock();
        }
    }// deleteNotification

    /**
     * Unregister bus object from AJ bus, Set to null emitter, transportChannel,
     * transportObj
     */
    public void clean(BusAttachment busAttachment) {
        deleteNotification();
        busAttachment.unregisterBusObject(transportObj);

        transportObj = null;
        emitter = null;
        lastMsgSerialId = null;
        lastNotifId = null;
    }// clean

}
