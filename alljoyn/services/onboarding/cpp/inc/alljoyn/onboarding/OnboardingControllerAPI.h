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

#ifndef _ONBOARDINGCONTROLLERAPI_H
#define _ONBOARDINGCONTROLLERAPI_H

#include <stdio.h>
#include <map>
#include <string>

#include <qcc/platform.h>
#include <qcc/String.h>

#include <alljoyn/BusAttachment.h>
#include <alljoyn/MsgArg.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/DBusStd.h>
#include <alljoyn/onboarding/Onboarding.h>
#include <alljoyn/Status.h>

namespace ajn {
namespace services {
/**
 *  OnboardingControllerAPI  interface class that is implemented  by the Application and controls the WIFI of the system.
 */

class OnboardingControllerAPI {

  public:

    /**
     * Destructor of OnboardingControllerAPI
     */
    virtual ~OnboardingControllerAPI() { }

    /**
     * ConfigureWiFi passing connection info to connect to WIFI
     * @param[in] SSID  of WIFI AP
     * @param[in] passphrase of WIFI AP in hex
     * @param[in] authType used by  WIFI AP
     * @param[out] status
     * @param[out] error
     * @param[out] errorMessage
     */
    virtual void ConfigureWiFi(qcc::String SSID, qcc::String passphrase, short authType, short& status, qcc::String& error, qcc::String& errorMessage) = 0;

    /**
     *	Connect to the WIFI using the ConfigureWiFi details supplied before
     */
    virtual void Connect() = 0;

    /**
     * GetScanInfo return a list of  OBScanInfo representing  WIFI scan info
     * @param[out] age time in minutes from the last scan
     * @param[out] scanInfoList list of OBScanInfo*
     * @param[out] scanListNumElements number of elements
     */
    virtual void GetScanInfo(unsigned short& age, OBScanInfo*& scanInfoList, size_t& scanListNumElements) = 0;

    /**
     *	Offboard disconnect from the WIFI
     */
    virtual void Offboard() = 0;

    /**
     * Getstate return the last state.
     */
    virtual short GetState() = 0;

    /**
     * GetLastError returns the last error
     * @return OBLastError
     */
    virtual const OBLastError& GetLastError() = 0;

  private:
};
}
}
#endif
