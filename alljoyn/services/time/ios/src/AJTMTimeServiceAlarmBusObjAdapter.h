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

/**
 * This class is experimental, and as such has not been tested.
 * Please help make it more robust by contributing fixes if you find issues
 **/

#import "alljoyn/time/TimeServiceAlarmBusObj.h"
#import "AJTMTimeServiceAlarmBusObj.h"

class AJTMTimeServiceAlarmBusObjAdapter : public ajn::services::TimeServiceAlarmBusObj {
public:

    id<AJTMTimeServiceAlarmBusObj> handle;

    /**
     * Constructor
     *
     * @param timeServiceAlarmBusObj
     */
    AJTMTimeServiceAlarmBusObjAdapter(id<AJTMTimeServiceAlarmBusObj> timeServiceAlarmBusObj);


    /**
     * Constructor
     *
     * @param timeServiceAlarmBusObj
     * @param objectPath
     */
    AJTMTimeServiceAlarmBusObjAdapter(id<AJTMTimeServiceAlarmBusObj> timeServiceAlarmBusObj, qcc::String const& objectPath);

    /**
     * Destructor
     */
    ~AJTMTimeServiceAlarmBusObjAdapter();


//    /**
//     * Initialize the Bus Object. Register it on the BusAttachment and in the AboutService for Announcement
//     *
//     * @param alarm Events delegate. This alarm receives all the Alarm related events.
//     * @param notAnnounced Interfaces that shouldn't be announced. If the vector is empty, all the interfaces added
//     * to TimeServiceAlarmBusObj will be announced.
//     * @return QStatus of the Alarm initialization
//     */
//    QStatus init(ajn::services::TimeServiceServerAlarm* alarm, const std::vector<qcc::String>& notAnnounced);
//
//    /**
//     * Initialize the Bus Object. Register it on the BusAttachment and in the AboutService for Announcement
//     *
//     * @param alarm Events delegate. This alarm receives all the Alarm related events.
//     * @param notAnnounced Interfaces that shouldn't be announced. If the vector is empty, all the interfaces added
//     * to TimeServiceAlarmBusObj will be announced.
//     * @param description Events&Actions description
//     * @param language Events&Actions description language
//     * @param translator Events&Actions Translator
//     * @return QStatus of the Alarm initialization
//     */
//    QStatus init(ajn::services::TimeServiceServerAlarm* alarm, const std::vector<qcc::String>& notAnnounced, const qcc::String& description,const qcc::String& language, ajn::Translator* translator);

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
    QStatus createCustomInterfaceHook(ajn::BusAttachment* bus);

    /**
     * Adds the interface to this Alarm BusObject.
     * This method must be called in order to add the interface identified by the given InterfaceDescription to this
     * Alarm BusObject. Additionally this method registers the interface to be sent in the Announcement signal.
     *
     * @param iface InterfaceDescription
     * @return status Status of adding the interface
     */
    QStatus addAlarmInterface(const ajn::InterfaceDescription& iface);

    /**
     * Callback for Alljoyn when GetProperty is called on this BusObject.
     * If custom Alarm that contains properties is created, override this method and call it from
     * the child class, to handle Get property calls on the standard Alarm interface.
     * If calling Get on the parent class returns status of ER_BUS_UNKNOWN_INTERFACE, it means that
     * the property doesn't belong to the standard Alarm interface and should be handled by the custom Alarm.
     *
     * @param interfaceName - the name of the Interface
     * @param propName - the name of the Property
     * @param val - the MsgArg to fill
     * @return status - success/failure
     */
    QStatus Get(const char* interfaceName, const char* propName, ajn::MsgArg& msgArg);

    /**
     * Callback for Alljoyn when SetProperty is called on this BusObject.
     * If custom Alarm that contains properties is created, override this method and call it from
     * the child class, to handle Set property calls on the standard Alarm interface.
     * If calling Set on the parent class returns status of ER_BUS_UNKNOWN_INTERFACE, it means that
     * the property doesn't belong to the standard Alarm interface and should be handled by the custom Alarm.
     *
     * @param interfaceName - the name of the Interface
     * @param propName - the name of the Property
     * @param val - the MsgArg that contains the new Value
     * @return status - success/failure
     */
    QStatus Set(const char* interfaceName, const char* propName, ajn::MsgArg& msgArg);
};