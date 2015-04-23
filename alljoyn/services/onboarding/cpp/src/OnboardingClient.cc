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

#include <alljoyn/onboarding/OnboardingClient.h>
#include <alljoyn/AllJoynStd.h>
#include <alljoyn/onboarding/LogModule.h>
#include "HandleOnboardingSignals.h"


#define CHECK_RETURN_STATUS(x) if ((status = x) != ER_OK) { return status; }
#define CHECK_RETURN(x) if ((status = x) != ER_OK) { return; }
#define CHECK_BREAK(x) if ((status = x) != ER_OK) { break; }

using namespace ajn;
using namespace services;

static const char* ONBOARDING_OBJECT_PATH = "/Onboarding";
static const char* ONBOARDING_INTERFACE_NAME = "org.alljoyn.Onboarding";

// A helper class for handling signals.
// This is not in the h file so that it is not exposed to the OnboardinfClient user
static HandleOnboardingSignals* s_signalHandler = NULL;

OnboardingClient::OnboardingClient(BusAttachment& bus, OnboardingClientListener& listener) :
    m_BusAttachment(&bus)
{
    QCC_DbgTrace(("In OnboardingClient basic Constructor"));

    QStatus status = ER_OK;
    const InterfaceDescription* getIface = NULL;
    getIface = m_BusAttachment->GetInterface(ONBOARDING_INTERFACE_NAME);
    if (!getIface) {
        InterfaceDescription* createIface = NULL;
        status = m_BusAttachment->CreateInterface(ONBOARDING_INTERFACE_NAME, createIface, AJ_IFC_SECURITY_REQUIRED);
        if (createIface) {
            CHECK_RETURN(createIface->AddMethod("ConfigureWiFi", "ssn", "n", "SSID,passphrase,authType,status", 0));
            CHECK_RETURN(createIface->AddMethod("Connect", NULL, NULL, NULL, 0));
            CHECK_RETURN(createIface->AddMemberAnnotation("Connect", org::freedesktop::DBus::AnnotateNoReply, "true"));
            CHECK_RETURN(createIface->AddMethod("Offboard", NULL, NULL, NULL, 0));
            CHECK_RETURN(createIface->AddMemberAnnotation("Offboard", org::freedesktop::DBus::AnnotateNoReply, "true"));
            CHECK_RETURN(createIface->AddMethod("GetScanInfo", NULL, "qa(sn)", "age,scanList", 0));
            CHECK_RETURN(createIface->AddProperty("LastError", "(ns)", PROP_ACCESS_READ));
            CHECK_RETURN(createIface->AddProperty("State", "n", PROP_ACCESS_READ));
            CHECK_RETURN(createIface->AddProperty("Version", "q", PROP_ACCESS_READ));
            CHECK_RETURN(createIface->AddSignal("ConnectionResult", "(ns)", "signalArg0", 0));
            createIface->Activate();

            // set signal handling
            const ajn::InterfaceDescription::Member* connectionResultSignalMethod = createIface->GetMember("ConnectionResult");

            s_signalHandler = new HandleOnboardingSignals(&bus, &listener);
            s_signalHandler->RegisterConnectionResultSignalHandler(connectionResultSignalMethod);
        }
    }
}

OnboardingClient::~OnboardingClient()
{
    QCC_DbgTrace(("In OnboardingClient Destructor"));

    if (s_signalHandler) {
        delete s_signalHandler;
        s_signalHandler = NULL;
    }
}

