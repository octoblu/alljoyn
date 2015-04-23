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

#ifndef TIMESERVICESERVER_H_
#define TIMESERVICESERVER_H_

#include <vector>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/Status.h>
#include <alljoyn/BusListener.h>
#include <alljoyn/SessionPortListener.h>
#include <alljoyn/time/TimeServiceServerClock.h>
#include <alljoyn/time/TimeServiceServerAuthorityClock.h>
#include <alljoyn/time/TimeServiceServerAlarm.h>
#include <alljoyn/time/TimeServiceServerAlarmFactory.h>
#include <alljoyn/time/TimeServiceServerTimer.h>
#include <alljoyn/time/TimeServiceServerTimerFactory.h>

namespace ajn {
namespace services {

/**
 * The main class to initialize the Time Service in the server mode.
 * Initialize the class by calling the TimeServiceServer.init() method
 */
class TimeServiceServer : public SessionListener, public SessionPortListener {

  public:

    /**
     * Get instance of TimeServiceServer singleton
     *
     * @return TimeServiceServer instance
     */
    static TimeServiceServer* getInstance();

    /**
     * Initialize the TimeServiceServer
     *
     * @param busAttachment to be used by the TimeServiceServer
     *
     * @return ER_OK if TimeServiceServer initialization succeeded
     */
    QStatus init(BusAttachment* busAttachment);

    /**
     * Stop the TimeServiceServer
     */
    void shutdown();

    /**
     * Returns BusAttachment that is used by the TimeServiceServer.
     *
     * @return BusAttachment or NULL if the BusAttachment is not initialized.
     */
    BusAttachment* getBusAttachment() const;

    /**
     * TimeServiceServer is started if the method TimeServiceServer.init()
     * has been called.
     *
     * @return TRUE if the TimeServiceServer has been started
     */
    bool isStarted() const;

    /**
     * Destructor
     */
    ~TimeServiceServer();

    /**
     * Activate Clock.
     * @param clock Methods of this Clock are invoked as a response to calls of the remote TimeServiceClient
     *
     * @return status of creating the Clock
     */
    QStatus activateClock(TimeServiceServerClock* clock) const;

    /**
     * Activate Time Authority Clock.
     * The clock allows to send TimeServiceServerAuthorityClock.timeSync() signal to suggest synchronizing
     * time with it.
     *
     * @param clock Methods of this Clock are invoked as a response to calls of the remote TimeServiceClient
     * @param type Options where this Time AuthorityClock synchronizes its time
     *
     * @return status of creating the TimeAuthority clock
     */
    QStatus activateTimeAuthorityClock(TimeServiceServerAuthorityClock* clock,
                                       tsConsts::ClockAuthorityType type) const;

    /**
     * Activate Alarm.
     *
     * @param alarm Methods of this Alarm are invoked
     * as a response to calls of the remote TimeServiceClient
     *
     * @return status of creating the alarm
     */
    QStatus activateAlarm(TimeServiceServerAlarm* alarm) const;

    /**
     * Activate Alarm with description.
     *
     * @param alarm Methods of this TimeServiceServerAlarm are invoked
     * as a response to calls of the remote TimeServiceClient
     * @param description Description to be added to the object when registering TimeServiceServerAlarm.
     * Since description is provided, org.allseen.Introspectable interface is added to the announced interfaces
     * to support Events & Actions feature.
     * @param language The language of the description
     * @param translator If not NULL, TimeServiceServerAlarm is registered with Translator
     * to support the feature of AllSeenIntrospectable.GetDescriptionLanguages()
     *
     * @return status of creating the alarm
     */
    QStatus activateAlarm(TimeServiceServerAlarm* alarm, const qcc::String& description,
                          const qcc::String& language, Translator* translator) const;

    /**
     * Register user defined custom TimeServiceServerAlarm.
     * To register custom Alarm inherit from the TimeServiceAlarmBusObj.
     * This method registers the given alarmBusObj and searches for the interfaces to be added to AboutService
     * and later announced.
     * To exclude interfaces from being announced, notAnnounced vector should be provided.
     * @param alarmBusObj Custom Alarm to be registered
     * @param alarm Methods of this Alarm are invoked
     * as a response to calls of the remote TimeServiceClient
     * @param notAnnounced AllJoyn name of the interfaces that should NOT be announced. If this vector is empty,
     * all the found interfaces are added to the AboutService to be announced.
     *
     * @return status of creating the alarm
     */
    QStatus registerCustomAlarm(TimeServiceAlarmBusObj* alarmBusObj, TimeServiceServerAlarm* alarm,
                                const std::vector<qcc::String>& notAnnounced) const;

