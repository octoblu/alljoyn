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

#include <stdio.h>
#include <vector>
#include <alljoyn/onboarding/OnboardingService.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/onboarding/LogModule.h>

#define CHECK_RETURN(x) if ((status = x) != ER_OK) { return status; }
#define CHECK_BREAK(x) if ((status = x) != ER_OK) { break; }

using namespace ajn;
using namespace services;

static const char* ONBOARDING_INTERFACE_NAME = "org.alljoyn.Onboarding";
static const char* const ERROR_INVALID = "org.alljoyn.Error.InvalidValue";
static const char* const ERROR_OUTOFRANGE = "org.alljoyn.Error.OutOfRange";
static const char* const ERROR_SSID_EMPTY = "SSID was empty";
static const char* const ERROR_SSID_LONG = "Maximum SSID length is 32 characters";
static const char* const ERROR_AUTHTYPE_INVALID = "Authentication protocol was invalid";
static const char* const ERROR_PASSPHRASE_EMPTY = "Passphrase was empty";
static const char* const ERROR_PASSPHRASE_NOTEMPTY = "Passphrase should be empty for open authentication";
static const char* const ERROR_PASSPHRASE_SHORT = "Minimum passphrase length is 8 characters";
static const char* const ERROR_PASSPHRASE_LONG = "Maximum passphrase length is 64 characters";
static const char* const ERROR_WEPKEY_INVALID = "Invalid WEP key length";

OnboardingService::OnboardingService(ajn::BusAttachment& bus, OnboardingControllerAPI& pOnboardingControllerAPI) :
    BusObject("/Onboarding"), m_BusAttachment(&bus), m_OnboardingController(pOnboardingControllerAPI)
{
    QCC_DbgTrace(("In OnboardingService basic Constructor"));
}

QStatus OnboardingService::Register()
{
    QCC_DbgTrace(("In OnboardingService Register"));
    QStatus status = ER_OK;

    InterfaceDescription* intf = NULL;
    intf = const_cast<InterfaceDescription*>(m_BusAttachment->GetInterface(ONBOARDING_INTERFACE_NAME));
    if (!intf) {
        CHECK_RETURN(m_BusAttachment->CreateInterface(ONBOARDING_INTERFACE_NAME, intf, AJ_IFC_SECURITY_REQUIRED));
        if (!intf) {
            return ER_FAIL;
        }

        CHECK_RETURN(intf->AddMethod("ConfigureWiFi", "ssn", "n", "SSID,passphrase,authType,status", 0));
        CHECK_RETURN(intf->AddMethod("Connect", NULL, NULL, NULL, 0));
        CHECK_RETURN(intf->AddMemberAnnotation("Connect", org::freedesktop::DBus::AnnotateNoReply, "true"));
        CHECK_RETURN(intf->AddMethod("Offboard", NULL, NULL, NULL, 0));
        CHECK_RETURN(intf->AddMemberAnnotation("Offboard", org::freedesktop::DBus::AnnotateNoReply, "true"));
        CHECK_RETURN(intf->AddMethod("GetScanInfo", NULL, "qa(sn)", "age,scanList", 0));
        CHECK_RETURN(intf->AddProperty("LastError", "(ns)", PROP_ACCESS_READ));
        CHECK_RETURN(intf->AddProperty("State", "n", PROP_ACCESS_READ));
        CHECK_RETURN(intf->AddProperty("Version", "q", PROP_ACCESS_READ));
        CHECK_RETURN(intf->AddSignal("ConnectionResult", "(ns)", "signalArg0", 0));
        intf->Activate();
    } //if (!intf)

    //Add the handlers
    CHECK_RETURN(AddInterface(*intf, ANNOUNCED));
    CHECK_RETURN(AddMethodHandler(intf->GetMember("ConfigureWiFi"),
                                  static_cast<MessageReceiver::MethodHandler>(&OnboardingService::ConfigureWiFiHandler)));
    CHECK_RETURN(AddMethodHandler(intf->GetMember("Connect"),
                                  static_cast<MessageReceiver::MethodHandler>(&OnboardingService::ConnectHandler)));
    CHECK_RETURN(AddMethodHandler(intf->GetMember("Offboard"),
                                  static_cast<MessageReceiver::MethodHandler>(&OnboardingService::OffboardHandler)));
    CHECK_RETURN(AddMethodHandler(intf->GetMember("GetScanInfo"),
                                  static_cast<MessageReceiver::MethodHandler>(&OnboardingService::GetScanInfoHandler)));

    return status;
}

