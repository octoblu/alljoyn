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

#ifndef ONBOARDINGCLIENTLISTENER_H_
#define ONBOARDINGCLIENTLISTENER_H_

#include <qcc/String.h>

namespace ajn {
namespace services {
/**
 * OnboardingClientListener
 *
 *  is a helper class used by an AllJoyn IoE client application to receive OnboardingService signal notification.
 * The user of the class need to implement virtual void ConnectionResultSignalReceived(...) function
 */

class OnboardingClientListener {
  public:
    /**
     * Destructor
     */
    virtual ~OnboardingClientListener() { }

    /**
     * ConnectionResultSignalReceived
     * Needs to be implemented by the user for the desired behaviour when receiving the ConnectResult Signal
     * @param connectionResultCode
     * @param connectionResultMessage
     */
    virtual void ConnectionResultSignalReceived(short connectionResultCode, const qcc::String& connectionResultMessage) = 0;
};
}
}



#endif /* ONBOARDINGCLIENTLISTENER_H_ */
