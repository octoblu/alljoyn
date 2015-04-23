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

#ifndef TIMESERVICECLOCKUTILITY_H_
#define TIMESERVICECLOCKUTILITY_H_

#include <alljoyn/BusAttachment.h>
#include <alljoyn/Status.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/time/TimeServiceDateTime.h>
#include <alljoyn/time/TimeServiceDate.h>
#include <alljoyn/time/TimeServiceTime.h>
#include <alljoyn/time/TimeServiceConstants.h>
#include <alljoyn/time/TimeServiceServerAuthorityClock.h>

namespace ajn {
namespace services {
namespace tsClockUtility {

//Clock interface and object
const qcc::String IFACE_PROP_VERSION       = "Version";
const qcc::String IFACE_PROP_DATE_TIME     = "DateTime";
const qcc::String IFACE_PROP_IS_SET        = "IsSet";
const qcc::String OBJ_PATH_PREFIX          = "/Clock";

//Clock Authority interface and object
const qcc::String IFACE_AUTH_PROP_VERSION  = "Version";
const qcc::String IFACE_AUTH_PROP_TYPE     = "AuthorityType";
const qcc::String IFACE_AUTH_SIG_TIME_SYNC = "TimeSync";
const qcc::String OBJ_PATH_AUTH_PREFIX     = "/AuthorityClock";
const uint16_t TIME_SYNC_TTL_SECONDS       = 180;

/**
 * Create Clock interface
 *
 * @param bus BusAttachment
 * @param ptrIfaceDesc InterfaceDescription
 */
QStatus createInterface(BusAttachment* bus, InterfaceDescription** ptrIfaceDesc);

/**
 * Unmarshal DateTime
 *
 * @param val MsgArg
 * @param out variable, DateTime to be populated with the unmarshalled data
 */
QStatus unmarshalDateTime(const MsgArg& val, TimeServiceDateTime* dateTime);

/**
 * Marshal DateTime
 *
 * @param msg out variable. MsgArg contains marshalled DateTime
 * @param DateTime to be marshalled
 */
QStatus marshalDateTime(MsgArg& msg, const TimeServiceDateTime& dateTime);

/**
 * Unmarshal IsSet
 *
 * @param val MsgArg
 * @param isSet out variable
 */
QStatus unmarshalIsSet(const MsgArg& val, bool* isSet);

/**
 * Marshal IsSet
 *
 * @param msg out variable. MsgArg contains marshalled IsSet
 * @param IsSet to be marshalled
 */
QStatus marshalIsSet(MsgArg& msg, bool isSet);

/**
 * Create Time Authority interface
 */
QStatus createAuthorityInterface(BusAttachment* bus, InterfaceDescription** ptrIfaceDesc);

/**
 * Marshal AuthorityType
 *
 * @param msg Out variable. MsgArg contains marshalled AuthorityType
 * @param type AuthorityType to be marshalled
 */
QStatus marshalAuthorityType(MsgArg& msg, tsConsts::ClockAuthorityType type);

/**
 * Marshal AuthorityType
 *
 * @param msg out variable. MsgArg contains AuthorityType to be unmarshalled
 * @param type Out Unmarshalled AuthorityType
 */
QStatus unmarshalAuthorityType(const MsgArg& msg, tsConsts::ClockAuthorityType* type);

}  /* namespace tsClockUtility */
}  /* namespace services */
}  /* namespace ajn */

#endif /* TIMESERVICECLOCKUTILITY_H_ */
