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

#ifndef LANGUAGESET_H_
#define LANGUAGESET_H_

#include <qcc/String.h>
#include <vector>

namespace ajn {
namespace services {

/**
 * Class used to define a LanguageSet
 */
class LanguageSet {

  public:

    /**
     * Constructor for LanguageSet class
     * @param languageSetName - name of LanguageSet
     */
    LanguageSet(qcc::String const& languageSetName);

    /**
     * Destructor for LanguageSet class
     */
    virtual ~LanguageSet();

    /**
     * Get the LanguageSetName
     * @return LanguageSetName
     */
    const qcc::String& getLanguageSetName() const;

    /**
     * Get the number of Languages defined
     * @return number of Languages
     */
    const size_t getNumLanguages() const;

    /**
     * Add a language to the LanguageSet
     * @param language - language to Add
     */
    void addLanguage(qcc::String const& language);

    /**
     *
     * Get the Languages defined in the LanguageSet
     * @return languages vector
     */
    const std::vector<qcc::String>& getLanguages() const;

  private:

    /**
     * Name of LanguageSet
     */
    qcc::String m_LanguageSetName;

    /**
     * vector to store Languages of LanguageSet
     */
    std::vector<qcc::String> m_Languages;
};

} /* namespace services */
} /* namespace ajn */
#endif /* LANGUAGESET_H_ */