static uint8_t A2H(char hex, QStatus* status)
{
    if (hex >= '0' && hex <= '9') {
        return hex - '0';
    }

    hex |= 0x20;
    if (hex >= 'a' && hex <= 'f') {
        return 10 + hex - 'a';
    } else if (hex >= 'A' && hex <= 'F') {
        return 10 + hex - 'A';
    } else {
        *status = ER_FAIL;
        return 0;
    }
}

QStatus HexToRaw(const char* hex, size_t hexLen, char* raw, size_t rawLen)
{
    QStatus status = ER_OK;
    char* p = (char*)raw;
    size_t sz = hexLen ? hexLen : strlen(hex);
    size_t i;

    /*
     * Length of encoded hex must be an even number
     */
    if (sz & 1) {
        return ER_FAIL;
    }
    if (rawLen < (sz / 2)) {
        return ER_FAIL;
    }
    for (i = 0; (i < sz) && (status == ER_OK); i += 2, hex += 2) {
        *p++ = (A2H(hex[0], &status) << 4) | A2H(hex[1], &status);
    }
    return status;
}

void OnboardingService::ConfigureWiFiHandler(const ajn::InterfaceDescription::Member* member, ajn::Message& msg)
{
    QCC_DbgTrace(("In OnboardingService ConfigureWiFiHandler"));
    const ajn::MsgArg* args;
    size_t numArgs;
    QStatus status = ER_OK;
    char raw[512];

    msg->GetArgs(numArgs, args);
    do {
        if (numArgs != 3) {
            break;
        }

        char* SSID;
        char*passphrase;
        short authType;
        CHECK_BREAK(args[0].Get("s", &SSID));
        CHECK_BREAK(args[1].Get("s", &passphrase));
        CHECK_BREAK(args[2].Get("n", &authType));

        short configureWifiStatus = 0;
        qcc::String error;
        qcc::String errorMessage;

        qcc::String strSSID(SSID);
        qcc::String strPass(passphrase);

        // Platform independent validation and logic
        if (strSSID.empty()) {
            error = ERROR_INVALID;
            errorMessage = ERROR_SSID_EMPTY;
        } else if (strSSID.size() > MAX_SSID_SIZE) {
            error = ERROR_INVALID;
            errorMessage = ERROR_SSID_LONG;
        }

        if (authType < WPA2_AUTO || authType > WPS) {
            error = ERROR_OUTOFRANGE;
            errorMessage = ERROR_AUTHTYPE_INVALID;
        }

        if (OPEN == authType) {
            if (!strPass.empty()) {
                error = ERROR_INVALID;
                errorMessage = ERROR_PASSPHRASE_NOTEMPTY;
            }
        } else if (WEP == authType) {
            if (strPass.empty()) {
                error = ERROR_INVALID;
                errorMessage = ERROR_PASSPHRASE_EMPTY;
            } else {
                if (!isValidWEPKey(strPass)) {
                    error = ERROR_INVALID;
                    errorMessage = ERROR_WEPKEY_INVALID;
                }
            }
        } else if (ANY != authType) {
            if (strPass.empty()) {
                error = ERROR_INVALID;
                errorMessage = ERROR_PASSPHRASE_EMPTY;
            } else if (strPass.size() < MIN_PSK_SIZE) {
                error = ERROR_INVALID;
                errorMessage = ERROR_PASSPHRASE_SHORT;
            } else if (strPass.size() / 2 > MAX_PSK_SIZE) {
                error = ERROR_INVALID;
                errorMessage = ERROR_PASSPHRASE_LONG;
            }
        }

        if (error.empty() && errorMessage.empty()) {
            if (WEP != authType) {
                // passcode is hexa encoded by client, decode
                size_t rawLength = strPass.length() / 2 + 1;
                CHECK_BREAK(HexToRaw(strPass.c_str(), strPass.length(), raw, rawLength));
                raw[strPass.length() / 2] = '\0';

                // save decoded string in strPass
                strPass.assign(raw);
            }
            m_OnboardingController.ConfigureWiFi(SSID, strPass, authType, configureWifiStatus, error, errorMessage);

            MsgArg retArgs[1];
            CHECK_BREAK(retArgs[0].Set("n", configureWifiStatus));
            MethodReply(msg, retArgs, 1);
        } else {
            MethodReply(msg, error.c_str(), errorMessage.c_str());
        }
        return;
    } while (0);

    MethodReply(msg, ER_INVALID_DATA);

}

