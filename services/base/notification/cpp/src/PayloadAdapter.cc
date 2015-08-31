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

#include <stdio.h>
#include <time.h>
#include <alljoyn/MsgArg.h>
#include <alljoyn/notification/Notification.h>
#include <alljoyn/notification/NotificationService.h>
#include "NotificationConstants.h"
#include "PayloadAdapter.h"
#include "Transport.h"
#include <alljoyn/notification/LogModule.h>

using namespace qcc;
using namespace ajn;
using namespace services;
using namespace nsConsts;

int32_t PayloadAdapter::m_MessageId(0);

QStatus PayloadAdapter::sendPayload(const char* deviceId, const char* deviceName,
                                    const uint8_t* appId, size_t appIdlen, const char* appName,
                                    NotificationMessageType messageType,
                                    std::vector<NotificationText> const&  notificationText,
                                    std::map<qcc::String, qcc::String> const& customAttributes,
                                    uint16_t ttl,
                                    const char* richIconUrl, std::vector<RichAudioUrl> const&  richAudioUrl,
                                    const char* richIconObjectPath, const char* richAudioObjectPath,
                                    const char* controlPanelServiceObjectPath, const char* originalSender, int32_t messageId)
{

    MsgArg deviceIdArg;
    MsgArg deviceNameArg;
    MsgArg appIdArg;
    MsgArg appNameArg;
    QStatus status;

    do {
        CHECK(deviceIdArg.Set(AJPARAM_STR.c_str(), deviceId));
        CHECK(deviceNameArg.Set(AJPARAM_STR.c_str(), deviceName));
        CHECK(appIdArg.Set(AJPARAM_ARR_BYTE.c_str(), appIdlen, appId));
        CHECK(appNameArg.Set(AJPARAM_STR.c_str(), appName));

        return(sendPayload(deviceIdArg, deviceNameArg, appIdArg, appNameArg, messageType, notificationText, customAttributes, ttl, richIconUrl, richAudioUrl, richIconObjectPath, richAudioObjectPath, controlPanelServiceObjectPath, originalSender, messageId));
    } while (0);

    QCC_LogError(status, ("Error occurred.  Could not marshal parameters."));

    return status;
}

