/******************************************************************************
 * Copyright (c) 2013-2014, AllSeen Alliance. All rights reserved.
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

#include <signal.h>
#include <iostream>
#include <sstream>
#include <alljoyn/PasswordManager.h>
#include <alljoyn/notification/NotificationService.h>
#include <alljoyn/notification/NotificationSender.h>
#include <alljoyn/notification/NotificationEnums.h>
#include <alljoyn/notification/NotificationText.h>
#include <alljoyn/notification/RichAudioUrl.h>
#include "../common/NotificationReceiverTestImpl.h"
#include "CommonSampleUtil.h"
#include "TestFunction.h"
#include <alljoyn/services_common/GuidUtil.h>
#include <alljoyn/services_common/LogModulesNames.h>

#define SERVICE_PORT 900

using namespace ajn;
using namespace services;
using namespace qcc;

// Global members
NotificationService* Service = 0;
NotificationSender* Sender = 0;
NotificationReceiverTestImpl* Receiver = 0;

BusAttachment* testBus = 0;
AboutPropertyStoreImpl* propertyStoreImpl = 0;
CommonBusListener* notificationBusListener = 0;
NotificationMessageType messageType =  UNSET;
std::vector<NotificationText> vecMessages;
std::vector<RichAudioUrl> richAudioUrl;
qcc::String richIconUrl = "";
qcc::String richIconObjectPath = "";
qcc::String richAudioObjectPath = "";
qcc::String controlPanelServiceObjectPath = "";
std::map<qcc::String, qcc::String> customAttributes;
bool didInitSend = false;
bool didInitReceive = false;
uint16_t ttl = 0;
int32_t sleepTime;
static volatile sig_atomic_t s_interrupt = false;

void resetParams()
{
    customAttributes.clear();
    vecMessages.clear();
    richAudioUrl.clear();
    richIconUrl = "";
    richIconObjectPath = "";
    richAudioObjectPath = "";
    controlPanelServiceObjectPath = "";
    ttl = 0;
    messageType = UNSET;
}

/*
 * Begin Function functions
 */

bool createService(std::map<qcc::String, qcc::String>& params)
{
    // Initialize Service object and sent it Notification Receiver object
    if (!testBus) {
        testBus = CommonSampleUtil::prepareBusAttachment();
        if (testBus == NULL) {
            std::cout << "Could not initialize BusAttachment." << std::endl;
            return 1;
        }
    }
    Service = NotificationService::getInstance();
//set Daemon passowrd only for bundled app
#ifdef QCC_USING_BD
    PasswordManager::SetCredentials("ALLJOYN_PIN_KEYX", "000000");
#endif
    std::cout << "Service Created" << std::endl;
    return true;
}

bool initSend(std::map<qcc::String, qcc::String>& params)
{
    if (!notificationBusListener) {
        notificationBusListener = new CommonBusListener();
    }

    if (!propertyStoreImpl) {
        propertyStoreImpl = new AboutPropertyStoreImpl();
    }

    qcc::String deviceid;
    GuidUtil::GetInstance()->GetDeviceIdString(&deviceid);
    qcc::String appid;
    GuidUtil::GetInstance()->GenerateGUID(&appid);

    DeviceNamesType deviceNames;
    deviceNames.insert(std::pair<qcc::String, qcc::String>("en", params["device_name"]));

    QStatus status;
    status = CommonSampleUtil::fillPropertyStore(propertyStoreImpl, appid, params["app_name"].c_str(),
                                                 deviceid, deviceNames);
    if (status != ER_OK) {
        std::cout << "Could not fill PropertyStore." << std::endl;
        return false;
    }

    status = CommonSampleUtil::prepareAboutService(testBus, propertyStoreImpl,
                                                   notificationBusListener, SERVICE_PORT);
    if (status != ER_OK) {
        std::cout << "Could not set up the AboutService." << std::endl;
        return false;
    }

    Sender = Service->initSend(testBus, propertyStoreImpl);
    if (!Sender) {
        std::cout << "Could not initialize the sender";
        return false;
    }

    status = CommonSampleUtil::aboutServiceAnnounce();
    if (status != ER_OK) {
        std::cout << "Could not announce." << std::endl;
        return 1;
    }

    didInitSend = true;
    std::cout << "Sender Initialized" << std::endl;
    return true;
}

