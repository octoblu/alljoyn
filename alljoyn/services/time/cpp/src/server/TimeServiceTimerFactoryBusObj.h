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

#ifndef TIMESERVICETIMERFACTORY_H_
#define TIMESERVICETIMERFACTORY_H_

#include <alljoyn/BusAttachment.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/time/TimeServiceServerTimerFactory.h>
#include <alljoyn/Status.h>
#include <vector>

namespace ajn {
namespace services {

/**
 * This class implements TimerFactory interface and realizes AllJoyn communication
 * with this TimeServiceTimerFactory
 */
class TimeServiceTimerFactoryBusObj : public ajn::BusObject {

  public:

    /**
     * Constructor
     *
     * @param objectPath
     */
    TimeServiceTimerFactoryBusObj(qcc::String const& objectPath);

    /**
     * Destructor
     */
    ~TimeServiceTimerFactoryBusObj();

    /**
     * Initialize the Bus Object. Register it on the BusAttachment and in the AboutService for Announcement
     *
     * @param TimerFactory Events delegate. This TimerFactory receives all the TimerFactory related events.
     *
     * @return QStatus of the TimerFactory initialization
     */
    QStatus init(TimeServiceServerTimerFactory* TimerFactory);

    /**
     * Initialize the Bus Object. Register it on the BusAttachment and in the AboutService for Announcement
     *
     * @param TimerFactory Events delegate. This TimerFactory receives all the TimerFactory related events.
     * @param description Events&Actions description
     * @param language Events&Actions description language
     * @param translator Events&Actions Translator
     *
     * @return QStatus of the TimerFactory initialization
     */
    QStatus init(TimeServiceServerTimerFactory* TimerFactory, const qcc::String& description, const qcc::String& language,
                 Translator* translator);

    /**
     * Releases object resources
     */
    void release();

    /**
     * Returns object path of this Timer
     *
     * @return Returns object path of this TimerFactory
     */
    qcc::String const& getObjectPath() const;

  private:

    /**
     * TimerFactory events delegate
     */
    TimeServiceServerTimerFactory* m_TimerFactory;

    /**
     * Object path of this TimerFactory object
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
     * Adds the interface to this TimerFactory BusObject.
     * This method must be called in order to add the interface identified by the given InterfaceDescription to this
     * TimerFactory BusObject.
     *
     * @param bus BusAttachment
     * @return status Status of adding the interface
     */
    QStatus addTimerFactoryInterface(BusAttachment* bus);

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
     * Handle New Timer
     *
     * @param member interface member
     * @param msg Message
     */
    void handleNewTimer(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Handle Delete Timer
     *
     * @param member interface member
     * @param msg Message
     */
    void handleDeleteTimer(const InterfaceDescription::Member* member, Message& msg);

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

#endif /* TIMESERVICETIMERFACTORY_H_ */
