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


#ifndef TIMESERVICEALARMBUSOBJ_H_
#define TIMESERVICEALARMBUSOBJ_H_

#include <alljoyn/BusAttachment.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/time/TimeServiceServerAlarm.h>
#include <alljoyn/Status.h>
#include <vector>

namespace ajn {
namespace services {

/**
 * This is the base class for all the classes implementing Alarm functionality.
 * Extend this class to handle AllJoyn communication with this Alarm.
 * Alarm related calls are delegated to the TimeServiceServerAlarm.
 */
class TimeServiceAlarmBusObj : public ajn::BusObject {

    /**
     * This TimeServiceAlarmBusObj object is a Friend of the TimeServiceAlarmFactory
     */
    friend class TimeServiceAlarmFactoryBusObj;

  public:

    /**
     * Constructor
     *
     * @param objectPath
     */
    TimeServiceAlarmBusObj(qcc::String const& objectPath);

    /**
     * Destructor
     */
    virtual ~TimeServiceAlarmBusObj();

    /**
     * Initialize the Bus Object. Register it on the BusAttachment and in the AboutService for Announcement
     *
     * @param alarm Events delegate. This alarm receives all the Alarm related events.
     * @param notAnnounced Interfaces that shouldn't be announced. If the vector is empty, all the interfaces added
     * to TimeServiceAlarmBusObj will be announced.
     *
     * @return QStatus of the Alarm initialization
     */
    virtual QStatus init(TimeServiceServerAlarm* alarm, const std::vector<qcc::String>& notAnnounced);

    /**
     * Initialize the Bus Object with object description.
     * Register it on the BusAttachment and in the AboutService for Announcement
     *
     * @param alarm Events delegate. This alarm receives all the Alarm related events.
     * @param notAnnounced Interfaces that shouldn't be announced. If the vector is empty, all the interfaces added
     * to TimeServiceAlarmBusObj will be announced.
     * @param description Events&Actions description
     * @param language Events&Actions description language
     * @param translator Events&Actions Translator
     *
     * @return QStatus of the Alarm initialization
     */
    virtual QStatus init(TimeServiceServerAlarm* alarm, const std::vector<qcc::String>& notAnnounced, const qcc::String& description,
                         const qcc::String& language, Translator* translator);

    /**
     * Releases object resources
     */
    void release();

    /**
     * Returns object path of this Alarm
     *
     * @return Returns object path of this Alarm
     */
    qcc::String const& getObjectPath() const;

    /**
     * Call this method to emit the signal, when the Alarm has been reached
     *
     * @return status
     */
    QStatus sendAlarmReached();

    /**
     * Whether the object has been initialize previously.
     *
     * @return TRUE if the object has been initialized previously by the call to the init() method
     */
    bool isInitialized() const;

  protected:

    /**
     * This hook method is called to allow creating any custom interface in addition to the standard Alarm interface.
     * This method is called by the init() method.
     * In order to create custom interface for the Alarm, override this method and create additional interface for the Alarm.
     * When the custom interface is created, call addAlarmInterface() method with the InterfaceDescription of the
     * created custom interface.
     *
     * @param bus BusAttachment
     * @return status ER_OK if succeeded in creating custom alarm
     */
    virtual QStatus createCustomInterfaceHook(BusAttachment* bus);

    /**
     * Adds the interface to this Alarm BusObject.
     * This method must be called in order to add the interface identified by the given InterfaceDescription to this
     * Alarm BusObject. Additionally this method registers the interface to be sent in the Announcement signal.
     *
     * @param iface InterfaceDescription
     * @return status Status of adding the interface
     */
    QStatus addAlarmInterface(const InterfaceDescription& iface);

    /**
     * Callback for Alljoyn when GetProperty is called on this BusObject.
     * If custom Alarm that contains properties is created, override this method and call it from
     * the child class, to handle Get property calls on the standard Alarm interface.
     * If calling Get on the parent class returns status of ER_BUS_UNKNOWN_INTERFACE, it means that
     * the property doesn't belong to the standard Alarm interface and should be handled by the custom Alarm.
     *
     * @param interfaceName - the name of the Interface
     * @param propName - the name of the Property
     * @param msgArg - Out variable. The MsgArg to fill
     * @return status - success/failure
     */
    virtual QStatus Get(const char* interfaceName, const char* propName, MsgArg& msgArg);

    /**
     * Callback for Alljoyn when SetProperty is called on this BusObject.
     * If custom Alarm that contains properties is created, override this method and call it from
     * the child class, to handle Set property calls on the standard Alarm interface.
     * If calling Set on the parent class returns status of ER_BUS_UNKNOWN_INTERFACE, it means that
     * the property doesn't belong to the standard Alarm interface and should be handled by the custom Alarm.
     *
     * @param interfaceName - the name of the Interface
     * @param propName - the name of the Property
     * @param msgArg - the MsgArg that contains the new Value
     * @return status - success/failure
     */
    virtual QStatus Set(const char* interfaceName, const char* propName, MsgArg& msgArg);

  private:

    /**
     * Alarm events delegate
     */
    TimeServiceServerAlarm* m_Alarm;

    /**
     * Object path of this Clock object
     */
    qcc::String m_ObjectPath;

    /**
     * The pointer is used to send AlarmReached signals
     */
    const ajn::InterfaceDescription::Member* m_AlarmReachedSignalMethod;

    /**
     * TRUE if the object has been initialized by the call to the init() method
     */
    bool m_IsInitialized;

    /**
     * TRUE if the Alarm should be announced
     */
    bool m_IsAnnounced;

    /**
     * Initialize the Bus Object. Register it on the BusAttachment and in the AboutService for Announcement
     *
     * @param alarm Events delegate. This alarm receives all the Alarm related events.
     * to TimeServiceAlarmBusObj will be announced.
     * @param description Events&Actions description
     * @param language Events&Actions description language
     * @param translator Events&Actions Translator
     * @return QStatus of the Alarm initialization
     */
    QStatus init(TimeServiceServerAlarm* alarm, const qcc::String& description, const qcc::String& language, Translator* translator);

    /**
     * Creates Alarm interface if it hasn't been created yet and adds it to the BusObject.
     *
     * @param bus
     * @return status
     */
    QStatus createAlarmInterface(BusAttachment* bus);

    /**
     * Handle Get Version request
     */
    QStatus handleGetVersion(MsgArg& msgArg);

    /**
     * Handle Get Schedule request
     */
    QStatus handleGetSchedule(MsgArg& msgArg);

    /**
     * Handle Get Title request
     */
    QStatus handleGetTitle(MsgArg& msgArg);

    /**
     * Handle Get Enabled request
     */
    QStatus handleGetEnabled(MsgArg& msgArg);

    /**
     * Handle Set Schedule request
     */
    QStatus handleSetSchedule(MsgArg& msgArg);

    /**
     * Handle Set Title request
     */
    QStatus handleSetTitle(MsgArg& msgArg);

    /**
     * Handle Set Enabled request
     */
    QStatus handleSetEnabled(MsgArg& msgArg);
};

} /* namespace services */
} /* namespace ajn */

#endif /* TIMESERVICEALARMBUSOBJ_H_ */
