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

#include "TimeServiceIntrospectionNode.h"
#include <alljoyn/time/LogModule.h>
#include "../common/TimeServiceUtility.h"

using namespace ajn;
using namespace services;

//Constructor
TimeServiceIntrospectionNode::TimeServiceIntrospectionNode(qcc::String objectPath) : m_ObjectPath(objectPath)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
}


//Destructor
TimeServiceIntrospectionNode::~TimeServiceIntrospectionNode()
{

    QCC_DbgTrace(("%s", __FUNCTION__));
}

//Returns ObjectDescription
const qcc::String& TimeServiceIntrospectionNode::getObjectDescription()
{

    return m_ObjectDescription;
}

//Returns vector of the children object paths
const std::vector<qcc::String>& TimeServiceIntrospectionNode::getChildObjects()
{

    return m_ChildObjects;
}

//Parse the object
QStatus TimeServiceIntrospectionNode::parse(BusAttachment* bus, const qcc::String& busName, ajn::SessionId sessionId,
                                            const qcc::String& language)
{

    QCC_DbgTrace(("%s, BusName: '%s', SID: '%u', Language: '%s'", __FUNCTION__, busName.c_str(), sessionId, language.c_str()));

    qcc::String xml;
    QStatus status = introspect(bus, busName, sessionId, language, xml);

    if (status != ER_OK) {

        return status;
    }

    if (xml.size() == 0) {

        QCC_LogError(ER_FAIL, ("No introspection XML"));
        return ER_FAIL;
    }

    xmlDocPtr doc  = NULL;
    status         = loadXml(xml.c_str(), &doc);

    if (status != ER_OK) {

        QCC_LogError(status, ("Failed to load introspection XML"));
        return status;
    }

    parseXml(doc);
    xmlFreeDoc(doc);

    return ER_OK;
}

//Introspect
QStatus TimeServiceIntrospectionNode::introspect(BusAttachment* bus, const qcc::String& busName, ajn::SessionId sessionId,
                                                 const qcc::String& language, qcc::String& xml)
{

    QCC_DbgTrace(("%s, BusName: '%s', SID: '%u', Language: '%s'", __FUNCTION__, busName.c_str(), sessionId, language.c_str()));

    QStatus status;
    ProxyBusObject proxyObj(*bus, busName.c_str(), m_ObjectPath.c_str(), sessionId);

    const InterfaceDescription::Member* introspectMethod = tsUtility::getIntrospectionMember(bus, &proxyObj,
                                                                                             tsUtility::INTROSPECT, status);

    if (!introspectMethod) {

        return status;
    }

    MsgArg langArg[1];
    status = langArg[0].Set("s", language.c_str());

    if (status != ER_OK) {

        return status;
    }

    Message replyMsg(*bus);
    status = proxyObj.MethodCall(*introspectMethod, langArg, 1, replyMsg);

    if (status != ER_OK) {

        return status;
    }

    status = unmarshalXml(replyMsg, xml);

    if (status != ER_OK) {

        return status;
    }

    return ER_OK;
}

//Unmarshal Introspection XML
QStatus TimeServiceIntrospectionNode::unmarshalXml(Message& msg, qcc::String& xml)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    const MsgArg* arg = msg->GetArg(0);
    if (!arg) {

        return ER_BAD_ARG_COUNT;
    }

    char* parsedXml = NULL;
    QStatus status  = arg->Get("s", &parsedXml);

    if (status != ER_OK) {

        return status;
    }

    xml.assign(parsedXml);
    return ER_OK;
}

//Load XML
QStatus TimeServiceIntrospectionNode::loadXml(const qcc::String& xml, xmlDocPtr* doc)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    (*doc) = xmlReadMemory(xml.c_str(), xml.size(), NULL, "UTF-8", XML_PARSE_NOERROR | XML_PARSE_NOBLANKS);

    if (!*doc) {

        QCC_LogError(ER_XML_MALFORMED, ("Failed to load XML"));
        return ER_XML_MALFORMED;
    }

    if (((*doc)->properties & XML_DOC_WELLFORMED) != XML_DOC_WELLFORMED) {

        QCC_LogError(ER_BUS_BAD_XML, ("Failed to load XML"));
        return ER_BUS_BAD_XML;
    }

    return ER_OK;
}

//Parse XML
void TimeServiceIntrospectionNode::parseXml(xmlDocPtr doc)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    xmlNode* root_element = xmlDocGetRootElement(doc);

    for  (xmlNode* currentKey = root_element->children; currentKey != NULL; currentKey = currentKey->next) {

        if (currentKey->type != XML_ELEMENT_NODE) {

            continue;
        }

        const xmlChar* keyName = currentKey->name;

        //Check if description was found
        if (xmlStrEqual(keyName, (const xmlChar*) "description")) {

            const char* value = (currentKey->children != NULL) ? (char*) currentKey->children->content : "";
            m_ObjectDescription.assign(value);
            continue;
        }

        //Check if the child node was found
        if (xmlStrEqual(keyName, (const xmlChar*) "node")) {


            xmlChar* nodeName = xmlGetProp(currentKey, (const xmlChar*) "name");

            //Construct child node object path
            qcc::String childObjPath;
            constructChildObjectPath((char*)nodeName, childObjPath);

            m_ChildObjects.push_back(childObjPath);
            continue;
        }
    }
}

//Construct child object path
void TimeServiceIntrospectionNode::constructChildObjectPath(const qcc::String& childPath, qcc::String& fullObjPath)
{

    fullObjPath = m_ObjectPath;

    //Check if the last character of the resObjPath is not "/"
    if (fullObjPath.c_str()[fullObjPath.size() - 1] != '/') {

        fullObjPath.append("/");
    }

    fullObjPath.append(childPath);
}
