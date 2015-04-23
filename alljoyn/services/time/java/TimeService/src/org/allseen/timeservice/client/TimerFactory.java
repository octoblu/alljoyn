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
 * Time Service TimerFactory.
 * Use this class to communication with {@link TimeServiceServer} TimerFactory object.
 */
public class TimerFactory extends ObjectIntrospector {
    private static final String TAG = "ajts" + TimerFactory.class.getSimpleName();

    /**
     * Constructor
     * @param tsClient {@link TimeServiceClient} managing this {@link TimerFactory} object
     * @param objectPath {@link TimeServiceServer} TimerFactory object
     */
    public TimerFactory(TimeServiceClient tsClient, String objectPath) {

        super(tsClient, objectPath);
    }

    /**
     * Retrieve {@link Version} from the {@link TimeServiceServer} TimerFactory object.
     * @return {@link Version}
     * @throws TimeServiceException Is thrown if failed to retrieve the {@link Version}
     */
    public short retrieveVersion() throws TimeServiceException {

        Log.d(TAG, "Retrieving Version, objPath: '" + objectPath + "'");

        try {

            short verAJ = getRemoteTimerFactory().getVersion();
            
            Log.d(TAG, "Retrieved Version: '" + verAJ + "', objPath: '" + objectPath + "'");

            return verAJ;
        }
        catch (Exception e) {

            throw new TimeServiceException("Failed to call TimerFactory.retrieveVersion()", e);
        }
    }

    /**
     * Creates new {@link Timer}
     * @return {@link Timer} that has been created
     * @throws TimeServiceException If failed to create the {@link Timer}
     */
    public Timer newTimer() throws TimeServiceException {

        Log.d(TAG, "Creating NewTimer, objPath: '" + objectPath + "'");

        String objectPath;

        try {

            objectPath = getRemoteTimerFactory().newTimer();
        } catch (BusException be) {

            throw new TimeServiceException("Failed to create NewTimer", be);
        }

        Timer timer = new Timer(tsClient, objectPath);
        Log.d(TAG, "Created NewTimer: '" + timer + "', objPath: '" + objectPath + "'");

        return timer;
    }

    /**
     * Delete {@link Timer} identified by the given object path.
     * Retrieve the object path by calling {@link Timer#getObjectPath()}.
     * @param objectPath Object path of the {@link Timer} to delete.
     * @throws TimeServiceException Is thrown if failed to delete the {@link Timer}
     */
    public void deleteTimer(String objectPath) throws TimeServiceException {

        Log.d(TAG, "Deleting Timer: '" + objectPath + "', objPath: '" + objectPath + "'");

        try {

            getRemoteTimerFactory().deleteTimer(objectPath);
        }
        catch (BusException be) {

            throw new TimeServiceException("Failed to delete the Timer: '" + objectPath + "'", be);
        }
    }

    /**
     * Retrieves a {@link List} of {@link Timer}s managed by this {@link TimerFactory}
     * @return List of {@link Timer}s
     * @throws TimeServiceException Is thrown if failed to retrieve the list of {@link Timer}s
     */
    public List<Timer> retrieveTimerList() throws TimeServiceException {

        Log.d(TAG, "Retrieving List of Timers, objPath: '" + objectPath + "'");

        IntrospectionNode introspector;
        try {

            introspector = introspect();
        } catch (Exception e) {

            throw new TimeServiceException("Failed to retrieve List of Timers", e);
        }

        List<IntrospectionNode> children = introspector.getChidren();
        List<Timer> timers               = new ArrayList<Timer>(children.size());

        //Assumption: that all the objects under the Timer Factor implement Timer interface
        for (IntrospectionNode child : children) {

            timers.add( new Timer(tsClient, child.getPath()) );
        }

        Log.d(TAG, "Returning List of Timers: {" + timers + "}, objPath: '" + objectPath + "'");
        return timers;
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
     * {@link org.allseen.timeservice.ajinterfaces.TimerFactory}
     * @return {@link org.allseen.timeservice.ajinterfaces.TimerFactory}
     * @throws TimeServiceException Is thrown if failed to create the {@link ProxyBusObject}
     * @see TimeClientBase#getProxyObject(Class[])
     */
    private org.allseen.timeservice.ajinterfaces.TimerFactory getRemoteTimerFactory() throws TimeServiceException {

        ProxyBusObject proxy = super.getProxyObject(new Class<?>[]{org.allseen.timeservice.ajinterfaces.TimerFactory.class});
        return proxy.getInterface(org.allseen.timeservice.ajinterfaces.TimerFactory.class);
    }

    /**
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {

        return "[TimerFactory - objPath: '" + objectPath + "']";
    }

}