QStatus PayloadAdapter::sendPayload(AboutData* propertyStore,
                                    NotificationMessageType messageType,
                                    std::vector<NotificationText> const&  notificationText,
                                    std::map<qcc::String, qcc::String> const& customAttributes,
                                    uint16_t ttl,
                                    const char* richIconUrl, std::vector<RichAudioUrl> const&  richAudioUrl,
                                    const char* richIconObjectPath, const char* richAudioObjectPath,
                                    const char* controlPanelServiceObjectPath, const char* originalSender)
{
    MsgArg deviceIdArg;
    MsgArg deviceNameArg;
    MsgArg appIdArg;
    MsgArg appNameArg;

    if (!m_MessageId) {
        srand(time(NULL));
        m_MessageId = rand();
    }

    MsgArg configArgs;
    MsgArg* configEntries;
    size_t configNum;
    QStatus status;

    if ((status = propertyStore->GetAboutData(&configArgs))) {
        return status;
    }

    if ((status = configArgs.Get(AJPARAM_ARR_DICT_STR_VAR.c_str(), &configNum, &configEntries))) {
        QCC_LogError(status, ("Error reading in about configuration data"));
        return status;
    }

    for (size_t i = 0; i < configNum; i++) {
        char* keyChar;
        String key;
        MsgArg* variant;

        CHECK(configEntries[i].Get(AJPARAM_DICT_STR_VAR.c_str(), &keyChar, &variant));

        key = keyChar;

        if (key.compare("DeviceId") == 0) {
            deviceIdArg = *variant;
        } else if (key.compare("DeviceName") == 0) {
            deviceNameArg = *variant;
        } else if (key.compare("AppId") == 0) {
            appIdArg = *variant;
        } else if (key.compare("AppName") == 0) {
            appNameArg = *variant;
        }
    }

    if (status != ER_OK) {
        QCC_LogError(status, ("Something went wrong unmarshalling the propertystore."));
        return status;
    }

    /* Validate Arguments */

    if (deviceIdArg.typeId != ALLJOYN_STRING) {
        QCC_LogError(ER_BAD_ARG_1, ("DeviceId argument is not correct type."));
        return ER_BAD_ARG_1;
    }
    if (deviceIdArg.v_string.str == 0 || deviceIdArg.v_string.len == 0) {
        QCC_LogError(ER_BAD_ARG_1, ("DeviceId argument can not be NULL or an empty String."));
        return ER_BAD_ARG_1;
    }

    if (deviceNameArg.typeId != ALLJOYN_STRING) {
        QCC_LogError(ER_BAD_ARG_1, ("DeviceName argument is not correct type."));
        return ER_BAD_ARG_1;
    }
    if (deviceNameArg.v_string.str == 0 || deviceNameArg.v_string.len == 0) {
        QCC_LogError(ER_BAD_ARG_1, ("DeviceName argument can not be NULL or an empty String."));
        return ER_BAD_ARG_1;
    }

    if (appIdArg.typeId != ALLJOYN_BYTE_ARRAY) {
        QCC_LogError(ER_BAD_ARG_1, ("ApplicationId argument is not correct type."));
        return ER_BAD_ARG_1;
    }

    if (appIdArg.v_scalarArray.numElements == 0) {
        QCC_LogError(ER_BAD_ARG_1, ("ApplicationId argument cannot be empty"));
        return ER_BAD_ARG_1;
    }

    if (appNameArg.typeId != ALLJOYN_STRING) {
        QCC_LogError(ER_BAD_ARG_1, ("ApplicationName argument is not correct type."));
        return ER_BAD_ARG_1;
    }
    if (appNameArg.v_string.str == 0 || appNameArg.v_string.len == 0) {
        QCC_LogError(ER_BAD_ARG_1, ("ApplicationName argument can not be NULL or an empty String."));
        return ER_BAD_ARG_1;
    }

    return (sendPayload(deviceIdArg, deviceNameArg, appIdArg, appNameArg, messageType, notificationText, customAttributes, ttl, richIconUrl, richAudioUrl, richIconObjectPath, richAudioObjectPath, controlPanelServiceObjectPath, originalSender, ++m_MessageId));
}