    /**
     * Register user defined custom TimeServiceServerAlarm.
     * To register custom Alarm inherit from the TimeServiceAlarmBusObj.
     * This method registers the given alarmBusObj and searches for the interfaces to be added to AboutService
     * and later announced.
     * To exclude interfaces from being announced, notAnnounced vector should be provided.
     * @param alarmBusObj Custom Alarm to be registered
     * @param alarm Methods of this TimeServiceServerAlarm are invoked
     * as a response to calls of the remote TimeServiceClient
     * @param notAnnounced AllJoyn name of the interfaces that should NOT be announced. If this vector is empty,
     * all the found interfaces are added to the AboutService to be announced.
     * @param description Description to be added to the TimeServiceAlarmBusObject
     * Since description is provided, org.allseen.Introspectable interface is added to the announced interfaces
     * to support Events & Actions feature.
     * @param language The language of the description
     * @param translator If not NULL, TimeServiceAlarmBusObj is registered with Translator
     * to support the feature of AllSeenIntrospectable.GetDescriptionLanguages()
     *
     * @return status of creating the alarm
     */
    QStatus registerCustomAlarm(TimeServiceAlarmBusObj* alarmBusObj, TimeServiceServerAlarm* alarm,
                                const std::vector<qcc::String>& notAnnounced, const qcc::String& description,
                                const qcc::String& language, Translator* translator) const;

    /**
     * Activate AlarmFactory.
     *
     * @param alarmFactory Methods of this TimeServiceServerAlarmFactory are invoked
     * as a response to calls of the remote TimeServiceClient
     *
     * @return status of creating the AlarmFactory
     */
    QStatus activateAlarmFactory(TimeServiceServerAlarmFactory* alarmFactory) const;

    /**
     * Activate AlarmFactory with description.
     *
     * @param alarmFactory Methods of this TimeServiceServerAlarmFactory are invoked
     * as a response to calls of the remote TimeServiceClient
     * @param description Description to be added to the object when TimeServiceServerAlarm is created
     * by the AlarmFactory.
     * Since description is provided, org.allseen.Introspectable interface is added to the announced interfaces
     * to support Events & Actions feature.
     * @param language The language of the description
     * @param translator If not NULL, Created TimeServiceServerAlarm is registered with Translator
     * to support the feature of AllSeenIntrospectable.GetDescriptionLanguages()
     *
     * @return status of creating the alarm
     */
    QStatus activateAlarmFactory(TimeServiceServerAlarmFactory* alarmFactory, const qcc::String& description,
                                 const qcc::String& language, Translator* translator) const;


    /**
     * Activate Timer.
     *
     * @param Timer Methods of this Timer are invoked
     * as a response to calls of the remote TimeServiceClient
     *
     * @return status of creating the Timer
     */
    QStatus activateTimer(TimeServiceServerTimer* Timer) const;

    /**
     * Activate Timer with description.
     *
     * @param Timer Methods of this TimeServiceServerTimer are invoked
     * as a response to calls of the remote TimeServiceClient
     * @param description Description to be added to the object when registering TimeServiceServerTimer.
     * Since description is provided, org.allseen.Introspectable interface is added to the announced interfaces
     * to support Events & Actions feature.
     * @param language The language of the description
     * @param translator If not NULL, TimeServiceServerTimer is registered with Translator
     * to support the feature of AllSeenIntrospectable.GetDescriptionLanguages()
     *
     * @return status of creating the Timer
     */
    QStatus activateTimer(TimeServiceServerTimer* Timer, const qcc::String& description,
                          const qcc::String& language, Translator* translator) const;

    /**
     * Register user defined custom TimeServiceServerTimer.
     * To register custom Timer inherit from the TimeServiceTimerBusObj.
     * This method registers the given TimerBusObj and searches for the interfaces to be added to AboutService
     * and later announced.
     * To exclude interfaces from being announced, notAnnounced vector should be provided.
     * @param TimerBusObj Custom Timer to be registered
     * @param Timer Methods of this Timer are invoked
     * as a response to calls of the remote TimeServiceClient
     * @param notAnnounced AllJoyn name of the interfaces that should NOT be announced. If this vector is empty,
     * all the found interfaces are added to the AboutService to be announced.
     *
     * @return status of creating the Timer
     */
    QStatus registerCustomTimer(TimeServiceTimerBusObj* TimerBusObj, TimeServiceServerTimer* Timer,
                                const std::vector<qcc::String>& notAnnounced) const;

