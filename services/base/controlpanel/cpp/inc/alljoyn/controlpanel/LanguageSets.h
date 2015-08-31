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

#ifndef LANGUAGESETS_H_
#define LANGUAGESETS_H_

#include <alljoyn/controlpanel/LanguageSet.h>
#include <map>

namespace ajn {
namespace services {

/**
 * Class to store the LanguageSets defined
 */
class LanguageSets {
  public:

    /**
     * Constructor for LanguageSets
     */
    LanguageSets();

    /**
     * Destructor for LanguageSets
     */
    virtual ~LanguageSets();

    /**
     * Add a LanguageSet
     * @param languageSetName - name of LanguageSet
     * @param languageSet - languageSet
     */
    static void add(qcc::String const& languageSetName, LanguageSet const& languageSet);

    /**
     * Get a LanguageSet
     * @param languageSetName - the name of the languageSet to get
     * @return the languageSet requested or NULL if it does not exist
     */
    static LanguageSet* get(qcc::String const& languageSetName);

  private:

    /**
     * The LanguageSets map storing the LanguageSets defined
     */
    static std::map<qcc::String, LanguageSet> languageSets;
};

} /* namespace services */
} /* namespace ajn */
#endif /* LANGUAGESETS_H_ */
