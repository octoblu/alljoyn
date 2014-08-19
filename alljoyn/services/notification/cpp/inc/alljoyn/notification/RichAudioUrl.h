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

#ifndef RICHAUDIOURL_H_
#define RICHAUDIOURL_H_

#include <qcc/String.h>

namespace ajn {
namespace services {

/**
 * Class to store RichAudio urls, a url per language
 */
class RichAudioUrl {

  public:

    /**
     * Constructor for RichAudioUrl
     * @param language Language of Audio Content
     * @param url      Url of Audio Content
     */
    RichAudioUrl(qcc::String const& language, qcc::String const& url);

    /**
     * Destructor for RichAudioUrl
     */
    ~RichAudioUrl() { };

    /**
     * Set Language for Audio Content
     * @param language
     */
    void setLanguage(qcc::String const& language);

    /**
     * Get Language for Audio Content
     * @return language
     */
    qcc::String const& getLanguage() const;

    /**
     * Set URL for Audio Content
     * @param url
     */
    void setUrl(qcc::String const& url);

    /**
     * Get URL for Audio Content
     * @return url
     */
    qcc::String const& getUrl() const;
  private:

    /**
     * Audio Content Language String
     */
    qcc::String m_Language;

    /**
     * Audio Content URL
     */
    qcc::String m_Url;
};
} //namespace services
} //namespace ajn

#endif /* RICHAUDIOURL_H_ */