QStatus PayloadAdapter::sendPayload(PropertyStore* propertyStore,
                                    NotificationMessageType messageType,
                                    std::vector<NotificationText> const&  notificationText,
                                    std::map<qcc::String, qcc::String> const& customAttributes,
                                    uint16_t ttl,
                                    const char* richIconUrl, std::vector<RichAudioUrl> const&  richAudioUrl,
                                    const char* richIconObjectPath, const char* richAudioObjectPath,
                                    const char* controlPanelServiceObjectPath, const char* originalSender)
{
    MsgArg deviceIdArg;
    MsgArg deviceNameArg;
    MsgArg appIdArg;
    MsgArg appNameArg;

    if (!m_MessageId) {
        srand(time(NULL));
        m_MessageId = rand();
    }

    MsgArg configArgs[1];
    MsgArg* configEntries;
    size_t configNum;
    QStatus status;

    if ((status = propertyStore->ReadAll(0, PropertyStore::READ, configArgs[0]))) {
        return status;
    }

    if ((status = configArgs[0].Get(AJPARAM_ARR_DICT_STR_VAR.c_str(), &configNum, &configEntries))) {
        QCC_LogError(status, ("Error reading in about configuration data"));
        return status;
    }

    for (size_t i = 0; i < configNum; i++) {
        char* keyChar;
        String key;
        MsgArg* variant;

        CHECK(configEntries[i].Get(AJPARAM_DICT_STR_VAR.c_str(), &keyChar, &variant));

        key = keyChar;

        if (key.compare("DeviceId") == 0) {
            deviceIdArg = *variant;
        } else if (key.compare("DeviceName") == 0) {
            deviceNameArg = *variant;
        } else if (key.compare("AppId") == 0) {
            appIdArg = *variant;
        } else if (key.compare("AppName") == 0) {
            appNameArg = *variant;
        }
    }

    if (status != ER_OK) {
        QCC_LogError(status, ("Something went wrong unmarshalling the propertystore."));
        return status;
    }

    /* Validate Arguments */

    if (deviceIdArg.typeId != ALLJOYN_STRING) {
        QCC_LogError(ER_BAD_ARG_1, ("DeviceId argument is not correct type."));
        return ER_BAD_ARG_1;
    }
    if (deviceIdArg.v_string.str == 0 || deviceIdArg.v_string.len == 0) {
        QCC_LogError(ER_BAD_ARG_1, ("DeviceId argument can not be NULL or an empty String."));
        return ER_BAD_ARG_1;
    }

    if (deviceNameArg.typeId != ALLJOYN_STRING) {
        QCC_LogError(ER_BAD_ARG_1, ("DeviceName argument is not correct type."));
        return ER_BAD_ARG_1;
    }
    if (deviceNameArg.v_string.str == 0 || deviceNameArg.v_string.len == 0) {
        QCC_LogError(ER_BAD_ARG_1, ("DeviceName argument can not be NULL or an empty String."));
        return ER_BAD_ARG_1;
    }

    if (appIdArg.typeId != ALLJOYN_BYTE_ARRAY) {
        QCC_LogError(ER_BAD_ARG_1, ("ApplicationId argument is not correct type."));
        return ER_BAD_ARG_1;
    }

    if (appIdArg.v_scalarArray.numElements == 0) {
        QCC_LogError(ER_BAD_ARG_1, ("ApplicationId argument cannot be empty"));
        return ER_BAD_ARG_1;
    }

    if (appNameArg.typeId != ALLJOYN_STRING) {
        QCC_LogError(ER_BAD_ARG_1, ("ApplicationName argument is not correct type."));
        return ER_BAD_ARG_1;
    }
    if (appNameArg.v_string.str == 0 || appNameArg.v_string.len == 0) {
        QCC_LogError(ER_BAD_ARG_1, ("ApplicationName argument can not be NULL or an empty String."));
        return ER_BAD_ARG_1;
    }

    return (sendPayload(deviceIdArg, deviceNameArg, appIdArg, appNameArg, messageType, notificationText, customAttributes, ttl, richIconUrl, richAudioUrl, richIconObjectPath, richAudioObjectPath, controlPanelServiceObjectPath, originalSender, ++m_MessageId));
}

