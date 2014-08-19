/******************************************************************************
 * Copyright (c) 2013, AllSeen Alliance. All rights reserved.
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

#include <alljoyn/notification/NotificationEnums.h>

using namespace ajn;
using namespace services;

const qcc::String MessageTypeUtil::MESSAGE_TYPE_STRINGS[3] = { "emergency", "warning", "info" };

qcc::String const& MessageTypeUtil::getMessageTypeString(int32_t messageType)
{
    return MESSAGE_TYPE_STRINGS[messageType];
}

NotificationMessageType MessageTypeUtil::getMessageType(int32_t messageType)
{
    return (NotificationMessageType) messageType;
}

int32_t MessageTypeUtil::getNumMessageTypes()
{
    return (int32_t) MESSAGE_TYPE_CNT;
}