QStatus OnboardingClient::GetScanInfo(const char* busName, unsigned short& age, ScanInfos& scanInfos,
                                      ajn::SessionId sessionId)
{
    QCC_DbgTrace(("In OnboardingClient GetScanInfo"));
    QStatus status = ER_OK;

    const InterfaceDescription* ifc = m_BusAttachment->GetInterface(ONBOARDING_INTERFACE_NAME);
    if (!ifc) {
        return ER_FAIL;
    }

    ProxyBusObject*proxyBusObj = new ProxyBusObject(*m_BusAttachment, busName, ONBOARDING_OBJECT_PATH, sessionId);
    if (!proxyBusObj) {
        return ER_FAIL;
    }
    status = proxyBusObj->AddInterface(*ifc);
    if (status != ER_OK) {
        delete proxyBusObj;
        return status;
    }

    Message replyMsg(*m_BusAttachment);
    status = proxyBusObj->MethodCall(ONBOARDING_INTERFACE_NAME, "GetScanInfo", NULL, 0, replyMsg);
    if (status != ER_OK) {
        delete proxyBusObj;
        proxyBusObj = NULL;
        if (status == ER_BUS_REPLY_IS_ERROR_MESSAGE) {
#if !defined(NDEBUG)
            qcc::String errorMessage;
            const char* errorName = replyMsg->GetErrorName(&errorMessage);
#endif
            QCC_DbgHLPrintf(("GetScanInfo errorName: %s errorMessage: %s", errorName ? errorName : "", errorMessage.c_str()));
        }
        return status;
    }

    const ajn::MsgArg* returnArgs;
    size_t numArgs;
    replyMsg->GetArgs(numArgs, returnArgs);
    if (numArgs == 2) {
        status = returnArgs[0].Get("q", &age);
        MsgArg* scanInfoEntries;
        size_t scanInfoNum;
        status = returnArgs[1].Get("a(sn)", &scanInfoNum, &scanInfoEntries);
        if (status == ER_OK && scanInfoNum > 0) {
            for (unsigned int i = 0; i < scanInfoNum; i++) {
                char* SSID;
                short authType;
                status = scanInfoEntries[i].Get("(sn)", &SSID, &authType);
                if (status != ER_OK) {
                    break;
                }
                OBScanInfo info;
                info.authType = (OBAuthType) authType;
                info.SSID.assign(SSID);
                scanInfos.push_back(info);
            }            //end of for
        }
    } else {
        status = ER_BAD_ARG_COUNT;
    }

    delete proxyBusObj;
    return status;
}

QStatus OnboardingClient::ConfigureWiFi(const char* busName, const OBInfo& oBInfo, short& resultStatus,
                                        ajn::SessionId sessionId)
{
    QCC_DbgTrace(("In OnboardingClient ConfigureWiFi"));

    QStatus status = ER_OK;

    const InterfaceDescription* ifc = m_BusAttachment->GetInterface(ONBOARDING_INTERFACE_NAME);
    if (!ifc) {
        return ER_FAIL;
    }

    ProxyBusObject*proxyBusObj = new ProxyBusObject(*m_BusAttachment, busName, ONBOARDING_OBJECT_PATH, sessionId);
    if (!proxyBusObj) {
        return ER_FAIL;
    }
    status = proxyBusObj->AddInterface(*ifc);
    if (status != ER_OK) {
        delete proxyBusObj;
        return status;
    }

    MsgArg args[3];
    Message replyMsg(*m_BusAttachment);
    do {
        CHECK_BREAK(args[0].Set("s", oBInfo.SSID.c_str()));
        CHECK_BREAK(args[1].Set("s", oBInfo.passcode.c_str()));
        CHECK_BREAK(args[2].Set("n", oBInfo.authType));

        CHECK_BREAK(proxyBusObj->MethodCall(ONBOARDING_INTERFACE_NAME, "ConfigureWiFi", args, 3, replyMsg));
    } while (0);
    if (status != ER_OK) {
        delete proxyBusObj;
        proxyBusObj = NULL;
        if (status == ER_BUS_REPLY_IS_ERROR_MESSAGE) {
#if !defined(NDEBUG)
            qcc::String errorMessage;
            const char* errorName = replyMsg->GetErrorName(&errorMessage);
#endif
            QCC_DbgHLPrintf(("ConfigureWiFi errorName: %s errorMessage: %s", errorName ? errorName : "", errorMessage.c_str()));
        }
        return status;
    }
    const ajn::MsgArg* returnArgs;
    size_t numArgs;
    replyMsg->GetArgs(numArgs, returnArgs);
    if (numArgs == 1) {
        status = returnArgs[0].Get("n", &resultStatus);
    } else {
        status = ER_BAD_ARG_COUNT;
    }

    delete proxyBusObj;
    return status;

}

QStatus OnboardingClient::ConnectTo(const char* busName, ajn::SessionId sessionId)
{
    QCC_DbgTrace(("In OnboardingClient ConnectTo"));

    QStatus status = ER_OK;

    const InterfaceDescription* ifc = m_BusAttachment->GetInterface(ONBOARDING_INTERFACE_NAME);
    if (!ifc) {
        return ER_FAIL;
    }

    ProxyBusObject*proxyBusObj = new ProxyBusObject(*m_BusAttachment, busName, ONBOARDING_OBJECT_PATH, sessionId);
    if (!proxyBusObj) {
        return ER_FAIL;
    }
    status = proxyBusObj->AddInterface(*ifc);
    if (status != ER_OK) {
        delete proxyBusObj;
        return status;
    }
    status = proxyBusObj->MethodCall(ONBOARDING_INTERFACE_NAME, "Connect", NULL, 0);
    delete proxyBusObj;
    return status;
}