QStatus PayloadAdapter::sendPayload(ajn::MsgArg deviceIdArg, ajn::MsgArg deviceNameArg,
                                    ajn::MsgArg appIdArg, ajn::MsgArg appNameArg,
                                    NotificationMessageType messageType,
                                    std::vector<NotificationText> const&  notificationText,
                                    std::map<qcc::String, qcc::String> const& customAttributes,
                                    uint16_t ttl,
                                    const char* richIconUrl, std::vector<RichAudioUrl> const&  richAudioUrl,
                                    const char* richIconObjectPath, const char* richAudioObjectPath,
                                    const char* controlPanelServiceObjectPath, const char* originalSender, int32_t messageId)
{
    QStatus status;
    MsgArg versionArg;
    MsgArg messageIdArg;
    MsgArg messageTypeArg;
    MsgArg richIconUrlArg;
    MsgArg richAudioArg;
    MsgArg richIconObjectPathArg;
    MsgArg richAudioObjectPathArg;
    MsgArg controlPanelServiceObjectPathArg;
    MsgArg originalSenderArg;

    do {
        CHECK(versionArg.Set(AJPARAM_UINT16.c_str(), NotificationService::getVersion()));

        CHECK(messageIdArg.Set(AJPARAM_INT.c_str(), messageId));

        CHECK(messageTypeArg.Set(AJPARAM_UINT16.c_str(), messageType));

        int32_t attrSize = 1;

        if (richIconUrl) {
            attrSize++;
        }
        if (!richAudioUrl.empty()) {
            attrSize++;
        }
        if (richIconObjectPath) {
            attrSize++;
        }
        if (richAudioObjectPath) {
            attrSize++;
        }
        if (controlPanelServiceObjectPath) {
            attrSize++;
        }

        std::vector<MsgArg> attributes(attrSize);
        std::vector<RichAudioUrl>::const_iterator contIter;
        std::vector<MsgArg> audioContent(richAudioUrl.size());

        int32_t attrIndx = 0;

        CHECK(originalSenderArg.Set(AJPARAM_STR.c_str(), originalSender))
        CHECK(attributes[attrIndx++].Set(AJPARAM_DICT_INT_VAR.c_str(), ORIGINAL_SENDER_ATTRIBUTE_KEY, &originalSenderArg));

        if (richIconUrl) {
            CHECK(richIconUrlArg.Set(AJPARAM_STR.c_str(), richIconUrl))
            CHECK(attributes[attrIndx++].Set(AJPARAM_DICT_INT_VAR.c_str(), RICH_CONTENT_ICON_URL_ATTRIBUTE_KEY, &richIconUrlArg));
        }

        if (!richAudioUrl.empty()) {
            /* Fill the audioContent array using the richAudioUrl passed in
               Then Marshal attributes argument */

            int32_t audioIndx = 0;
            for (contIter = richAudioUrl.begin(); contIter != richAudioUrl.end(); contIter++) {
                if (!(contIter->getLanguage().size()) || !(contIter->getUrl().size())) {
                    QCC_LogError(ER_BAD_ARG_7, ("Problem sending message. Cannot send a message with audio content with an empty language/url values."));
                    return ER_BAD_ARG_7;
                }
                CHECK(audioContent[audioIndx++].Set(AJPARAM_STRUCT_STR_STR.c_str(), contIter->getLanguage().c_str(),
                                                    contIter->getUrl().c_str()));
            }
            if (status != ER_OK) {
                break;
            }

            CHECK(richAudioArg.Set(AJPARAM_ARR_STRUCT_STR_STR.c_str(), audioContent.size(), audioContent.data()))
            CHECK(attributes[attrIndx++].Set(AJPARAM_DICT_INT_VAR.c_str(), RICH_CONTENT_AUDIO_URL_ATTRIBUTE_KEY, &richAudioArg));
        }

        if (richIconObjectPath) {
            CHECK(richIconObjectPathArg.Set(AJPARAM_STR.c_str(), richIconObjectPath))
            CHECK(attributes[attrIndx++].Set(AJPARAM_DICT_INT_VAR.c_str(), RICH_CONTENT_ICON_OBJECT_PATH_ATTRIBUTE_KEY, &richIconObjectPathArg));
        }

        if (richAudioObjectPath) {
            CHECK(richAudioObjectPathArg.Set(AJPARAM_STR.c_str(), richAudioObjectPath))
            CHECK(attributes[attrIndx++].Set(AJPARAM_DICT_INT_VAR.c_str(), RICH_CONTENT_AUDIO_OBJECT_PATH_ATTRIBUTE_KEY, &richAudioObjectPathArg));
        }

        if (controlPanelServiceObjectPath) {
            CHECK(controlPanelServiceObjectPathArg.Set(AJPARAM_STR.c_str(), controlPanelServiceObjectPath))
            CHECK(attributes[attrIndx++].Set(AJPARAM_DICT_INT_VAR.c_str(), CPS_OBJECT_PATH_ATTRIBUTE_KEY, &controlPanelServiceObjectPathArg));
        }

        if (status != ER_OK) {
            break;
        }

        MsgArg attributesArg;
        CHECK(attributesArg.Set(AJPARAM_ARR_DICT_INT_VAR.c_str(), attrSize, attributes.data()));

        /* Fill the custom attributes array then using customAttributes
           Then Marshal the custom attributes argument */

        int32_t customAttrIndx = 0;
        std::vector<MsgArg> sendCustomAttributes(customAttributes.size());
        std::map<qcc::String, qcc::String>::const_iterator cAiter;
        for (cAiter = customAttributes.begin(); cAiter != customAttributes.end(); cAiter++) {
            CHECK(sendCustomAttributes[customAttrIndx++].Set(AJPARAM_DICT_STR_STR.c_str(),
                                                             cAiter->first.c_str(), cAiter->second.c_str()));
        }
        if (status != ER_OK) {
            break;
        }

        MsgArg customAttributesArg;
        CHECK(customAttributesArg.Set(AJPARAM_ARR_DICT_STR_STR.c_str(), customAttributes.size(), sendCustomAttributes.data()));

        /* Fill the notificationText array using the notifications passed in
           Then Marshal notificationText argument */

        int32_t notifIndx = 0;
        std::vector<NotificationText>::const_iterator notIter;
        std::vector<MsgArg> notifications(notificationText.size());

        for (notIter = notificationText.begin(); notIter != notificationText.end(); notIter++) {
            if (!(notIter->getLanguage().size()) || !(notIter->getText().size())) {
                QCC_LogError(ER_BAD_ARG_6, ("Problem sending message. Cannot send a message with an empty language/text values."));
                return ER_BAD_ARG_6;
            }
            CHECK(notifications[notifIndx++].Set(AJPARAM_STRUCT_STR_STR.c_str(), notIter->getLanguage().c_str(),
                                                 notIter->getText().c_str()));
        }
        if (status != ER_OK) {
            break;
        }

        MsgArg notificationTextArg;
        CHECK(notificationTextArg.Set(AJPARAM_ARR_STRUCT_STR_STR.c_str(), notificationText.size(), notifications.data()));

        MsgArg notificationArgs[AJ_NOTIFY_NUM_PARAMS];
        notificationArgs[0] = versionArg;
        notificationArgs[1] = messageIdArg;
        notificationArgs[2] = messageTypeArg;
        notificationArgs[3] = deviceIdArg;
        notificationArgs[4] = deviceNameArg;
        notificationArgs[5] = appIdArg;
        notificationArgs[6] = appNameArg;
        notificationArgs[7] = attributesArg;
        notificationArgs[8] = customAttributesArg;
        notificationArgs[9] = notificationTextArg;

        QCC_DbgPrintf(("Attempting to send messageId: %d", messageId));

        Transport* transport = Transport::getInstance();
        status = transport->sendNotification(messageType, notificationArgs, ttl);
        if (status == ER_OK) {
            QCC_DbgHLPrintf(("Message sent successfully with messageId: %d", messageId));
        }
        return status;
    } while (0);

    QCC_LogError(status, ("Error occurred.  Could not marshal parameters."));

    return status;
}