void OnboardingService::Check_MethodReply(const Message& msg, const MsgArg* args, size_t numArgs)
{
    QStatus status = ER_OK;
    //check it the ALLJOYN_FLAG_NO_REPLY_EXPECTED exists if so send response
    if (!(msg->GetFlags() & ALLJOYN_FLAG_NO_REPLY_EXPECTED)) {
        status = MethodReply(msg, args, numArgs);
        if (status != ER_OK) {
            QCC_LogError(status, ("Method did not execute successfully."));
        }
    }
}

void OnboardingService::Check_MethodReply(const Message& msg, QStatus status)
{
    //check it the ALLJOYN_FLAG_NO_REPLY_EXPECTED exists if so send response ER_INVALID_DATA
    if (!(msg->GetFlags() & ALLJOYN_FLAG_NO_REPLY_EXPECTED)) {
        status = MethodReply(msg, status);
        if (status != ER_OK) {
            QCC_LogError(status, ("Method did not execute successfully."));
        }
    }
}

void OnboardingService::ConnectHandler(const ajn::InterfaceDescription::Member* member, ajn::Message& msg)
{
    QCC_DbgTrace(("In OnboardingService ConnectHandler"));
    const ajn::MsgArg* args = 0;
    size_t numArgs = 0;
    msg->GetArgs(numArgs, args);
    do {
        if (numArgs != 0) {
            break;
        }
        MsgArg*args = NULL;
        Check_MethodReply(msg, args, 0);
        m_OnboardingController.Connect();
        return;
    } while (0);

    Check_MethodReply(msg, ER_INVALID_DATA);
}

void OnboardingService::OffboardHandler(const ajn::InterfaceDescription::Member* member, ajn::Message& msg)
{
    QCC_DbgTrace(("In OnboardingService OffboardHandler"));
    const ajn::MsgArg* args = 0;
    size_t numArgs = 0;
    do {
        msg->GetArgs(numArgs, args);
        if (numArgs != 0) {
            break;
        }
        MsgArg*args = NULL;
        Check_MethodReply(msg, args, 0);
        m_OnboardingController.Offboard();
        return;
    } while (0);

    Check_MethodReply(msg, ER_INVALID_DATA);
}

void OnboardingService::GetScanInfoHandler(const ajn::InterfaceDescription::Member* member, ajn::Message& msg)
{
    QCC_DbgTrace(("In OnboardingService GetScanInfoHandler"));
    const ajn::MsgArg* args = NULL;
    size_t numArgs = 0;
    QStatus status = ER_OK;
    msg->GetArgs(numArgs, args);
    do {
        if (numArgs != 0) {
            break;
        }
        unsigned short age = 0;
        size_t numberOfElements = 0;
        OBScanInfo* scanInfoList = NULL; // no need to delete scanInfoList it will be delete by the MsgArgs
        m_OnboardingController.GetScanInfo(age, scanInfoList, numberOfElements);
        if (!(scanInfoList != NULL && numberOfElements > 0)) {
            Check_MethodReply(msg, ER_FAIL);
            return;
        }

        std::vector<MsgArg> scanInfoArgs(numberOfElements);
        for (size_t i = 0; i < numberOfElements; i++) {
            CHECK_BREAK(scanInfoArgs[i].Set("(sn)", scanInfoList[i].SSID.c_str(), scanInfoList[i].authType));
        }
        CHECK_BREAK(status); //In case a break came from the for loop we want to break again.
        MsgArg retArgs[2];
        CHECK_BREAK(retArgs[0].Set("q", age));
        CHECK_BREAK(retArgs[1].Set("a(sn)", numberOfElements, scanInfoArgs.data()));
        Check_MethodReply(msg, retArgs, 2);
        return;

    } while (0);

    Check_MethodReply(msg, ER_INVALID_DATA);
}

QStatus OnboardingService::Get(const char* ifcName, const char* propName, MsgArg& val)
{
    QCC_DbgTrace(("In OnboardingService GetProperty"));
    QStatus status = ER_OK;
// Check the requested property and return the value if it exists
    if (0 == strcmp(ifcName, ONBOARDING_INTERFACE_NAME)) {
        if (0 == strcmp("Version", propName)) {
            status = val.Set("q", 1);
        } else if (0 == strcmp("State", propName)) {
            status = val.Set("q", m_OnboardingController.GetState());
        } else if (0 == strcmp("LastError", propName)) {
            OBLastError lastError = m_OnboardingController.GetLastError();
            QCC_DbgHLPrintf(("Last State: %d lastErrorMessage: %s", lastError.validationState, lastError.message.c_str()));
            status = val.Set("(ns)", lastError.validationState, lastError.message.c_str());
        } else {
            status = ER_BUS_NO_SUCH_PROPERTY;
        }
    } else {
        status = ER_BUS_NO_SUCH_PROPERTY;
    }
    return status;
}