QStatus OnboardingClient::OffboardFrom(const char* busName, ajn::SessionId sessionId)
{
    QCC_DbgTrace(("In OnboardingClient OffboardFrom"));
    QStatus status = ER_OK;

    const InterfaceDescription* ifc = m_BusAttachment->GetInterface(ONBOARDING_INTERFACE_NAME);
    if (!ifc) {
        return ER_FAIL;
    }

    ProxyBusObject*proxyBusObj = new ProxyBusObject(*m_BusAttachment, busName, ONBOARDING_OBJECT_PATH, sessionId);
    if (!proxyBusObj) {
        return ER_FAIL;
    }
    status = proxyBusObj->AddInterface(*ifc);
    if (status != ER_OK) {
        delete proxyBusObj;
        return status;
    }
    status = proxyBusObj->MethodCall(ONBOARDING_INTERFACE_NAME, "Offboard", NULL, 0);
    delete proxyBusObj;
    return status;

}

QStatus OnboardingClient::GetVersion(const char* busName, int& version, ajn::SessionId sessionId)
{
    QCC_DbgTrace(("In OnboardingClient GetVersion"));
    QStatus status = ER_OK;

    const InterfaceDescription* ifc = m_BusAttachment->GetInterface(ONBOARDING_INTERFACE_NAME);
    if (!ifc) {
        return ER_FAIL;
    }

    ProxyBusObject*proxyBusObj = new ProxyBusObject(*m_BusAttachment, busName, ONBOARDING_OBJECT_PATH, sessionId);
    if (!proxyBusObj) {
        return ER_FAIL;
    }
    MsgArg arg;
    if (ER_OK == proxyBusObj->AddInterface(*ifc)) {
        status = proxyBusObj->GetProperty(ONBOARDING_INTERFACE_NAME, "Version", arg);
        if (ER_OK == status) {
            version = arg.v_variant.val->v_uint16;
        }
    }
    delete proxyBusObj;
    proxyBusObj = NULL;
    return status;
}

QStatus OnboardingClient::GetState(const char* busName, short& state, ajn::SessionId sessionId)
{
    QCC_DbgTrace(("In OnboardingClient GetState"));

    QStatus status = ER_OK;

    const InterfaceDescription* ifc = m_BusAttachment->GetInterface(ONBOARDING_INTERFACE_NAME);
    if (!ifc) {
        return ER_FAIL;
    }

    ProxyBusObject*proxyBusObj = new ProxyBusObject(*m_BusAttachment, busName, ONBOARDING_OBJECT_PATH, sessionId);
    if (!proxyBusObj) {
        return ER_FAIL;
    }
    MsgArg arg;
    if (ER_OK == proxyBusObj->AddInterface(*ifc)) {
        status = proxyBusObj->GetProperty(ONBOARDING_INTERFACE_NAME, "State", arg);
        if (ER_OK == status) {
            state = arg.v_variant.val->v_int16;
        }
    }
    delete proxyBusObj;
    proxyBusObj = NULL;
    return status;
}

QStatus OnboardingClient::GetLastError(const char* busName, OBLastError& lastError, ajn::SessionId sessionId)
{
    QCC_DbgTrace(("In OnboardingClient GetLastError"));
    QStatus status = ER_OK;

    const InterfaceDescription* ifc = m_BusAttachment->GetInterface(ONBOARDING_INTERFACE_NAME);
    if (!ifc) {
        return ER_FAIL;
    }

    ProxyBusObject*proxyBusObj = new ProxyBusObject(*m_BusAttachment, busName, ONBOARDING_OBJECT_PATH, sessionId);
    if (!proxyBusObj) {
        return ER_FAIL;
    }
    MsgArg arg;
    if (ER_OK == proxyBusObj->AddInterface(*ifc)) {
        status = proxyBusObj->GetProperty(ONBOARDING_INTERFACE_NAME, "LastError", arg);
        if (ER_OK == status) {
            char* tempMesage;
            short errorCode;
            status = arg.Get("(ns)", &errorCode, &tempMesage);
            if (status == ER_OK) {
                lastError.message.assign(tempMesage);
                lastError.validationState = errorCode;
            }
        }
    }
    delete proxyBusObj;
    proxyBusObj = NULL;
    return status;

}
