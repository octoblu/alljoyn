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

#ifndef DIALOGBUSOBJECT_H_
#define DIALOGBUSOBJECT_H_

#include "WidgetBusObject.h"

namespace ajn {
namespace services {

/**
 * DialogBusObject - BusObject for Dialogs
 */
class DialogBusObject : public WidgetBusObject {
  public:

    /**
     * Constructor for DialogBusObject class
     * @param bus - the bus to create the interface
     * @param objectPath - objectPath of BusObject
     * @param langIndx - the languageIndex of the BusObject
     * @param status - success/failure
     * @param widget - the widget associated with the BusObject
     */
    DialogBusObject(ajn::BusAttachment* bus, qcc::String const& objectPath,
                    uint16_t langIndx, QStatus& status, Widget* widget);

    /**
     * Destructor for DialogBusObject
     */
    virtual ~DialogBusObject();

    /**
     * Callback for Alljoyn when GetProperty is called on this BusObject
     * @param interfaceName - the name of the Interface
     * @param propName - the name of the Property
     * @param val - the MsgArg to fill
     * @return status - success/failure
     */
    QStatus Get(const char* interfaceName, const char* propName, MsgArg& val);

    /**
     * Callback when Execute is called
     * @param member - the member (method) of the interface that was executed
     * @param msg - the Message of the method
     */
    void DialogExecute(const ajn::InterfaceDescription::Member* member, ajn::Message& msg);

    /**
     * Execute this Dialog's remoteAction 1
     * @return status - success/failure
     */
    QStatus ExecuteAction1();

    /**
     * Execute this Dialog's remoteAction 2
     * @return status - success/failure
     */
    QStatus ExecuteAction2();

    /**
     * Execute this Dialog's remoteAction 3
     * @return status - success/failure
     */
    QStatus ExecuteAction3();

  private:

    QStatus fillProperty(char* key, MsgArg* variant);
};

} /* namespace services */
} /* namespace ajn */
#endif /* DIALOGBUSOBJECT_H_ */
