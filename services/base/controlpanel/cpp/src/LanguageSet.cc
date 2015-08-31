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

#include <alljoyn/controlpanel/LanguageSet.h>
#include <algorithm>

namespace ajn {
namespace services {

LanguageSet::LanguageSet(qcc::String const& languageSetName) :
    m_LanguageSetName(languageSetName)
{
}

LanguageSet::~LanguageSet()
{
}

const qcc::String& LanguageSet::getLanguageSetName() const
{
    return m_LanguageSetName;
}

const size_t LanguageSet::getNumLanguages() const
{
    return m_Languages.size();
}

void LanguageSet::addLanguage(qcc::String const& language)
{
    if (std::find(m_Languages.begin(), m_Languages.end(), language) == m_Languages.end()) {
        m_Languages.push_back(language);
    }
}

const std::vector<qcc::String>& LanguageSet::getLanguages() const
{
    return m_Languages;
}
} /* namespace services */
} /* namespace ajn */
