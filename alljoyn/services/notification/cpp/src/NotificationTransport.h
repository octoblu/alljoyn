/******************************************************************************
 * Copyright (c) 2013-2014, AllSeen Alliance. All rights reserved.
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

#ifndef NOTIFICATIONTRANSPORT_H_
#define NOTIFICATIONTRANSPORT_H_

#include <alljoyn/BusAttachment.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>

namespace ajn {
namespace services {

/**
 * A class implementing the BusObject used to create the Notification interface
 * and register it on the bus
 */
class NotificationTransport : public ajn::BusObject {
  public:

    /**
     * Constructor for NotificationTransport. Creates Interface and prepares
     * infrastructure to be able to send Signal
     * @param bus         - BusAttachment that is used
     * @param servicePath - servicePath of BusObject
     * @param status      - success/failure
     */
    NotificationTransport(ajn::BusAttachment* bus, qcc::String const& servicePath,
                          QStatus& status, qcc::String const& interfaceName);

    /**
     * Destructor for NotificationTransport
     */
    virtual ~NotificationTransport() = 0;

    /**
     * Callback for GetProperty
     * @param ifcName
     * @param propName
     * @param val
     * @return
     */
    QStatus Get(const char* ifcName, const char* propName, ajn::MsgArg& val);

    /**
     * Callback for SetProperty
     * @param ifcName
     * @param propName
     * @param val
     * @return
     */
    QStatus Set(const char* ifcName, const char* propName, ajn::MsgArg& val);

  protected:

    /**
     * The pointer used to send signal/register Signal Handler
     */
    const ajn::InterfaceDescription::Member* m_SignalMethod;
};
} //namespace services
} //namespace ajn

#endif /* NOTIFICATIONTRANSPORT_H_ */
