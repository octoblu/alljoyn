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

#import <Foundation/Foundation.h>
#import "alljoyn/onboarding/OnboardingClientListener.h"
#import "AJOBOnboardingClientListener.h"

/**
 AJOBOnboardingClientListenerAdapter enable bind the C++ OnboardingClientListener API with an objective-c AJOBOnboardingClientListener
 */
class AJOBOnboardingClientListenerAdapter : public ajn::services::OnboardingClientListener {
public:
    // Handle to the objective-c AJOBOnboardingClientListener

    id<AJOBOnboardingClientListener> ajOnboardingClientListener;
    
    /**
     Constructor
     */
    AJOBOnboardingClientListenerAdapter(id <AJOBOnboardingClientListener>);
    
    /**
     Destructor
     */
    ~AJOBOnboardingClientListenerAdapter();

    /**
     * ConnectionResultSignalReceived
     * Forward the signal to the objective-c listener
     */
    
    void ConnectionResultSignalReceived(short connectionResultCode, const qcc::String& connectionResultMessage);
};