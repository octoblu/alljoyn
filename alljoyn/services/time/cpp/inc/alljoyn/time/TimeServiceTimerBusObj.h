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


#ifndef TIMESERVICETIMERBUSOBJ_H_
#define TIMESERVICETIMERBUSOBJ_H_

#include <alljoyn/BusAttachment.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/time/TimeServiceServerTimer.h>
#include <alljoyn/Status.h>
#include <vector>

namespace ajn {
namespace services {

/**
 * This is the base class for all the classes implementing Timer functionality.
 * Extend this class to handle AllJoyn communication with this Timer.
 * Timer related calls are delegated to the TimeServiceServerTimer.
 */
class TimeServiceTimerBusObj : public ajn::BusObject {

    /**
     * This TimeServiceTimerBusObj object is a Friend of the TimeServiceTimerFactory
     */
    friend class TimeServiceTimerFactoryBusObj;

  public:

    /**
     * Constructor
     *
     * @param objectPath
     */
    TimeServiceTimerBusObj(qcc::String const& objectPath);

    /**
     * Destructor
     */
    virtual ~TimeServiceTimerBusObj();

    /**
     * Initialize the Bus Object. Register it on the BusAttachment and in the AboutService for Announcement
     *
     * @param Timer Events delegate. This Timer receives all the Timer related events.
     * @param notAnnounced Interfaces that shouldn't be announced. If the vector is empty, all the interfaces added
     * to TimeServiceTimerBusObj will be announced.
     *
     * @return QStatus of the Timer initialization
     */
    virtual QStatus init(TimeServiceServerTimer* Timer, const std::vector<qcc::String>& notAnnounced);

    /**
     * Initialize the Bus Object with object description.
     * Register it on the BusAttachment.
     *
     * @param Timer Events delegate. This Timer receives all the Timer related events.
     * @param notAnnounced Interfaces that shouldn't be announced. If the vector is empty, all the interfaces added
     * to TimeServiceTimerBusObj will be announced.
     * @param description Events&Actions description
     * @param language Events&Actions description language
     * @param translator Events&Actions Translator
     *
     * @return QStatus of the Timer initialization
     */
    virtual QStatus init(TimeServiceServerTimer* Timer, const std::vector<qcc::String>& notAnnounced, const qcc::String& description,
                         const qcc::String& language, Translator* translator);

    /**
     * Releases object resources
     */
    void release();

    /**
     * Returns object path of this Timer
     *
     * @return Returns object path of this Timer
     */
    qcc::String const& getObjectPath() const;

    /**
     * Call this method to emit the timer event signal
     *
     * @return status
     */
    QStatus sendTimerEvent();

    /**
     * Call this method to emit the run state changed signal
     * @param isRunning TRUE if the timer is running, otherwise FALSE
     *
     * @return status
     */
    QStatus sendRunStateChanged(bool isRunning);

    /**
     * Whether the object has been initialize previously.
     *
     * @return TRUE if the object has been initialized by the call to the init() method
     */
    bool isInitialized() const;

  protected:

    /**
     * This hook method is called to allow creating any custom interface in addition to the standard Timer interface.
     * This method is called by the init() method.
     * In order to create custom interface for the Timer, override this method and create additional interface for the Timer.
     * When the custom interface is created, call addTimerInterface() method with the InterfaceDescription of the
     * created custom interface.
     *
     * @param bus BusAttachment
     * @return status ER_OK if succeeded in creating custom Timer
     */
    virtual QStatus createCustomInterfaceHook(BusAttachment* bus);

    /**
     * Adds the interface to this Timer BusObject.
     * This method must be called in order to add the interface identified by the given InterfaceDescription to this
     * Timer BusObject. Additionally this method registers the interface to be sent in the Announcement signal.
     *
     * @param iface InterfaceDescription
     * @return status Status of adding the interface
     */
    QStatus addTimerInterface(const InterfaceDescription& iface);

