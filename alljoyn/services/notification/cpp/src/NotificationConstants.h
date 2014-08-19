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

#ifndef NOTIFICATIONCONSTANTS_H_
#define NOTIFICATIONCONSTANTS_H_

#include <qcc/String.h>

/*
 * Common Macros
 */

#define CHECK(x) if ((status = x))  { break; }

namespace ajn {
namespace services {
namespace nsConsts {

static const qcc::String BUS_NAME = "NotificationService";

static const uint16_t TTL_MIN = 30;
static const uint16_t TTL_MAX = 43200;

static const uint16_t UUID_LENGTH = 16;
static const int16_t ANNOUNCMENT_PORT_NUMBER = 900;

static const uint16_t NOTIFICATION_DISMISSER_VERSION = 1;
static const uint16_t NOTIFICATION_PRODUCER_VERSION = 1;

static const qcc::String AJPARAM_BOOL = "b";
static const qcc::String AJPARAM_UINT16 = "q";
static const qcc::String AJPARAM_STR = "s";
static const qcc::String AJPARAM_INT = "i";
static const qcc::String AJPARAM_ARR_BYTE = "ay";
static const qcc::String AJPARAM_DICT_INT_VAR = "{iv}";
static const qcc::String AJPARAM_DICT_INT_UINT16 = "{iq}";
static const qcc::String AJPARAM_DICT_INT_STR = "{is}";
static const qcc::String AJPARAM_DICT_STR_STR = "{ss}";
static const qcc::String AJPARAM_DICT_STR_VAR = "{sv}";
static const qcc::String AJPARAM_ARR_DICT_INT_VAR = "a{iv}";
static const qcc::String AJPARAM_ARR_DICT_STR_VAR = "a{sv}";
static const qcc::String AJPARAM_ARR_DICT_STR_STR = "a{ss}";
static const qcc::String AJPARAM_STRUCT_STR_STR = "(ss)";
static const qcc::String AJPARAM_ARR_STRUCT_STR_STR = "a(ss)";
static const qcc::String AJPARAM_ARR_STRUCT_STR_ARR_STR = "a(sas)";
static const qcc::String AJPARAM_STRUCT_STR_ARR_STR = "(sas)";

static const int32_t AJ_NOTIFY_NUM_PARAMS = 10;
static const int32_t AJ_NUM_METADATA_DEFLT_PARAMS = 0;
static const int32_t RICH_CONTENT_ICON_URL_ATTRIBUTE_KEY = 0;
static const int32_t RICH_CONTENT_AUDIO_URL_ATTRIBUTE_KEY = 1;
static const int32_t RICH_CONTENT_ICON_OBJECT_PATH_ATTRIBUTE_KEY = 2;
static const int32_t RICH_CONTENT_AUDIO_OBJECT_PATH_ATTRIBUTE_KEY = 3;
static const int32_t CPS_OBJECT_PATH_ATTRIBUTE_KEY = 4;
static const int32_t ORIGINAL_SENDER_ATTRIBUTE_KEY = 5;

static const qcc::String AJ_NOTIFICATION_INTERFACE_NAME = "org.alljoyn.Notification";
static const uint16_t AJ_NOTIFICATION_PRODUCER_SERVICE_PORT = 1010;
static const qcc::String AJ_PROPERTY_VERSION = "Version";
static const qcc::String AJ_SIGNAL_METHOD = "notify";
static const qcc::String AJ_CONSUMER_SERVICE_PATH = "/receiver";
static const qcc::String AJ_PRODUCER_SERVICE_PATH_PREFIX = "/";
static const qcc::String AJ_NOTIFY_PARAMS = AJPARAM_UINT16 + AJPARAM_INT + AJPARAM_UINT16 + AJPARAM_STR + AJPARAM_STR + AJPARAM_ARR_BYTE + AJPARAM_STR +
                                            AJPARAM_ARR_DICT_INT_VAR + AJPARAM_ARR_DICT_STR_STR + AJPARAM_ARR_STRUCT_STR_STR;
static const qcc::String AJ_NOTIFY_PARAM_NAMES = "version, notificationId, messageType, deviceId, deviceName, appId, appName, attributes, customAttributes, notificationText";
static const qcc::String AJ_SESSIONLESS_MATCH = "sessionless='t'";
static const qcc::String AJ_SA_INTERFACE_NAME = "org.alljoyn.Notification.Superagent";

static const qcc::String AJ_NOTIFICATION_PRODUCER_INTERFACE = "org.alljoyn.Notification.Producer";
static const qcc::String AJ_NOTIFICATION_PRODUCER_PATH = "/notificationProducer";
static const qcc::String AJ_DISMISS_METHOD_NAME = "Dismiss";
static const qcc::String AJ_DISMISS_METHOD_PARAMS = "i";
static const qcc::String AJ_DISMISS_METHOD_PARAMS_NAMES = "msgId";

static const qcc::String AJ_NOTIFICATION_DISMISSER_INTERFACE = "org.alljoyn.Notification.Dismisser";
static const qcc::String AJ_DISMISS_SIGNAL_NAME = "Dismiss";
static const qcc::String AJ_DISMISS_SIGNAL_PARAMS = AJPARAM_INT + AJPARAM_ARR_BYTE;
static const qcc::String AJ_DISMISS_PARAM_NAMES = "msgId, appId";
static const int32_t AJ_DISMISSER_NUM_PARAMS = 2;
static const qcc::String AJ_NOTIFICATION_DISMISSER_PATH = "/notificationDismisser";

}         //namespace nsConsts
}     //namespace services
} //namespace ajn


#endif /* NOTIFICATIONCONSTANTS_H_ */

