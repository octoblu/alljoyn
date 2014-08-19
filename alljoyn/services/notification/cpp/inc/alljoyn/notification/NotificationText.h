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

#ifndef NOTIFICATIONTEXT_H_
#define NOTIFICATIONTEXT_H_

#include <qcc/String.h>

namespace ajn {
namespace services {

/**
 * Class NotificationText is used to represent the actual text message of the notification
 * Made up of language and text values
 */
class NotificationText {

  public:

    /**
     * Constructor for NotificationText
     * @param language Language of Notification
     * @param text     Text of Notification
     */
    NotificationText(qcc::String const& language, qcc::String const& text);

    /**
     * Destructor for NotificationText
     */
    ~NotificationText() { };

    /**
     * Set Language for Notification
     * @param language
     */
    void setLanguage(qcc::String const& language);

    /**
     * Get Language for Notification
     * @return language
     */
    qcc::String const& getLanguage() const;

    /**
     * Set Text for Notification
     * @param text
     */
    void setText(qcc::String const& text);

    /**
     * Get Text for Notification
     * @return text
     */
    qcc::String const& getText() const;

  private:

    /**
     * Notification Language
     */
    qcc::String m_Language;

    /**
     * Notification Text
     */
    qcc::String m_Text;
};
} //namespace services
} //namespace ajn

#endif /* NOTIFICATIONTEXT_H_ */
