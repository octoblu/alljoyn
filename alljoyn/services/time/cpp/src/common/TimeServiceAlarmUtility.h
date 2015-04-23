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

#ifndef TIMESERVICEALARMUTILITY_H_
#define TIMESERVICEALARMUTILITY_H_

#include <alljoyn/BusAttachment.h>
#include <alljoyn/Status.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/time/TimeServiceConstants.h>
#include <alljoyn/time/TimeServiceSchedule.h>

namespace ajn {
namespace services {
namespace tsAlarmUtility {

//Alarm interface and object
const qcc::String IFACE_PROP_VERSION            = "Version";
const qcc::String IFACE_PROP_SCHEDULE           = "Schedule";
const qcc::String IFACE_PROP_TITLE              = "Title";
const qcc::String IFACE_PROP_ENABLED            = "Enabled";
const qcc::String IFACE_SIG_ALARM_REACHED       = "AlarmReached";
const qcc::String IFACE_DESCRIPTION             = "Alarm";
const qcc::String IFACE_DESCRIPTION_LAGN        = "en";
const qcc::String IFACE_SIG_ALARM_REACHED_DESC  = "Alarm reached signal";
const qcc::String OBJ_PATH_PREFIX               = "/Alarm";
const uint16_t ALARM_REACHED_TTL_SECONDS        = 50;

const qcc::String IFACE_FAC_PROP_VERSION        = "Version";
const qcc::String IFACE_FAC_METHOD_NEW_ALARM    = "NewAlarm";
const qcc::String IFACE_FAC_METHOD_DELETE_ALARM = "DeleteAlarm";
const qcc::String FAC_OBJ_PATH_PREFIX           = "/AlarmFactory";

/**
 * Create Alarm interface
 *
 * @param bus BusAttachment
 * @param ptrIfaceDesc InterfaceDescription
 */
QStatus createInterface(BusAttachment* bus, InterfaceDescription** ptrIfaceDesc);

/**
 * Unmarshal Schedule
 *
 * @param msgArg Contains schedule to be unmarshalled
 * @param schedule Out variable. Unmarshalled Schedule
 */
QStatus unmarshalSchedule(const MsgArg&  msgArg, TimeServiceSchedule* schedule);

/**
 * Marshal Schedule
 *
 * @param msgArg Out variable. MsgArg contains marshalled schedule.
 * @param schedule Schedule to be marshalled.
 */
QStatus marshalSchedule(MsgArg& msgArg, const TimeServiceSchedule& schedule);

/**
 * Unmarshal title
 *
 * @param msgArg Contains title to be unmarshalled
 * @param title Out variable. Unmarshalled title
 */
QStatus unmarshalTitle(const MsgArg&  msgArg, qcc::String* title);

/**
 * Marshal Title
 *
 * @param msgArg Out variable. MsgArg contains marshalled title.
 * @param title Title to be marshalled.
 */
QStatus marshalTitle(MsgArg& msgArg, const qcc::String& title);

/**
 * Unmarshal Enabled property
 *
 * @param msgArg Contains Enabled property to be unmarshalled
 * @param isEnabled Out variable. Unmarshalled Enabled property
 */
QStatus unmarshalEnabled(const MsgArg&  msgArg, bool* isEnabled);

/**
 * Marshal Enabled property
 *
 * @param msgArg Out variable. MsgArg contains marshalled Enabled property.
 * @param isEnabled Enabled property to be marshalled.
 */
QStatus marshalEnabled(MsgArg& msgArg, const bool isEnabled);

/**
 * Create AlarmFactory interface
 *
 * @param bus BusAttachment
 * @param ptrIfaceDesc InterfaceDescription
 */
QStatus createFactoryInterface(BusAttachment* bus, InterfaceDescription** ptrIfaceDesc);

/**
 * Marshal object path
 *
 * @param msgArg Out. Contains object path to marshal
 * @param objPath Object path to marshal
 */
QStatus marshalObjectPath(MsgArg& msgArg, const qcc::String& objPath);

/**
 * Unmarshal object path
 *
 * @param msgArg Out. Contains object path to be unmarshalled
 * @param objPath Object path to unmarshal
 */
QStatus unmarshalObjectPath(const MsgArg& msgArg, qcc::String* objPath);

}  /* namespace tsAlarmUtility */
}  /* namespace services */
}  /* namespace ajn */

#endif /* TIMESERVICEALARMUTILITY_H_ */
