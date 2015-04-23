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

#include <alljoyn/time/TimeServiceObjectIntrospector.h>
#include <alljoyn/time/LogModule.h>
#include <alljoyn/time/TimeServiceClient.h>
#include "TimeServiceIntrospectionNode.h"
#include "../common/TimeServiceUtility.h"

using namespace ajn;
using namespace services;

//Constructor
TimeServiceObjectIntrospector::TimeServiceObjectIntrospector(const TimeServiceClient& tsClient,
                                                             const qcc::String& objectPath) : TimeServiceClientBase(tsClient, objectPath),
    m_LangRetrieved(false)
{
}

//Destructor
TimeServiceObjectIntrospector::~TimeServiceObjectIntrospector()
{
}

//Retrieves array of description languages supported by the introspected object.
const std::vector<qcc::String>& TimeServiceObjectIntrospector::retrieveDescriptionLanguages()
{

    if (m_LangRetrieved) {

        return m_DescriptionLanguages;
    }

    QCC_DbgPrintf(("Retrieving description languages, ObjectPath: '%s'", m_ObjectPath.c_str()));

    ConnectionData connData;
    QStatus status = getConnectionData(&connData);

    if (status != ER_OK) {

        return m_DescriptionLanguages;
    }

    ProxyBusObject proxyObj(*connData.bus, m_TsClient.getServerBusName().c_str(), getObjectPath().c_str(), connData.sid);

    const InterfaceDescription::Member* introspectMethod = tsUtility::getIntrospectionMember(connData.bus, &proxyObj,
                                                                                             tsUtility::GET_LANGUAGES, status);

    if (status != ER_OK) {

        QCC_LogError(status, ("Bad interface description"));
        return m_DescriptionLanguages;
    }

    Message msg(*connData.bus);
    status = proxyObj.MethodCall(*introspectMethod, NULL, 0, msg);

    if (status != ER_OK) {

        return m_DescriptionLanguages;
    }

    unmarshalDescriptionLanguages(msg);

    m_LangRetrieved = true;
    return m_DescriptionLanguages;
}

//Retrieve description of the introspected object.
const qcc::String TimeServiceObjectIntrospector::retrieveObjectDescription(const qcc::String& language)
{

    //Check that description for the given language hasn't been retrieved before
    std::map<qcc::String, qcc::String>::iterator iter = m_ObjectDescription.find(language);
    if (iter != m_ObjectDescription.end()) {

        return iter->second;
    }

    QCC_DbgPrintf(("Retrieving object description in language: '%s', ObjectPath: '%s'", language.c_str(), m_ObjectPath.c_str()));

    //If the given language is valid retrieve object description
    if (language.length() == 0 || checkDescLanguageValidity(language) != ER_OK) {

        QCC_LogError(ER_FAIL, ("Unknown language"));
        return qcc::String::Empty;
    }

    ConnectionData connData;
    QStatus status = getConnectionData(&connData);

    if (status != ER_OK) {

        return qcc::String::Empty;
    }

    TimeServiceIntrospectionNode introspector(m_ObjectPath);
    status = introspector.parse(connData.bus, m_TsClient.getServerBusName(), connData.sid, language);

    if (status != ER_OK) {

        QCC_LogError(status, ("Introspection of the Object Path :'%s' has failed", m_ObjectPath.c_str()));
        return status;
    }

    const qcc::String& objDesc = introspector.getObjectDescription();

    //Store language and its object description
    m_ObjectDescription.insert(std::pair<qcc::String, qcc::String>(language, objDesc));

    return objDesc;
}

//Introspect server BusObject and retrieve child objects identified by their object paths
QStatus TimeServiceObjectIntrospector::retrieveChildObjects(std::vector<qcc::String>* childObjects)
{

    QCC_DbgPrintf(("Retrieving child objects for the  object path: '%s'", m_ObjectPath.c_str()));

    ConnectionData connData;
    QStatus status = getConnectionData(&connData);

    if (status != ER_OK) {

        return status;
    }

    TimeServiceIntrospectionNode introspector(m_ObjectPath);
    status = introspector.parse(connData.bus, m_TsClient.getServerBusName(), connData.sid, tsUtility::NO_LANG);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to introspect Child objects of the object path: '%s'", m_ObjectPath.c_str()));
        return status;
    }

    const std::vector<qcc::String>& childObjectsIntr = introspector.getChildObjects();

    if (childObjectsIntr.size() > 0) {

        childObjects->assign(childObjectsIntr.begin(), childObjectsIntr.end());
    } else {

        QCC_DbgPrintf(("Not found any child object for the object path: '%s'", m_ObjectPath.c_str()));
    }

    return ER_OK;
}

//Checks validity of the given descriptionLanguage
QStatus TimeServiceObjectIntrospector::checkDescLanguageValidity(const qcc::String& language)
{

    QStatus status = ER_FAIL;
    const std::vector<qcc::String>& langs = retrieveDescriptionLanguages();

    for (std::vector<qcc::String>::const_iterator iter = langs.begin(); iter != langs.end(); ++iter) {

        if (language.compare(*iter) == 0) {

            status = ER_OK;
            break;
        }
    }

    return status;
}

//Description languages
void TimeServiceObjectIntrospector::unmarshalDescriptionLanguages(Message& replyMsg)
{

    const MsgArg* arg = replyMsg->GetArg(0);
    if (!arg) {

        QCC_LogError(ER_BAD_ARG_COUNT, ("Fail to get MsgArg"));
        return;
    }

    MsgArg* asArray;
    size_t numLangs;

    QStatus status = arg->Get("as", &numLangs, &asArray);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to Get Description Languages"));
        return;
    }

    for (size_t i = 0; i < numLangs; ++i) {

        char* lang;
        status = asArray[i].Get("s", &lang);

        if (status != ER_OK) {

            QCC_LogError(status, ("Failed to unmarshal Description Languages"));
            return;
        }

        m_DescriptionLanguages.push_back(lang);
    }
}
