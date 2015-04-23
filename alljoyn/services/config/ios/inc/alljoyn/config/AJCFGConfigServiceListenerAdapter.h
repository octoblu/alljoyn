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

#ifndef ALLJOYN_CONFIG_OBJC_AJCFGCONFIGSERVICELISTENERADAPTER_H
#define ALLJOYN_CONFIG_OBJC_AJCFGCONFIGSERVICELISTENERADAPTER_H

#import <alljoyn/config/ConfigService.h>
#import "AJCFGConfigServiceListener.h"
#import "alljoyn/about/AJNConvertUtil.h"

/**
 AJCFGConfigServiceListenerImplAdapter enable bind the C++ ConfigService Listener API with an objective-c ConfigService listener
 */
class AJCFGConfigServiceListenerImplAdapter : public ajn::services::ConfigService::Listener {
public:
	AJCFGConfigServiceListenerImplAdapter(id <AJCFGConfigServiceListener> configServiceListener)
	{
		qcsConfigServiceListener = configServiceListener;
	}
	/**
     Restart of the device.
     @return ER_OK if successful.
	 */
	virtual QStatus Restart()
	{
		return [qcsConfigServiceListener restart];
	}
    
	/**
     Factory reset of the device ,return to default values including password!
     @return ER_OK if successful.
	 */
	virtual QStatus FactoryReset()
	{
		return [qcsConfigServiceListener factoryReset];
	}
    
	/**
     Receive Passphrase info and persist it.
     @param daemonRealm Daemon realm to persist.
     @param passcodeSize passcode size.
     @param passcode passcode content.
     @return ER_OK if successful.
	 */
	virtual QStatus SetPassphrase(const char *daemonRealm, size_t passcodeSize, const char *passcode, ajn::SessionId sessionId)
	{
		NSString *passphrase = [[NSString alloc] initWithBytes:passcode length:passcodeSize encoding:NSASCIIStringEncoding];
        
		return [qcsConfigServiceListener setPassphrase:[AJNConvertUtil convertConstCharToNSString:daemonRealm] withPasscode:passphrase];
	}

    
	virtual ~AJCFGConfigServiceListenerImplAdapter()
	{
	}
    
private:
	id <AJCFGConfigServiceListener> qcsConfigServiceListener;
};

#endif //ALLJOYN_CONFIG_OBJC_AJCFGCONFIGSERVICELISTENERADAPTER_H
