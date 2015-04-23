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

package org.allseen.timeservice.client;

import java.util.ArrayList;
import java.util.List;

import org.alljoyn.bus.BusException;
import org.alljoyn.bus.ProxyBusObject;
import org.allseen.timeservice.TimeServiceException;
import org.allseen.timeservice.server.TimeServiceServer;

import android.util.Log;

/**
 * Time Service AlarmFactory.
 * Use this class to communication with {@link TimeServiceServer} AlarmFactory object.
 */
public class AlarmFactory extends ObjectIntrospector {
    private static final String TAG = "ajts" + AlarmFactory.class.getSimpleName();

    /**
     * Constructor
     * @param tsClient {@link TimeServiceClient} managing this {@link AlarmFactory} object
     * @param objectPath {@link TimeServiceServer} AlarmFactory object
     */
    public AlarmFactory(TimeServiceClient tsClient, String objectPath) {

        super(tsClient, objectPath);
    }

    /**
     * Retrieve {@link Version} from the {@link TimeServiceServer} AlarmFactory object.
     * @return {@link Version}
     * @throws TimeServiceException Is thrown if failed to retrieve the {@link Version}
     */
    public short retrieveVersion() throws TimeServiceException {

        Log.d(TAG, "Retrieving Version, objPath: '" + objectPath + "'");

        try {

            short verAJ = getRemoteAlarmFactory().getVersion();
            
            Log.d(TAG, "Retrieved Version: '" + verAJ + "', objPath: '" + objectPath + "'");

            return verAJ;
        }
        catch (Exception e) {

            throw new TimeServiceException("Failed to call AlarmFactory.retrieveVersion()", e);
        }
    }

    /**
     * Creates new {@link Alarm}
     * @return {@link Alarm} that has been created
     * @throws TimeServiceException Is thrown if failed to create the {@link Alarm}
     */
    public Alarm newAlarm() throws TimeServiceException {

        Log.d(TAG, "Creating NewAlarm, objPath: '" + objectPath + "'");

        String objectPath;

        try {

            objectPath = getRemoteAlarmFactory().newAlarm();
        } catch (BusException be) {

            throw new TimeServiceException("Failed to create NewAlarm", be);
        }

        Alarm alarm = new Alarm(tsClient, objectPath);
        Log.d(TAG, "Created NewAlarm: '" + alarm + "', objPath: '" + objectPath + "'");

        return alarm;
    }

    /**
     * Delete {@link Alarm} identified by the given object path.
     * Retrieve the object path by calling {@link Alarm#getObjectPath()}.
     * @param objectPath Object path of the {@link Alarm} to delete.
     * @throws TimeServiceException Is thrown if failed to delete the {@link Alarm}
     */
    public void deleteAlarm(String objectPath) throws TimeServiceException {

        Log.d(TAG, "Deleting Alarm: '" + objectPath + "', objPath: '" + objectPath + "'");

        try {

            getRemoteAlarmFactory().deleteAlarm(objectPath);
        }
        catch (BusException be) {

            throw new TimeServiceException("Failed to delete the Alarm: '" + objectPath + "'", be);
        }
    }

    /**
     * Retrieves a {@link List} of {@link Alarm}s managed by this {@link AlarmFactory}
     * @return List of {@link Alarm}s
     * @throws TimeServiceException Is thrown if failed to retrieve the list of {@link Alarm}s
     */
    public List<Alarm> retrieveAlarmList() throws TimeServiceException {

        Log.d(TAG, "Retrieving List of Alarms, objPath: '" + objectPath + "'");

        IntrospectionNode introspector;
        try {

            introspector = introspect();
        } catch (Exception e) {

            throw new TimeServiceException("Failed to retrieve List of Alarms", e);
        }

        List<IntrospectionNode> children = introspector.getChidren();
        List<Alarm> alarms               = new ArrayList<Alarm>(children.size());

        //Assumption: that all the objects under the Alarm Factory implement Alarm interface
        for (IntrospectionNode child : children) {

            alarms.add( new Alarm(tsClient, child.getPath()) );
        }

        Log.d(TAG, "Returning List of Alarms: {" + alarms + "}, objPath: '" + objectPath + "'");
        return alarms;
    }

    /**
     * @see org.allseen.timeservice.client.TimeClientBase#release()
     */
    @Override
    public void release() {

        super.release();
    }

    /**
     * Create and return {@link ProxyBusObject} casted to the
     * {@link org.allseen.timeservice.ajinterfaces.AlarmFactory}
     * @return {@link org.allseen.timeservice.ajinterfaces.AlarmFactory}
     * @throws TimeServiceException Is thrown if failed to create the {@link ProxyBusObject}
     * @see TimeClientBase#getProxyObject(Class[])
     */
    private org.allseen.timeservice.ajinterfaces.AlarmFactory getRemoteAlarmFactory() throws TimeServiceException {

        ProxyBusObject proxy = super.getProxyObject(new Class<?>[]{org.allseen.timeservice.ajinterfaces.AlarmFactory.class});
        return proxy.getInterface(org.allseen.timeservice.ajinterfaces.AlarmFactory.class);
    }

    /**
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {

        return "[AlarmFactory - objPath: '" + objectPath + "']";
    }

}