bool send(std::map<qcc::String, qcc::String>& params)
{
    int32_t type = atoi(params["type"].c_str());
    String typeString = static_cast<std::ostringstream*>(&(std::ostringstream() << type))->str().c_str();
    if (!(params["type"].compare(typeString) == 0)) {
        std::cout << "Could not send message: Message Type is not an integer value." << std::endl;
        return false;
    }

    messageType = (NotificationMessageType)atoi(params["type"].c_str());
    ttl = ((uint16_t)atoi(params["ttl"].c_str()));

    Notification notification(messageType, vecMessages);
    notification.setCustomAttributes(customAttributes);
    notification.setRichAudioUrl(richAudioUrl);
    if (richIconUrl.length()) {
        notification.setRichIconUrl(richIconUrl.c_str());
    }
    if (richIconObjectPath.length()) {
        notification.setRichIconObjectPath(richIconObjectPath.c_str());
    }
    if (richAudioObjectPath.length()) {
        notification.setRichAudioObjectPath(richAudioObjectPath.c_str());
    }
    if (controlPanelServiceObjectPath.length()) {
        notification.setControlPanelServiceObjectPath(controlPanelServiceObjectPath.c_str());
    }
    QStatus status = Sender->send(notification, ttl);
    if (status != ER_OK) {
        std::cout << "Could not send the message successfully.";
        return false;
    }
    std::cout << "Notification Sent for message type " << messageType << " and ttl of "  << ttl << " seconds" << std::endl;
    resetParams();
    return true;
}

bool setMsg(std::map<qcc::String, qcc::String>& params)
{
    // Empty qcc::Strings are valid input so we will not check it here
    NotificationText textToSend(params["lang"].c_str(), params["text"].c_str());
    vecMessages.push_back(textToSend);
    std::cout << "Message " << params["text"].c_str() << " set." << std::endl;
    return true;
}

bool setCustomAttributes(std::map<qcc::String, qcc::String>& params)
{
    // Empty qcc::Strings are valid input, only check format was correct
    customAttributes[params["key"].c_str()] = params["value"].c_str();
    std::cout << "Set custom pair " << params["key"].c_str() << " : " << params["value"].c_str() << std::endl;
    return true;
}

bool setLogger(std::map<qcc::String, qcc::String>& params)
{
    QCC_SetDebugLevel(logModules::NOTIFICATION_MODULE_LOG_NAME, logModules::ALL_LOG_LEVELS);
    return true;
}

bool disableSuperAgent(std::map<qcc::String, qcc::String>& params)
{
    if (!Service->disableSuperAgent()) {
        std::cout << "Could not disable SuperAgent." << std::endl;
        return false;
    }
    std::cout << "SuperAgentMode disabled" << std::endl;
    return true;
}

bool initReceive(std::map<qcc::String, qcc::String>& params)
{
    Receiver = new NotificationReceiverTestImpl();
    if (Service->initReceive(testBus, Receiver) != ER_OK) {
        std::cout << "Could not initialize receiver." << std::endl;
        return false;
    }

    didInitReceive = true;

    std::cout << "Receiver Initialized" << std::endl;
    return true;
}

bool ResponseToNotification(std::map<qcc::String, qcc::String>& params)
{
    NotificationReceiverTestImpl::NotificationAction action = ((NotificationReceiverTestImpl::NotificationAction)atoi(params["action"].c_str()));
    Receiver->SetNotificationAction(action);
    return true;
}

bool setRichIconUrl(std::map<qcc::String, qcc::String>& params)
{
    // Empty qcc::Strings are valid input so we will not check it here
    richIconUrl = params["url"];
    std::cout << "Icon Url " << params["url"].c_str() << " set." << std::endl;
    return true;
}

bool setRichAudioUrl(std::map<qcc::String, qcc::String>& params)
{
    // Empty qcc::Strings are valid input so we will not check it here
    RichAudioUrl audioContent(params["lang"].c_str(), params["url"].c_str());
    richAudioUrl.push_back(audioContent);
    std::cout << "Audio Content " << params["url"].c_str() << " set." << std::endl;
    return true;
}

bool setRichIconObjectPath(std::map<qcc::String, qcc::String>& params)
{
    // Empty qcc::Strings are valid input so we will not check it here
    richIconObjectPath = params["path"];
    std::cout << "Icon Object Path " << params["path"].c_str() << " set." << std::endl;
    return true;
}

