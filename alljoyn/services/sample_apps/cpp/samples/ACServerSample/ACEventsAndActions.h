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

#ifndef ACEVENTSANDACTIONS_H_
#define ACEVENTSANDACTIONS_H_

#include <qcc/platform.h>
#include <qcc/String.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/MsgArg.h>
#include <alljoyn/Status.h>

namespace ajn {
namespace services {

/**
 * ACEventsAndActions is the bus object that implements events and actions for the ACServerSample.
 */
class ACEventsAndActions : public ajn::BusObject {
  public:
    /**
     * ACEventsAndActions CTOR.
     * @param bus attachment.
     */
    ACEventsAndActions(ajn::BusAttachment* bus);
    /**
     * ACEventsAndActions DTOR.
     */
    virtual ~ACEventsAndActions();

    /**
     * Get property value.
     * @param ifcName - interface name
     * @param propName - property name
     * @param val - value to get
     * @return status
     */
    QStatus Get(const char* ifcName, const char* propName, MsgArg& val);

    /**
     * Set property value.
     * @param ifcName - interface name
     * @param propName - property name
     * @param val - value to set
     * @return status
     */
    QStatus Set(const char* ifcName, const char* propName, MsgArg& val);

    /**
     * send event method
     * @param actualEvent - send relevant signal for the event
     * @return status
     */
    QStatus SendEvent(const InterfaceDescription::Member* actualEvent);

    /**
     * send events for actions
     */
    void SendEventsForActions();

  private:
    void ActionSetModeToAuto(const ajn::InterfaceDescription::Member* member, ajn::Message& msg);
    void ActionSetModeToCool(const ajn::InterfaceDescription::Member* member, ajn::Message& msg);
    void ActionSetModeToHeat(const ajn::InterfaceDescription::Member* member, ajn::Message& msg);
    void ActionSetModeToFan(const ajn::InterfaceDescription::Member* member, ajn::Message& msg);
    void ActionSetModeToOff(const ajn::InterfaceDescription::Member* member, ajn::Message& msg);
    void Check_MethodReply(const Message& msg, QStatus status);

    void EventsInterface();
    void ActionsInterface();

    BusAttachment* bus;
    const InterfaceDescription::Member* temperature80ReachedMember;
    const InterfaceDescription::Member* temperature60ReachedMember;
    const InterfaceDescription::Member* modeSetToOffMember;
    const InterfaceDescription::Member* modeSetToOnMember;
    qcc::String version;
};

} //namespace services
} //namespace ajn

#endif /* ACEVENTSANDACTIONS_H_ */