void PayloadAdapter::receivePayload(Message& msg)
{
    QStatus status;

    const MsgArg* versionArg = msg.unwrap()->GetArg(0);
    const MsgArg* messageIdArg = msg.unwrap()->GetArg(1);
    const MsgArg* messageTypeArg = msg.unwrap()->GetArg(2);
    const MsgArg* deviceIdArg = msg.unwrap()->GetArg(3);
    const MsgArg* deviceNameArg = msg.unwrap()->GetArg(4);
    const MsgArg* appIdArg = msg.unwrap()->GetArg(5);
    const MsgArg* appNameArg = msg.unwrap()->GetArg(6);
    const MsgArg* attributesArg = msg.unwrap()->GetArg(7);
    const MsgArg* customAttributesArg = msg.unwrap()->GetArg(8);
    const MsgArg* notificationsArg = msg.unwrap()->GetArg(9);

    const char* sender = msg->GetSender();

    do {
        //Unmarshal Version
        if (versionArg->typeId != ALLJOYN_UINT16) {
            QCC_DbgHLPrintf(("Problem receiving message: Can not Unmarshal this Version argument."));
            return;
        }

        uint16_t version;
        CHECK(versionArg->Get(AJPARAM_UINT16.c_str(), &version));

        //Unmarshal messageId
        if (messageIdArg->typeId != ALLJOYN_INT32) {
            QCC_DbgHLPrintf(("Problem receiving message: Can not Unmarshal this messageId argument."));
            return;
        }

        int32_t messageId;
        CHECK(messageIdArg->Get(AJPARAM_INT.c_str(), &messageId));

        //Unmarshal messageType
        if (messageTypeArg->typeId != ALLJOYN_UINT16) {
            QCC_DbgHLPrintf(("Problem receiving message: Can not Unmarshal this message type argument."));
            return;
        }

        uint16_t intVal;
        NotificationMessageType messageType;
        CHECK(messageTypeArg->Get(AJPARAM_UINT16.c_str(), &intVal));
        messageType = MessageTypeUtil::getMessageType(intVal);

        //Unmarshal deviceId
        if (deviceIdArg->typeId != ALLJOYN_STRING) {
            QCC_DbgHLPrintf(("Problem receiving message: Can not Unmarshal this deviceId argument."));
            return;
        }

        char* deviceId;
        CHECK(deviceIdArg->Get(AJPARAM_STR.c_str(), &deviceId));

        //Unmarshal deviceName
        if (deviceNameArg->typeId != ALLJOYN_STRING) {
            QCC_DbgHLPrintf(("Problem receiving message: Can not Unmarshal this device Name argument."));
            return;
        }

        char* deviceName;
        CHECK(deviceNameArg->Get(AJPARAM_STR.c_str(), &deviceName));

        //Unmarshal appId
        if (appIdArg->typeId != ALLJOYN_BYTE_ARRAY) {
            QCC_DbgHLPrintf(("Problem receiving message: Can not Unmarshal this appId argument."));
            return;
        }

        String appId;
        uint8_t* appIdBin;
        size_t len;
        CHECK(appIdArg->Get(AJPARAM_ARR_BYTE.c_str(), &len, &appIdBin));

        if (len != UUID_LENGTH) {
            QCC_DbgHLPrintf(("App id length is not equal to %u", UUID_LENGTH * 2));
            return;
        }

        //convert bytes to stringstrea
        uint32_t i;
        static const char* const chars = "0123456789ABCDEF";

        appId.reserve(2 * len);
        for (i = 0; i < len; ++i) {
            const unsigned char c = appIdBin[i];
            appId.push_back(chars[c >> 4]);
            appId.push_back(chars[c & 15]);
        }

        //Unmarshal appName
        if (appNameArg->typeId != ALLJOYN_STRING) {
            QCC_DbgHLPrintf(("Problem receiving message: Can not Unmarshal this app Name argument."));
            return;
        }

        char* appName;
        CHECK(appNameArg->Get(AJPARAM_STR.c_str(), &appName));

        //Unmarshal Attributes
        if (attributesArg->typeId != ALLJOYN_ARRAY) {
            QCC_DbgHLPrintf(("Problem receiving message: Can not Unmarshal this Attributes argument."));
            return;
        }

        MsgArg*attribEntries;
        size_t attribNum;
        char* richIconUrl = 0;
        char* richIconObjectPath = 0;
        char* richAudioObjectPath = 0;
        char* controlPanelServiceObjectPath = 0;
        char* originalSender = 0;
        std::vector<RichAudioUrl> richAudioUrl;


        CHECK(attributesArg->Get(AJPARAM_ARR_DICT_INT_VAR.c_str(), &attribNum, &attribEntries));

        for (size_t i = 0; i < attribNum; i++) {
            int32_t key;
            MsgArg* variant;

            CHECK(attribEntries[i].Get(AJPARAM_DICT_INT_VAR.c_str(), &key, &variant));

            switch (key) {
            case RICH_CONTENT_ICON_URL_ATTRIBUTE_KEY:
                {
                    CHECK(variant->Get(AJPARAM_STR.c_str(), &richIconUrl));
                    break;
                }

            case RICH_CONTENT_AUDIO_URL_ATTRIBUTE_KEY:
                {

                    MsgArg*richAudioEntries;
                    size_t richAudioNum;

                    CHECK(variant->Get(AJPARAM_ARR_STRUCT_STR_STR.c_str(), &richAudioNum, &richAudioEntries));

                    for (size_t i = 0; i < richAudioNum; i++) {
                        char*key;
                        char*StringVal;
                        status = richAudioEntries[i].Get(AJPARAM_STRUCT_STR_STR.c_str(), &key, &StringVal);
                        if (status != ER_OK) {
                            QCC_LogError(status, ("Can not Unmarshal this NotificationText argument."));
                            break;
                        }
                        richAudioUrl.push_back(RichAudioUrl(key, StringVal));
                    }
                    break;
                }

            case RICH_CONTENT_ICON_OBJECT_PATH_ATTRIBUTE_KEY:
                {
                    CHECK(variant->Get(AJPARAM_STR.c_str(), &richIconObjectPath));
                    break;
                }

            case RICH_CONTENT_AUDIO_OBJECT_PATH_ATTRIBUTE_KEY:
                {
                    CHECK(variant->Get(AJPARAM_STR.c_str(), &richAudioObjectPath));
                    break;
                }

            case CPS_OBJECT_PATH_ATTRIBUTE_KEY:
                {
                    CHECK(variant->Get(AJPARAM_STR.c_str(), &controlPanelServiceObjectPath));
                    break;
                }

            case ORIGINAL_SENDER_ATTRIBUTE_KEY:
                {
                    CHECK(variant->Get(AJPARAM_STR.c_str(), &originalSender));
                    break;
                }

            default:
                QCC_DbgHLPrintf(("Can not Unmarshal this attribute argument"));
                break;
            }
        } // for (size_t i = 0; i < attribNum; i++)

        //Unmarshal Custom Attributes
        if (customAttributesArg->typeId != ALLJOYN_ARRAY) {
            QCC_DbgHLPrintf(("Problem receiving message: Can not Unmarshal this custom Attributes argument."));
            return;
        }

        MsgArg*customAttributesEntries;
        size_t customAttributesNum;
        std::map<qcc::String, qcc::String> customAttributes;

        CHECK(customAttributesArg->Get(AJPARAM_ARR_DICT_STR_STR.c_str(), &customAttributesNum, &customAttributesEntries));

        for (size_t i = 0; i < customAttributesNum; i++) {
            char*key;
            char*StringVal;
            status = customAttributesEntries[i].Get(AJPARAM_DICT_STR_STR.c_str(), &key, &StringVal);
            if (status != ER_OK) {
                QCC_DbgHLPrintf(("Can not Unmarshal this Custom Attribute argument"));
                break;
            }
            customAttributes.insert(std::pair<qcc::String, qcc::String>(key, StringVal));
        }

        if (status != ER_OK) {
            break;
        }

        //Unmarshal NotificationTexts
        if (notificationsArg->typeId != ALLJOYN_ARRAY) {
            QCC_DbgHLPrintf(("Problem receiving message: Can not Unmarshal this NotificationsArg argument."));
            return;
        }

        MsgArg*notTextEntries;
        size_t notTextNum;
        CHECK(notificationsArg->Get(AJPARAM_ARR_STRUCT_STR_STR.c_str(), &notTextNum, &notTextEntries));

        std::vector<NotificationText> text;
        for (size_t i = 0; i < notTextNum; i++) {
            char*key;
            char*StringVal;
            status = notTextEntries[i].Get(AJPARAM_STRUCT_STR_STR.c_str(), &key, &StringVal);
            if (status != ER_OK) {
                QCC_DbgHLPrintf(("Can not Unmarshal this NotificationText argument"));
                break;
            }
            text.push_back(NotificationText(key, StringVal));
        }


        if (status != ER_OK) {
            break;
        }

        //Create notification and send it on
        Notification notification(messageId, messageType, deviceId, deviceName,
                                  appId.c_str(), appName, sender, customAttributes, text,
                                  richIconUrl, richAudioUrl, richIconObjectPath, richAudioObjectPath, controlPanelServiceObjectPath, originalSender);

        Transport* transport = Transport::getInstance();
        transport->onReceivedNotification(notification);
        return;

    } while (0);

    QCC_LogError(status, ("Error occurred.  Could not unmarshal parameters."));

    return;
}