bool setRichAudioObjectPath(std::map<qcc::String, qcc::String>& params)
{
    // Empty qcc::Strings are valid input so we will not check it here
    richAudioObjectPath = params["path"];
    std::cout << "Icon Object Path " << params["path"].c_str() << " set." << std::endl;
    return true;
}

bool setControlPanelServiceObjectPath(std::map<qcc::String, qcc::String>& params)
{
    // Empty qcc::Strings are valid input so we will not check it here
    controlPanelServiceObjectPath = params["path"];
    std::cout << "ControlPanelService Object Path " << params["path"].c_str() << " set." << std::endl;
    return true;
}

bool shutdownSender(std::map<qcc::String, qcc::String>& params)
{
    didInitSend = false;
    CommonSampleUtil::aboutServiceDestroy(testBus, notificationBusListener);

    Service->shutdownSender();
    std::cout << "service sender stopped" << std::endl;
    return true;
}

bool shutdown(std::map<qcc::String, qcc::String>& params)
{
    // clean up
    CommonSampleUtil::aboutServiceDestroy(testBus, notificationBusListener);
    if (notificationBusListener) {
        delete notificationBusListener;
        notificationBusListener = 0;
    }

    if (propertyStoreImpl) {
        delete propertyStoreImpl;
        propertyStoreImpl = 0;
    }

    if (Service) {
        Service->shutdown();
        Service = 0;
    }
    if (Receiver) {
        delete Receiver;
        Receiver = 0;
    }
    if (testBus) {
        delete testBus;
        testBus = 0;
    }

    didInitSend = false;
    didInitReceive = false;
    resetParams();
    std::cout << "Service Shutdown!" << std::endl;
    return true;
}

bool shutdownReceiver(std::map<qcc::String, qcc::String>& params)
{
    didInitReceive = false;
    Service->shutdownReceiver();
    std::cout << "service receiver stopped" << std::endl;
    return true;
}

bool deleteLastMsg(std::map<qcc::String, qcc::String>& params)
{
    NotificationMessageType deleteMessageType = (NotificationMessageType)atoi(params["type"].c_str());
    QStatus status = Sender->deleteLastMsg(deleteMessageType);
    if (status != ER_OK) {
        std::cout << "Could not delete the message successfully" << std::endl;
        return false;
    }
    std::cout << "Deleted last message of type " << params["type"].c_str() << std::endl;
    return true;
}

bool clearParams(std::map<qcc::String, qcc::String>& params)
{
    std::cout << "Clearing out message data. " << std::endl;
    resetParams();
    return true;
}

/*
 * Begin Utility Functions
 */
#define NUM_OF_FUNCTIONS 19

void checkNumFunctions(int32_t*i)
{
    if (*i == NUM_OF_FUNCTIONS) {
        std::cout << "Max number of functions < " << NUM_OF_FUNCTIONS << "> reached.  Exiting application due to error." << std::endl;
        exit(0);
    }
}