    /**
     * Callback for Alljoyn when GetProperty is called on this BusObject.
     * If custom Timer that contains properties is created, override this method and call it from
     * the child class, to handle Get property calls on the standard Timer interface.
     * If calling Get on the parent class returns status of ER_BUS_UNKNOWN_INTERFACE, it means that
     * the property doesn't belong to the standard Timer interface and should be handled by the custom Timer.
     *
     * @param interfaceName The name of the Interface
     * @param propName The name of the Property
     * @param msgArg Out variable the MsgArg to fill
     * @return status success/failure
     */
    virtual QStatus Get(const char* interfaceName, const char* propName, MsgArg& msgArg);

    /**
     * Callback for Alljoyn when SetProperty is called on this BusObject.
     * If custom Timer that contains properties is created, override this method and call it from
     * the child class, to handle Set property calls on the standard Timer interface.
     * If calling Set on the parent class returns status of ER_BUS_UNKNOWN_INTERFACE, it means that
     * the property doesn't belong to the standard Timer interface and should be handled by the custom Timer.
     *
     * @param interfaceName The name of the Interface
     * @param propName The name of the Property
     * @param msgArg The MsgArg that contains the new Value
     * @return status - success/failure
     */
    virtual QStatus Set(const char* interfaceName, const char* propName, MsgArg& msgArg);

  private:

    /**
     * Timer events delegate
     */
    TimeServiceServerTimer* m_Timer;

    /**
     * Object path of this Clock object
     */
    qcc::String m_ObjectPath;

    /**
     * The pointer is used to send TimerEvent signals
     */
    const ajn::InterfaceDescription::Member* m_TimerEventSignalMethod;

    /**
     * The pointer is used to send runStateChanged signals
     */
    const ajn::InterfaceDescription::Member* m_RunStateChangedSignalMethod;

    /**
     * TRUE if the object has been initialized by the call to the init() method
     */
    bool m_IsInitialized;

    /**
     * TRUE if the Timer should be announced
     */
    bool m_IsAnnounced;

    /**
     * Initialize the Bus Object. Register it on the BusAttachment and in the AboutService for Announcement
     *
     * @param Timer Events delegate. This Timer receives all the Timer related events.
     * to TimeServiceTimerBusObj will be announced.
     * @param description Events&Actions description
     * @param language Events&Actions description language
     * @param translator Events&Actions Translator
     * @return QStatus of the Timer initialization
     */
    QStatus init(TimeServiceServerTimer* Timer, const qcc::String& description, const qcc::String& language, Translator* translator);

    /**
     * Creates Timer interface if it hasn't been created yet and adds it to the BusObject.
     *
     * @param bus
     * @return status
     */
    QStatus createTimerInterface(BusAttachment* bus);

    /**
     * Handle Get Version request
     */
    QStatus handleGetVersion(MsgArg& msgArg);

    /**
     * Handle Get Interval request
     */
    QStatus handleGetInterval(MsgArg& msgArg);

    /**
     * Handle Get Time left request
     */
    QStatus handleGetTimeLeft(MsgArg& msgArg);

    /**
     * Handle Get Title request
     */
    QStatus handleGetTitle(MsgArg& msgArg);

    /**
     * Handle Get Is Running request
     */
    QStatus handleGetIsRunning(MsgArg& msgArg);

    /**
     * Handle Get Repeat request
     */
    QStatus handleGetRepeat(MsgArg& msgArg);

    /**
     * Handle Set Interval request
     */
    QStatus handleSetInterval(MsgArg& msgArg);

    /**
     * Handle Set Title request
     */
    QStatus handleSetTitle(MsgArg& msgArg);

    /**
     * Handle Set Repeat request
     */
    QStatus handleSetRepeat(MsgArg& msgArg);

    /**
     * Handle start method
     */
    void handleStart(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Handle pause method
     */
    void handlePause(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Handle Reset method
     */
    void handleReset(const InterfaceDescription::Member* member, Message& msg);

};

} /* namespace services */
} /* namespace ajn */

#endif /* TIMESERVICETIMERBUSOBJ_H_ */
