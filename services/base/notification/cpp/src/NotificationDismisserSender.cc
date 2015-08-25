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

#include <alljoyn/Message.h>
#include <alljoyn/notification/NotificationService.h>

#include "NotificationDismisserSender.h"
#include "NotificationConstants.h"
#include "Transport.h"
#include <alljoyn/notification/LogModule.h>

using namespace ajn;
using namespace services;
using namespace nsConsts;
using namespace qcc;

NotificationDismisserSender::NotificationDismisserSender(BusAttachment* bus, String const& objectPath, QStatus& status) :
    NotificationDismisser(bus, objectPath, status)

{
    /**
     * Do not add code until the status that returned from the base class is verified.
     */
    if (status != ER_OK) {
        return;
    }

    //Add code here
    QCC_DbgPrintf(("NotificationDismisserSender()  - Got objectpath=%s", objectPath.c_str()));
}

QStatus NotificationDismisserSender::sendSignal(ajn::MsgArg const dismisserArgs[AJ_DISMISSER_NUM_PARAMS],
                                                uint16_t ttl)
{
    QCC_DbgTrace(("Notification::sendSignal() called"));

    if (m_SignalMethod == 0) {
        QCC_LogError(ER_BUS_INTERFACE_NO_SUCH_MEMBER, ("signalMethod not set. Can't send signal"));
        return ER_BUS_INTERFACE_NO_SUCH_MEMBER;
    }

    uint8_t flags =  ALLJOYN_FLAG_SESSIONLESS;
    QStatus status = Signal(NULL, 0, *m_SignalMethod, dismisserArgs, AJ_DISMISSER_NUM_PARAMS, ttl, flags);

    if (status != ER_OK) {
        QCC_LogError(status, ("Could not send signal."));
        return status;
    }

    QCC_DbgPrintf(("Sent signal successfully"));
    return status;
}