void createListOfFunctions(TestFunction*testFunctions)
{
    int32_t i = 0;

    // createService
    testFunctions[i].functionName  = "createservice";
    testFunctions[i].usage = testFunctions[i].functionName;
    testFunctions[i].activateTest = createService;

    i++;
    checkNumFunctions(&i);
    // initSend
    testFunctions[i].functionName  = "initsend";
    testFunctions[i].usage = testFunctions[i].functionName + " device_name=<name>&app_name=<app>";
    testFunctions[i].activateTest = initSend;
    testFunctions[i].requiredParams.push_back("device_name");
    testFunctions[i].requiredParams.push_back("app_name");
    testFunctions[i].requiredSteps.push_back("Service");

    i++;
    checkNumFunctions(&i);
    // send
    testFunctions[i].functionName  = "send";
    testFunctions[i].usage = testFunctions[i].functionName + " type=<0,1,2>&ttl=<ttl>";
    testFunctions[i].activateTest = send;
    testFunctions[i].requiredParams.push_back("type");
    testFunctions[i].requiredParams.push_back("ttl");
    testFunctions[i].requiredSteps.push_back("Service");
    testFunctions[i].requiredSteps.push_back("Sender");
    testFunctions[i].requiredSteps.push_back("messageType");
    testFunctions[i].requiredSteps.push_back("vecMessages");

    i++;
    checkNumFunctions(&i);
    // setMsg
    testFunctions[i].functionName  = "setmsg";
    testFunctions[i].usage = testFunctions[i].functionName + " text=<text>&lang=<lang>";
    testFunctions[i].activateTest = setMsg;
    testFunctions[i].requiredParams.push_back("text");
    testFunctions[i].requiredParams.push_back("lang");

    i++;
    checkNumFunctions(&i);
    // setCustomAttributes
    testFunctions[i].functionName  = "setcustomattributes";
    testFunctions[i].usage = testFunctions[i].functionName + " key=<key>&value=<value>";
    testFunctions[i].activateTest = setCustomAttributes;
    testFunctions[i].requiredParams.push_back("key");
    testFunctions[i].requiredParams.push_back("value");

    i++;
    checkNumFunctions(&i);
    // setLogger
    testFunctions[i].functionName  = "setlogger";
    testFunctions[i].usage = testFunctions[i].functionName;
    testFunctions[i].activateTest = setLogger;
    testFunctions[i].requiredSteps.push_back("Service");

    i++;
    checkNumFunctions(&i);
    // initReceive
    testFunctions[i].functionName  = "initreceive";
    testFunctions[i].usage = testFunctions[i].functionName;
    testFunctions[i].activateTest = initReceive;
    testFunctions[i].requiredSteps.push_back("Service");

    i++;
    checkNumFunctions(&i);
    // ResponseToNotification
    testFunctions[i].functionName  = "responsetonotification";
    testFunctions[i].usage = testFunctions[i].functionName + " action=<0-Nothing,1-Dismiss>";
    testFunctions[i].activateTest = ResponseToNotification;
    testFunctions[i].requiredSteps.push_back("Service");
    testFunctions[i].requiredSteps.push_back("Receiver");
    testFunctions[i].requiredParams.push_back("action");

    i++;
    checkNumFunctions(&i);
    // shutdown
    testFunctions[i].functionName  = "shutdown";
    testFunctions[i].usage = testFunctions[i].functionName;
    testFunctions[i].activateTest = shutdown;
    testFunctions[i].requiredSteps.push_back("Service");

    i++;
    checkNumFunctions(&i);
    // shutdownSender
    testFunctions[i].functionName  = "shutdownsender";
    testFunctions[i].usage = testFunctions[i].functionName;
    testFunctions[i].activateTest = shutdownSender;
    testFunctions[i].requiredSteps.push_back("Service");

    i++;
    checkNumFunctions(&i);
    // shutdownReceiver
    testFunctions[i].functionName  = "shutdownreceiver";
    testFunctions[i].usage = testFunctions[i].functionName;
    testFunctions[i].activateTest = shutdownReceiver;
    testFunctions[i].requiredSteps.push_back("Service");

    i++;
    checkNumFunctions(&i);
    // deletelastmsg
    testFunctions[i].functionName  = "deletelastmsg";
    testFunctions[i].usage = testFunctions[i].functionName + " type=<0,1,2>";
    testFunctions[i].activateTest = deleteLastMsg;
    testFunctions[i].requiredSteps.push_back("Service");
    testFunctions[i].requiredSteps.push_back("Sender");
    testFunctions[i].requiredParams.push_back("type");

    i++;
    checkNumFunctions(&i);
    // disableSuperAgent
    testFunctions[i].functionName  = "disablesuperagent";
    testFunctions[i].usage = testFunctions[i].functionName;
    testFunctions[i].activateTest = disableSuperAgent;
    testFunctions[i].requiredSteps.push_back("Service");

    i++;
    checkNumFunctions(&i);
    // clearParams
    testFunctions[i].functionName  = "clearparams";
    testFunctions[i].usage = testFunctions[i].functionName;
    testFunctions[i].activateTest = clearParams;

    i++;
    checkNumFunctions(&i);
    // setRichIconUrl
    testFunctions[i].functionName  = "setrichiconurl";
    testFunctions[i].usage = testFunctions[i].functionName + " url=<http://url>";
    testFunctions[i].requiredSteps.push_back("Service");
    testFunctions[i].requiredSteps.push_back("Sender");
    testFunctions[i].requiredParams.push_back("url");
    testFunctions[i].activateTest = setRichIconUrl;

    i++;
    checkNumFunctions(&i);
    // setRichAudioUrl
    testFunctions[i].functionName  = "setrichaudiourl";
    testFunctions[i].usage = testFunctions[i].functionName + " url=<http://url>&lang=<en_US>";
    testFunctions[i].requiredSteps.push_back("Service");
    testFunctions[i].requiredSteps.push_back("Sender");
    testFunctions[i].requiredParams.push_back("lang");
    testFunctions[i].requiredParams.push_back("url");
    testFunctions[i].activateTest = setRichAudioUrl;

    i++;
    checkNumFunctions(&i);
    // setRichIconObjectPath
    testFunctions[i].functionName  = "setrichiconobjectpath";
    testFunctions[i].usage = testFunctions[i].functionName + " path=<path>";
    testFunctions[i].requiredSteps.push_back("Service");
    testFunctions[i].requiredSteps.push_back("Sender");
    testFunctions[i].requiredParams.push_back("path");
    testFunctions[i].activateTest = setRichIconObjectPath;

    i++;
    checkNumFunctions(&i);
    // setRichAudioObjectPath
    testFunctions[i].functionName  = "setrichaudioobjectpath";
    testFunctions[i].usage = testFunctions[i].functionName + " path=<path>";
    testFunctions[i].requiredSteps.push_back("Service");
    testFunctions[i].requiredSteps.push_back("Sender");
    testFunctions[i].requiredParams.push_back("path");
    testFunctions[i].activateTest = setRichAudioObjectPath;

    i++;
    checkNumFunctions(&i);
    // setcontrolpanelserviceobjectpath
    testFunctions[i].functionName  = "setcontrolpanelserviceobjectpath";
    testFunctions[i].usage = testFunctions[i].functionName + " path=<path>";
    testFunctions[i].requiredSteps.push_back("Service");
    testFunctions[i].requiredSteps.push_back("Sender");
    testFunctions[i].requiredParams.push_back("path");
    testFunctions[i].activateTest = setControlPanelServiceObjectPath;

}

