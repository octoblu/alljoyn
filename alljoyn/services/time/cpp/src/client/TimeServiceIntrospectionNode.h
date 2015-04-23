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

#ifndef INTROSPECTIONNODE_H_
#define INTROSPECTIONNODE_H_

#include <alljoyn/BusAttachment.h>
#include <vector>
#include <libxml/parser.h>
#include <libxml/tree.h>

namespace ajn {
namespace services {

/**
 * Utility class for introspection of the given objectPath and
 * parsing the IntrospectionXML to retrieve child object paths and the object description if defined.
 * The introspection is done with the org::allseen::Introspectable interface.
 */
class TimeServiceIntrospectionNode {

  public:

    /**
     * Constructor
     *
     * @param objectPath Object to be introspected and parsed
     */
    TimeServiceIntrospectionNode(qcc::String objectPath);

    /**
     * Destructor
     */
    ~TimeServiceIntrospectionNode();

    /**
     * Parse the object
     *
     * @param bus BusAttachment to use for introspection
     * @param busName Server bus name, hosting the object to be introspected
     * @param sessionId SID to be used for the introspection
     * @param language The language to be used for Introspection with Description
     *
     * @return status ER_OK if the introspection and parsing were successful
     */
    QStatus parse(BusAttachment* bus, const qcc::String& busName, ajn::SessionId sessionId, const qcc::String& language);

    /**
     * Returns ObjectDescription
     *
     * @param Object Description of the introspected object
     */
    const qcc::String& getObjectDescription();

    /**
     * Returns vector of the children object paths
     *
     * @return Vector of the object paths
     */
    const std::vector<qcc::String>& getChildObjects();

  private:

    /**
     * Object path to be introspected
     */
    qcc::String m_ObjectPath;

    /**
     * Object description of the parsed object
     */
    qcc::String m_ObjectDescription;

    /**
     * Vector of child object paths
     */
    std::vector<qcc::String> m_ChildObjects;

    /**
     * Introspect the object
     *
     * @param bus BusAttachment to use for introspection
     * @param busName Server bus name, hosting the object to be introspected
     * @param sessionId SID to be used for the introspection
     * @param Out variable. Result of the introspection
     * @param language The language to be used for Introspection with Description
     *
     * @return status ER_OK if the introspection was successful
     */
    QStatus introspect(BusAttachment* bus, const qcc::String& busName, ajn::SessionId sessionId, const qcc::String& language,
                       qcc::String& xml);

    /**
     * Unmarshal Introspection XML
     *
     * @param msgArg Argument to be unmarshalled
     * @param xml Out Variable. Resultant XML
     *
     * @return status
     */
    QStatus unmarshalXml(Message& msg, qcc::String& xml);

    /**
     * Creates XML document from the char*
     *
     * @return xmlDocPtr or NULL on failure
     */
    QStatus loadXml(const qcc::String& xml, xmlDocPtr* xmlDoc);

    /**
     * Parses the XML and search for child nodes and the objectDescription
     */
    void parseXml(xmlDocPtr doc);

    /**
     * Constructs full object path from the m_ObjectPath and the childPath
     *
     * @param childPath Child node object path
     * @param fullObjPath Out variable. Object path to be constructed
     */
    void constructChildObjectPath(const qcc::String& childPath, qcc::String& fullObjPath);
};

} /* namespace services */
} /* namespace ajn */

#endif /* INTROSPECTIONNODE_H_ */
