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

#ifndef TIMESERVICEOBJECTINTROSPECTOR_H_
#define TIMESERVICEOBJECTINTROSPECTOR_H_

#include <alljoyn/time/TimeServiceClientBase.h>
#include <alljoyn/Status.h>
#include <map>
#include <vector>

namespace ajn {
namespace services {

/**
 * This class introspects the object identified by the Object Path provided in the class constructor.
 * The introspection is done with the org::allseen::introspectable interface.
 */
class TimeServiceObjectIntrospector : public TimeServiceClientBase {

  public:

    /**
     * Constructor
     *
     * @param tsClient TimeServiceClient managing this object
     * @param objectPath Object path of the Time Service server side object
     */
    TimeServiceObjectIntrospector(const TimeServiceClient& tsClient, const qcc::String& objectPath);

    /**
     * Destructor
     */
    ~TimeServiceObjectIntrospector();

    /**
     * Retrieves array of description languages supported by the introspected object.
     *
     * @return vector of the description languages.
     * If the returned vector is empty, it means that the introspected object has no description.
     */
    const std::vector<qcc::String>& retrieveDescriptionLanguages();

    /**
     * Retrieve description of the introspected object.
     * The description is retrieved in the requested language. The language should be
     * one of those returned by the retrieveDescriptionLanguages().
     *
     * @param language Language of the retrieved object description
     *
     * @return Object description or empty string if no description was found
     */
    const qcc::String retrieveObjectDescription(const qcc::String& language);

  protected:

    /**
     * Introspect server BusObject and retrieve child objects identified by their object paths
     *
     * @param childObjects Out variable. This vector is populated with the child objects of the introspected Bus Object.
     *
     * @return status Whether succeeded to introspect and retrieve the child objects
     */
    QStatus retrieveChildObjects(std::vector<qcc::String>* childObjects);

  private:

    /**
     * Gets TRUE if description languages have been retrieved.
     * No need to retrieve them more than once
     */
    bool m_LangRetrieved;

    /**
     * Key: description language, value description in the language
     */
    std::map<qcc::String, qcc::String> m_ObjectDescription;

    /**
     * Description languages retrieved by the GetDescriptionLanguages of the org::allseen::introspectable
     * interface
     */
    std::vector<qcc::String> m_DescriptionLanguages;

    /**
     * Checks validity of the given descriptionLanguage
     *
     * @param language Language to be checked
     *
     * @return status ER_OK if the language is not qcc::String::Empty and in the descriptionLanguages vector
     */
    QStatus checkDescLanguageValidity(const qcc::String& language);

    /**
     * Unmarshal description languages and populate m_DescriptionLanguages Map
     *
     * @param replyMsg
     */
    void unmarshalDescriptionLanguages(Message& replyMsg);
};

} /* namespace services */
} /* namespace ajn */

#endif /* TIMESERVICEOBJECTINTROSPECTOR_H_ */