bool functionExists(qcc::String& funcName, TestFunction*testFunctions, int32_t*functionIndex)
{
    for (int32_t i = 0; i < NUM_OF_FUNCTIONS; i++) {
        if (((testFunctions[i].functionName).compare(funcName)) == 0) {
            *functionIndex = i;
            return true;
        }
    }
    std::cout << "functionExists - ERROR - not found funcName:" << funcName.c_str() << std::endl;
    return false;
}

void Usage(TestFunction*testFunctions, qcc::String funcName = "", int32_t*functionIndex = 0)
{
    if (funcName.size() > 0) {
        if (functionExists(funcName, testFunctions, functionIndex)) {
            std::cout << testFunctions[*functionIndex].usage.c_str() << std::endl;
            return;
        }
    }
    std::cout << "FUNCTION USAGE-------------------------------------FUNCTION USAGE" << std::endl;
    std::cout << "function name param=value&param=value" << std::endl << std::endl;
    std::cout << "Add  -h after any function name to print out the function usage." << std::endl << std::endl;
    std::cout << "Press 'enter' at any time to receive this help message again." << std::endl << std::endl;
    std::cout << "Available functions are:\n" << std::endl;
    for (int32_t i = 0; i < NUM_OF_FUNCTIONS; i++) {
        std::cout << testFunctions[i].functionName.c_str() << std::endl;
    }
    return;
}

void signal_callback_handler(int32_t signum)
{
    s_interrupt = true;
}

bool checkRequiredSteps(TestFunction& test, TestFunction*testFunctions, int32_t*functionIndex)
{
    std::vector<qcc::String> reqSteps = test.requiredSteps;
    for (std::vector<qcc::String>::const_iterator reqSteps_it = reqSteps.begin(); reqSteps_it != reqSteps.end(); ++reqSteps_it) {
        if ((reqSteps_it->compare("Service") == 0) && (!Service)) {
            std::cout << "Action not allowed.  Cannot run '" << test.functionName.c_str() << "' without creating a service first (createservice)" << std::endl;
            return false;
        } else if ((reqSteps_it->compare("Sender") == 0) && (!didInitSend)) {
            std::cout << "Action not allowed.  Cannot run '" << test.functionName.c_str() << "' without initializing a sender." << std::endl;
            qcc::String preReqApi = "initsend";
            if (functionExists(preReqApi, testFunctions,  functionIndex)) {
                Usage(testFunctions, preReqApi, functionIndex);
            }
            return false;
        } else if ((reqSteps_it->compare("Receiver") == 0) && (!didInitReceive)) {
            std::cout << "Action not allowed.  Cannot run '" << test.functionName.c_str() << "' without initializing a receiver." << std::endl;
            qcc::String preReqApi = "initreceive";
            if (functionExists(preReqApi, testFunctions,  functionIndex)) {
                Usage(testFunctions, preReqApi, functionIndex);
            }
            return false;
        } else if ((reqSteps_it->compare("vecMessages") == 0) && (!vecMessages.size())) {
            std::cout << "Cannot send message.  Missing mandatory parameter message text." << std::endl;
            qcc::String preReqApi = "setmsg";
            if (functionExists(preReqApi, testFunctions, functionIndex)) {
                Usage(testFunctions, preReqApi, functionIndex);
            }
            return false;
        }
    }
    return true;
}

