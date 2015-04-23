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

#ifndef TIMESERVICEALARMFACTORY_H_
#define TIMESERVICEALARMFACTORY_H_

#include <alljoyn/BusAttachment.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/time/TimeServiceServerAlarmFactory.h>
#include <alljoyn/Status.h>
#include <vector>

namespace ajn {
namespace services {

/**
 * This class implements AlarmFactory interface and realizes AllJoyn communication
 * with this TimeServiceAlarmFactory
 */
class TimeServiceAlarmFactoryBusObj : public ajn::BusObject {

  public:

    /**
     * Constructor
     *
     * @param objectPath
     */
    TimeServiceAlarmFactoryBusObj(qcc::String const& objectPath);

    /**
     * Destructor
     */
    ~TimeServiceAlarmFactoryBusObj();

    /**
     * Initialize the Bus Object. Register it on the BusAttachment and in the AboutService for Announcement
     *
     * @param alarmFactory Events delegate. This alarmFactory receives all the AlarmFactory related events.
     *
     * @return QStatus of the AlarmFactory initialization
     */
    QStatus init(TimeServiceServerAlarmFactory* alarmFactory);

    /**
     * Initialize the Bus Object. Register it on the BusAttachment and in the AboutService for Announcement
     *
     * @param alarmFactory Events delegate. This alarmFactory receives all the AlarmFactory related events.
     * @param description Events&Actions description
     * @param language Events&Actions description language
     * @param translator Events&Actions Translator
     *
     * @return QStatus of the AlarmFactory initialization
     */
    QStatus init(TimeServiceServerAlarmFactory* alarmFactory, const qcc::String& description, const qcc::String& language,
                 Translator* translator);

    /**
     * Releases object resources
     */
    void release();

    /**
     * Returns object path of this Alarm
     *
     * @return Returns object path of this AlarmFactory
     */
    qcc::String const& getObjectPath() const;

  private:

    /**
     * AlarmFactory events delegate
     */
    TimeServiceServerAlarmFactory* m_AlarmFactory;

    /**
     * Object path of this AlarmFactory object
     */
    const qcc::String m_ObjectPath;

    /**
     * Introspection description
     */
    qcc::String m_Description;

    /**
     * Introspection language
     */
    qcc::String m_Language;

    /**
     * Translator
     */
    Translator* m_Translator;

    /**
     * Adds the interface to this AlarmFactory BusObject.
     * This method must be called in order to add the interface identified by the given InterfaceDescription to this
     * AlarmFactory BusObject.
     *
     * @param bus BusAttachment
     * @return status Status of adding the interface
     */
    QStatus addAlarmFactoryInterface(BusAttachment* bus);

    /**
     * Callback for Alljoyn when GetProperty is called on this BusObject.
     *
     * @param interfaceName - the name of the Interface
     * @param propName - the name of the Property
     * @param val - the MsgArg to fill
     * @return status - success/failure
     */
    virtual QStatus Get(const char* interfaceName, const char* propName, MsgArg& msgArg);

    /**
     * Handles Get Version property request
     *
     * @param val Is the message argument
     */
    QStatus handleGetVersion(MsgArg& msgArg);

    /**
     * Handle New Alarm
     *
     * @param member interface member
     * @param msg Message
     */
    void handleNewAlarm(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Handle Delete Alarm
     *
     * @param member interface member
     * @param msg Message
     */
    void handleDeleteAlarm(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Call MethodReply with error description
     *
     * @param status QStatus
     * @param errorInfo Error name and description
     * @param msg Message context
     */
    void replyError(QStatus status, const tsConsts::ErrorInfo* errorInfo, Message& msg);
};

} /* namespace services */
} /* namespace ajn */

#endif /* TIMESERVICEALARMFACTORY_H_ */