    /**
     * Register user defined custom TimeServiceServerTimer.
     * To register custom Timer inherit from the TimeServiceTimerBusObj.
     * This method registers the given TimerBusObj and searches for the interfaces to be added to AboutService
     * and later announced.
     * To exclude interfaces from being announced, notAnnounced vector should be provided.
     * @param TimerBusObj Custom Timer to be registered
     * @param Timer Methods of this TimeServiceServerTimer are invoked
     * as a response to calls of the remote TimeServiceClient
     * @param notAnnounced AllJoyn name of the interfaces that should NOT be announced. If this vector is empty,
     * all the found interfaces are added to the AboutService to be announced.
     * @param description Description to be added to the TimeServiceTimerBusObject
     * Since description is provided, org.allseen.Introspectable interface is added to the announced interfaces
     * to support Events & Actions feature.
     * @param language The language of the description
     * @param translator If not NULL, TimeServiceTimerBusObj is registered with Translator
     * to support the feature of AllSeenIntrospectable.GetDescriptionLanguages()
     *
     * @return status of creating the Timer
     */
    QStatus registerCustomTimer(TimeServiceTimerBusObj* TimerBusObj, TimeServiceServerTimer* Timer,
                                const std::vector<qcc::String>& notAnnounced, const qcc::String& description,
                                const qcc::String& language, Translator* translator) const;

    /**
     * Activate TimerFactory.
     *
     * @param timerFactory Methods of this TimeServiceServerTimerFactory are invoked
     * as a response to calls of the remote TimeServiceClient(s)
     *
     * @return status of creating the TimerFactory
     */
    QStatus activateTimerFactory(TimeServiceServerTimerFactory* timerFactory) const;

    /**
     * Activate TimerFactory with description.
     *
     * @param TimerFactory Methods of this TimeServiceServerTimerFactory are invoked
     * as a response to calls of the remote TimeServiceClient
     * @param description Description to be added to the object when TimeServiceServerTimer is created
     * by the TimerFactory.
     * Since description is provided, org.allseen.Introspectable interface is added to the announced interfaces
     * to support Events & Actions feature.
     * @param language The language of the description
     * @param translator If not NULL, Created TimeServiceServerTimer is registered with Translator
     * to support the feature of AllSeenIntrospectable.GetDescriptionLanguages()
     *
     * @return status of creating the Timer
     */
    QStatus activateTimerFactory(TimeServiceServerTimerFactory* TimerFactory, const qcc::String& description,
                                 const qcc::String& language, Translator* translator) const;

    /**
     * Generate an object path.
     * Call this method to generate object path that is required for initialization of the
     * TimeServiceAlarmBusObj and TimeServiceTimerBusObj. These bus objects are required for
     * registerCustomAlarm(), or registerCustomTimer() methods.
     *
     * @param objectPath In Out variable. Prefix of the object path that is generate by this method.
     *
     * @return ER_OK if the objectPath was created successfully.
     * The given objectPath is modified to be the full object path.
     */
    QStatus generateObjectPath(qcc::String* objectPath) const;

    /**
     * Get the session id, if there exists one.
     * The server uses multipoint session, so only one session can be established with this TimeServiceServer.
     *
     * @return Session Id or 0 if no session is established
     */
    SessionId getSessionId();

  private:

    /**
     * Instance variable - TimeServiceServer is a singleton
     */
    static TimeServiceServer* s_Instance;

    /**
     * Session connected to the TimeServiceServer
     */
    SessionId m_Sid;

    /**
     * Bus Attachment that is used by the Time Service Server
     */
    BusAttachment* m_Bus;

    /**
     * Constructor
     */
    TimeServiceServer();

    /**
     * Callback for when the session is lost
     *
     * @param sessionId
     * @param reason for session lost
     */
    void SessionLost(ajn::SessionId sessionId, SessionLostReason reason);

    /**
     * AcceptSessionJoiner - Receive request to join session and decide whether to accept it or not
     *
     * @param sessionPort - the port of the request
     * @param joiner - the name of the joiner
     * @param opts - the session options
     * @return true/false
     */
    bool AcceptSessionJoiner(ajn::SessionPort sessionPort, const char* joiner, const ajn::SessionOpts& opts);

    /**
     * Callback when the session is joined
     *
     * @param sessionPort - port of session
     * @param id - sessionId of session
     * @param joiner - name of joiner
     */
    void SessionJoined(ajn::SessionPort sessionPort, ajn::SessionId id, const char* joiner);
};

} /* namespace services */
} /* namespace ajn */

#endif /* TIMESERVICESERVER_H_ */