void trim(qcc::String& str, const qcc::String& whitespace = " ")
{
    if (str.size() > 0) {
        unsigned strBegin = str.find_first_not_of(whitespace.c_str());
        if (strBegin == qcc::String::npos) {
            str = ""; // no content
            return;
        }

        unsigned strEnd = str.find_last_not_of(whitespace.c_str());
        if (strEnd == strBegin) {
            std::cout << "no content" << std::endl;
            str = "";     // no content
            return;
        }
        unsigned strRange = strEnd - strBegin + 1;
        str = str.substr(strBegin, strRange);
    }
}

bool processInput(const qcc::String& input, qcc::String& funcName, std::map<qcc::String, qcc::String>& params, TestFunction*testFunctions, int32_t*functionIndex)
{
    // Check first if the user pressed enter.
    if (!input.size()) {
        return false;
    }
    unsigned spacePos = input.find(" ");
    funcName = input.substr(0, spacePos).c_str();
    if (!funcName.size()) {
        std::cout << "Incorrect input format. Please check usage format" << std::endl;
        return false;
    }
    // Make FUNCTION name lower case
    std::transform(funcName.begin(), funcName.end(), funcName.begin(), ::tolower);
    if (!functionExists(funcName, testFunctions, functionIndex)) {
        std::cout << "Incorrect Function name. Please check usage format" << std::endl;
        return false;
    }

    // Check if the function was sent in with a help command
    // This covers -h, --h, -help and --help
    if (input.find(" -h") != qcc::String::npos) {
        // Help requested
        return false;
    }

    // Now parse the parameters sent in and put them into a std::map
    qcc::String paramString = input.substr(spacePos + 1).c_str();
    if (input.size() == paramString.size()) { // no parameters sent in so return early
        return true;
    }
    trim(paramString, " ");

    std::stringstream iss(paramString.c_str());
    std::string singleParam;
    while (getline(iss, singleParam, '&')) {
        unsigned equalPos = singleParam.find("=");
        if (equalPos == qcc::String::npos) {
            std::cout << "Parameters were not entered in the correct format. Please check usage format." << std::endl;
            Usage(testFunctions);
            return false;
        }
        params[singleParam.substr(0, equalPos).c_str()] = singleParam.substr(equalPos + 1).c_str();
    }
    return true;
}


int main(int argc, char* argv[])
{
    std::cout << "Beginning TestService Application. (Press CTRL+C and Enter to end application)" << std::endl;

    TestFunction testFunctions[NUM_OF_FUNCTIONS];
    createListOfFunctions(testFunctions);
    Usage(testFunctions);
    if (argc > 1) {
        return 0;
    }

    // Allow CTRL+C to end application
    signal(SIGINT, signal_callback_handler);

    while (!s_interrupt) {
        std::cout << "> ";
        std::string input;
        qcc::String funcName;
        std::map<qcc::String, qcc::String> params;
        int32_t functionIndex;
        getline(std::cin, input);
        if (processInput(input.c_str(), funcName, params, testFunctions, &functionIndex)) {
            if (((testFunctions[functionIndex].functionName).compare(funcName)) == 0) {
                if (checkRequiredSteps(testFunctions[functionIndex], testFunctions, &functionIndex)) {
                    if (testFunctions[functionIndex].checkRequiredParams(params)) {
                        testFunctions[functionIndex].checkOptionalParams(params);
                        testFunctions[functionIndex].activateTest(params);

                    }
                }
            }
        } else if (!s_interrupt) {
            Usage(testFunctions, funcName, &functionIndex);
        }
    }

    std::map<qcc::String, qcc::String> params;
    // clean up
    shutdown(params);
    std::cout << "Goodbye!" << std::endl;
    return 0;
}
