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

#include <iostream>
#include <sstream>
#include <cstdio>
#include <vector>
#include <map>
#include <iomanip>
#include <iterator>
#include <signal.h>
#include <SrpKeyXListener.h>
#include <CommonSampleUtil.h>
#include <alljoyn/services_common/LogModulesNames.h>
#include <alljoyn/services_common/GuidUtil.h>

#include "TimeClientAnnouncementHandler.h"
#include <alljoyn/time/TimeServiceConstants.h>
#include <alljoyn/time/TimeServiceClient.h>

#include "TimeServiceSessionListenerImpl.h"
#include "SampleTestUtils.h"
#include "TimeClientSignalHandler.h"
#include <alljoyn/AboutObjectDescription.h>

using namespace ajn;
using namespace services;
using namespace qcc;

// Set application constants
#define DEVICE_NAME "TimeService Client"
#define APP_NAME "Chronos Client"

BusAttachment* bus                             = NULL;
SrpKeyXListener* srpKeyXListener               = NULL;
TimeClientAnnouncementHandler* announceHandler = NULL;

static volatile sig_atomic_t s_interrupt       = false;

bool isClientStarted                           = false;
const char* interfaces[1];

//Map from the deviceId_appId key to the TimeServiceClient object
std::map<qcc::String, TimeServiceClient*> timeClients;

/**
 * TimeServiceClient that currently is being used
 */
TimeServiceClient* tsClient                       = NULL;
TimeServiceSessionListenerImpl* sessionListener   = NULL;
TimeClientSignalHandler* signalHandler            = NULL;
qcc::String currentServerKey;

/**
 * Alarms created by the AlarmFactory
 */
std::vector<TimeServiceClientAlarm*> dynamicAlarms;

/**
 * Timers created by the TimerFactory
 */
std::vector<TimeServiceClientTimer*> dynamicTimers;


//===========================================================//

#define CHECK_LOG(str, status) {                             \
        if (status != ER_OK) {                                   \
                                                             \
            printf("Failed to %s, Status: '%u'", str, status);   \
            return;                                              \
        }                                                        \
}
//===========================================================//

/**
 * Enum of possible TimeServiceClient actions
 */
typedef enum Actions {
    USAGE,
    START,
    STOP,
    SET_SERVER_KEY,         // KEY
    CONNECT,
    DISCONNECT,
    PRINT_DEVICES,
    PRINT_CLOCK,            //  OBJ
    SET_CLOCK,              //  OBJ
    TIME_SYNC,              //  OBJ
    TIME_SYNC_STOP,         //  OBJ
    PRINT_ALARM,            //  OBJ
    SET_ALARM,              //  OBJ; Optional: DAYS(sun,mon,tue,wen,thu,fri,sat) ENA: "t"/"f", TITLE
    ALARM_REACHED,          //  OBJ
    ALARM_REACHED_STOP,     //  OBJ
    FACTORY_NEW_ALARM,      //  OBJ
    FACTORY_DELETE_ALARM,   //  OBJ   ALARM
    FACTORY_GET_ALARMS,     //  OBJ
    PRINT_TIMER,            //  OBJ
    SET_TIMER,              //  OBJ; Optional: INT:"hh:mm:ss" REP TITLE
    TIMER_SIGNAL,           //  OBJ
    TIMER_SIGNAL_STOP,      //  OBJ
    TIMER_START,            //  OBJ
    TIMER_PAUSE,            //  OBJ
    TIMER_RESET,            //  OBJ
    FACTORY_NEW_TIMER,      //  OBJ
    FACTORY_DELETE_TIMER,   //  OBJ   ALARM
    FACTORY_GET_TIMERS,     //  OBJ
} Actions;

//===========================================================//
//Forward declaration of the Announcement CB

void onAnnouncement(const qcc::String& busName,
                    const qcc::String& deviceId,
                    const qcc::String& appId,
                    const qcc::String& uniqKey,
                    const ajn::AboutObjectDescription& objectDescription);

//===========================================================//
//Time Service Client actions

//Start service
void startService(const std::vector<std::string>& actionArgs);

//Stop service
void stopService(const std::vector<std::string>& actionArgs);

//Set server key
void setServerKey(const std::vector<std::string>& actionArgs);

//Connect
void connect(const std::vector<std::string>& actionArgs);

//Disconnect
void disconnect(const std::vector<std::string>& actionArgs);

//Print Devices
void printDevices(const std::vector<std::string>& actionArgs);

//Print Clock
void printClock(const std::vector<std::string>& actionArgs);

//Set Clock
void setClock(const std::vector<std::string>& actionArgs);

//Time Sync
void timeSync(const std::vector<std::string>& actionArgs);

//Time Sync Stop
void timeSyncStop(const std::vector<std::string>& actionArgs);

//Print Alarm
void printAlarm(const std::vector<std::string>& actionArgs);

//Set Alarm
void setAlarm(const std::vector<std::string>& actionArgs);

//Alarm Reached
void alarmReached(const std::vector<std::string>& actionArgs);

//Alarm Reached Stop
void alarmReachedStop(const std::vector<std::string>& actionArgs);

//Factory New Alarm
void factoryNewAlarm(const std::vector<std::string>& actionArgs);

//Factory Delete Alarm
void factoryDeleteAlarm(const std::vector<std::string>& actionArgs);

//Factory Get Alarms
void factoryGetAlarms(const std::vector<std::string>& actionArgs);

//Print Timer
void printTimer(const std::vector<std::string>& actionArgs);

//Set Timer
void setTimer(const std::vector<std::string>& actionArgs);

//Timer Signal
void timerSignal(const std::vector<std::string>& actionArgs);

//Timer Signal Stop
void timerSignalStop(const std::vector<std::string>& actionArgs);

//Timer Start
void timerStart(const std::vector<std::string>& actionArgs);

//Timer Pause
void timerPause(const std::vector<std::string>& actionArgs);

//Timer Reset
void timerReset(const std::vector<std::string>& actionArgs);

//Factory New Timer
void factoryNewTimer(const std::vector<std::string>& actionArgs);

//Factory Delete Timer
void factoryDeleteTimer(const std::vector<std::string>& actionArgs);

//Factory Get Timers
void factoryGetTimers(const std::vector<std::string>& actionArgs);

//===========================================================//

//Define pointer to the action function
typedef void (*ActionFunction)(const std::vector<std::string>&);

//Actions Map - from action type -> to action to execute
std::map<Actions, ActionFunction> actions;

//===========================================================//

/**
 * Interrupt (CTRL+C) handler
 */
static void SigIntHandler(int sig)
{

    std::cout << "got SigIntHandler" << std::endl;
    s_interrupt = true;
}

/**
 * Unregister AnnouncementHandler and delete the object
 */
void cleanAnnounceHandler()
{

    std::cout << "Clean Announcement handler" << std::endl;

    if (!announceHandler) {

        std::cout << "Announcement handler hasn't been initialized" << std::endl;
        return;
    }

    if (!bus) {

        std::cout << "BusAttachment hasn't been initialized" << std::endl;
        return;
    }

    QStatus status = bus->CancelWhoImplements(interfaces, sizeof(interfaces) / sizeof(interfaces[0]));
    if (status != ER_OK) {

        std::cout << "Failed to unregister AnnouncementHandler" << std::endl;
        return;
    }


    bus->UnregisterAboutListener(*announceHandler);

    delete announceHandler;
    announceHandler = NULL;
}

/**
 * Cleanup TimeServiceClient objects
 */
void cleanTimeClients()
{

    for (std::map<qcc::String, TimeServiceClient*>::iterator iter = timeClients.begin(); iter != timeClients.end(); ++iter) {

        delete iter->second;
    }

    timeClients.clear();

    tsClient = NULL;
    currentServerKey.clear();
}

/**
 * Delete the objects inside the received vector and clean it.
 * If passed tsClient is NULL, all the objects of cleanMe vector are deleted.
 * If tsClient is not NULL, only objects that belong to certain TimeServiceClient are removed
 *
 * @param cleanMe vector of the TimeService objects that were created by either Alarm or Timer factories
 * @param tsClient TimeServiceClient hosting the factory that created the TimeService object
 */
template <class TSItem>
void cleanDynamicTsItems(std::vector<TSItem*>& cleanMe, TimeServiceClient* tsClient = NULL)
{

    typename std::vector<TSItem*>::iterator iter = cleanMe.begin();

    while (iter != cleanMe.end()) {

        if (tsClient) {

            TimeServiceClientBase* currTsItem = static_cast<TimeServiceClientBase*>(*iter);

            //If currTsItem belongs to the requested tsClient => delete it
            if ((tsClient->getAppId().compare(currTsItem->getTsClient().getAppId()) == 0) &&
                (tsClient->getDeviceId().compare(currTsItem->getTsClient().getDeviceId()) == 0)) {

                delete *iter;
                iter = cleanMe.erase(iter);
            } else {

                ++iter;
            }
        }
        //tsClient was not passed in => Remove all its elements
        else {

            delete *iter;
            iter = cleanMe.erase(iter);
        }
    } //while
}

/**
 * Clean Time Objects
 */
void cleanupTimeObjects()
{

    cleanAnnounceHandler();

    cleanDynamicTsItems(dynamicAlarms);
    cleanDynamicTsItems(dynamicTimers);

    //This MUST be AFTER cleaning the dynamic Alarms and Timers, in order not to delete TimeServiceClient memory
    cleanTimeClients();
}

/**
 * Cleaning up resources
 */
void cleanup()
{

    cleanupTimeObjects();

    if (bus) {

        bus->Stop();

        delete bus;
        bus = NULL;
    }

    if (srpKeyXListener) {

        delete srpKeyXListener;
        srpKeyXListener = NULL;
    }

    if (sessionListener) {

        delete sessionListener;
        sessionListener = NULL;
    }

    if (signalHandler) {

        delete signalHandler;
        signalHandler = NULL;
    }

    std::cout << "Goodbye!" << std::endl;
}

/**
 * Print prompt sign
 */
void printPrompt()
{

    printf("\n> ");
}

/**
 * Print the Usage guide
 */
void printUsage()
{

    std::cout << "***********************************************************************************************************" << std::endl;
    std::cout << "*                                                                                                         *" << std::endl;
    std::cout << std::setiosflags(std::ios::left);
    std::cout << "* " << std::setw(104) << (currentServerKey.empty() ? "Key is not set" : ("Key is:" + currentServerKey).c_str())  << "*" << std::endl;
    std::cout << "*                                                                                                         *" << std::endl;
    std::cout << "* Select possible action to execute and press [Enter],                                                    *" << std::endl;
    std::cout << "* to stop this application press [Ctrl+C] and [Enter].                                                    *" << std::endl;
    std::cout << "* o Denotes optional param                                                                                *" << std::endl;
    std::cout << "* Usage                     '0'                                                                           *" << std::endl;
    std::cout << "* Start Client              '1'                                                                           *" << std::endl;
    std::cout << "* Stop Client               '2'                                                                           *" << std::endl;
    std::cout << "* Set server key            '3' KEY:'adddbc8bd97942458faba15120106709_A74CDF7C20A343A8AE276E57D57A577F'   *" << std::endl;
    std::cout << "* Connect                   '4'                                                                           *" << std::endl;
    std::cout << "* Disconnect                '5'                                                                           *" << std::endl;
    std::cout << "* Print Devices             '6'                                                                           *" << std::endl;
    std::cout << "* Print Clock               '7' OBJ:'/Clock1'                                                             *" << std::endl;
    std::cout << "* Set Clock                 '8' OBJ:'/Clock1'                                                             *" << std::endl;
    std::cout << "* Time Sync                 '9' OBJ:'/Clock1'                                                             *" << std::endl;
    std::cout << "* Time Sync Stop           '10' OBJ:'/Clock1'                                                             *" << std::endl;
    std::cout << "* Print Alarm              '11' OBJ:'/Alarm2'                                                             *" << std::endl;
    std::cout << "* Set Alarm                '12' OBJ:'/Alarm2' oDays:DAYS(sun,mon,tue,wen,thu,fri,sat) oENA:\"t\"/\"f\",oTITLE *" << std::endl;
    std::cout << "* Alarm Reached            '13' OBJ:'/Alarm2'                                                             *" << std::endl;
    std::cout << "* Alarm Reached Stop       '14' OBJ:'/Alarm2'                                                             *" << std::endl;
    std::cout << "* Factory New Alarm        '15' OBJ:'/AlarmFactory3'                                                      *" << std::endl;
    std::cout << "* Factory Delete Alarm     '16' OBJ:'/AlarmFactory3' ALARM:'/AlarmFactory3/Alarm5'                        *" << std::endl;
    std::cout << "* Factory Get Alarms       '17' OBJ:'/AlarmFactory3'                                                      *" << std::endl;
    std::cout << "* Print Timer              '18' OBJ:'/Timer4'                                                             *" << std::endl;
    std::cout << "* Set Timer                '19' OBJ:'/Timer4' INT:\"hh:mm:ss\" REP:3 oTITLE:'my title'                      *" << std::endl;
    std::cout << "* Timer Signal             '20' OBJ:'/Timer4'                                                             *" << std::endl;
    std::cout << "* Timer Signal Stop        '21' OBJ:'/Timer4'                                                             *" << std::endl;
    std::cout << "* Timer Start              '22' OBJ:'/Timer4'                                                             *" << std::endl;
    std::cout << "* Timer Pause              '23' OBJ:'/Timer4'                                                             *" << std::endl;
    std::cout << "* Timer Reset              '24' OBJ:'/Timer4'                                                             *" << std::endl;
    std::cout << "* Factory New Timer        '25' OBJ:'/FactoryTimer6'                                                      *" << std::endl;
    std::cout << "* Factory Delete Timer     '26' OBJ:'/FactoryTimer6' ALARM:'/FactoryTimer6/Timer7'                        *" << std::endl;
    std::cout << "* Factory Get Timers       '27' OBJ:'/FactoryTimer6'                                                      *" << std::endl;
    std::cout << "***********************************************************************************************************" << std::endl;
    printPrompt();
}

/**
 * Parses input string
 * @param String to parse
 * @return Vector with string parts
 */
void parseInput(std::string toParse, std::vector<std::string>* strParts)
{

    std::istringstream strBuffer(toParse);
    std::istream_iterator<std::string> strBeginIter(strBuffer);
    std::istream_iterator<std::string> strEndIter;

    std::copy(strBeginIter, strEndIter, std::back_inserter(*strParts));
    return;
}

/**
 * Uses atoi() to convert string to int
 */
int strToInt(const std::string str)
{

    return atoi(str.c_str());
}

/**
 * Performs action following the first argument in the received input
 * @param input to parse and execute the appropriate action.
 * If the action is unsupported Usage menu is printed
 */
void doAction(std::string input)
{

    std::vector<std::string> strParts;
    parseInput(input, &strParts);

    if (strParts.size() == 0) {

        printPrompt();
        return;
    }

    Actions action = static_cast<Actions>(strToInt(strParts[0]));
    if (action == USAGE || !action) {

        printUsage();
        return;
    }

    ActionFunction invFunction = actions[action];
    if (!invFunction) {

        std::cout << "ACTION METHOD IS UNDEFINED" << std::endl;
        printUsage();
        return;
    }

    //Invoke the action function
    (*invFunction)(strParts);

    std::cout << std::endl;
    std::cout << std::endl;
    printUsage();
}

/**
 * Starts Time Client.
 * Receives commands from STDIN
 */
void startTimeClient()
{

    printUsage();

    std::string input;

    do {

        getline(std::cin, input);
        doAction(input);

#ifdef _WIN32
        Sleep(100);
#else
        usleep(100 * 1000);
#endif

    } while (s_interrupt == false && !std::cin.eof());
}

/**
 * Populate TimeServiceClient actions
 */
void populateActions()
{

    actions.insert(std::pair<Actions, ActionFunction>(START,                startService));
    actions.insert(std::pair<Actions, ActionFunction>(STOP,                 stopService));
    actions.insert(std::pair<Actions, ActionFunction>(STOP,                 stopService));
    actions.insert(std::pair<Actions, ActionFunction>(SET_SERVER_KEY,       setServerKey));
    actions.insert(std::pair<Actions, ActionFunction>(CONNECT,              connect));
    actions.insert(std::pair<Actions, ActionFunction>(DISCONNECT,           disconnect));
    actions.insert(std::pair<Actions, ActionFunction>(PRINT_DEVICES,        printDevices));
    actions.insert(std::pair<Actions, ActionFunction>(PRINT_CLOCK,          printClock));
    actions.insert(std::pair<Actions, ActionFunction>(SET_CLOCK,            setClock));
    actions.insert(std::pair<Actions, ActionFunction>(TIME_SYNC,            timeSync));
    actions.insert(std::pair<Actions, ActionFunction>(TIME_SYNC_STOP,       timeSyncStop));
    actions.insert(std::pair<Actions, ActionFunction>(PRINT_ALARM,          printAlarm));
    actions.insert(std::pair<Actions, ActionFunction>(SET_ALARM,            setAlarm));
    actions.insert(std::pair<Actions, ActionFunction>(ALARM_REACHED,        alarmReached));
    actions.insert(std::pair<Actions, ActionFunction>(ALARM_REACHED_STOP,   alarmReachedStop));
    actions.insert(std::pair<Actions, ActionFunction>(FACTORY_NEW_ALARM,    factoryNewAlarm));
    actions.insert(std::pair<Actions, ActionFunction>(FACTORY_DELETE_ALARM, factoryDeleteAlarm));
    actions.insert(std::pair<Actions, ActionFunction>(FACTORY_GET_ALARMS,   factoryGetAlarms));
    actions.insert(std::pair<Actions, ActionFunction>(PRINT_TIMER,          printTimer));
    actions.insert(std::pair<Actions, ActionFunction>(SET_TIMER,            setTimer));
    actions.insert(std::pair<Actions, ActionFunction>(TIMER_SIGNAL,         timerSignal));
    actions.insert(std::pair<Actions, ActionFunction>(TIMER_SIGNAL_STOP,    timerSignalStop));
    actions.insert(std::pair<Actions, ActionFunction>(TIMER_START,          timerStart));
    actions.insert(std::pair<Actions, ActionFunction>(TIMER_PAUSE,          timerPause));
    actions.insert(std::pair<Actions, ActionFunction>(TIMER_RESET,          timerReset));
    actions.insert(std::pair<Actions, ActionFunction>(FACTORY_NEW_TIMER,    factoryNewTimer));
    actions.insert(std::pair<Actions, ActionFunction>(FACTORY_DELETE_TIMER, factoryDeleteTimer));
    actions.insert(std::pair<Actions, ActionFunction>(FACTORY_GET_TIMERS,   factoryGetTimers));
}

/**
 * Main
 */
int main()
{
    // Allow CTRL+C to end application
    signal(SIGINT, SigIntHandler);
    std::cout << "Beginning TimeService Client Application. (Press CTRL+C and enter or CTRL+D to end application)" << std::endl;

    QCC_SetDebugLevel(logModules::TIME_MODULE_LOG_NAME, logModules::ALL_LOG_LEVELS);

    //Initialize AuthListener
    srpKeyXListener = new SrpKeyXListener();

    //Create BusAttachment
    bus = CommonSampleUtil::prepareBusAttachment(srpKeyXListener);
    if (bus == NULL) {

        std::cout << "Could not initialize BusAttachment." << std::endl;
        return 1;
    }

    std::cout << "Created BusAttachment bus: '" << bus->GetUniqueName() << "'" << std::endl;

    //Populate actions
    populateActions();

    //Initialize interfaces to be filtered out by the About
    qcc::String annIface  = tsConsts::IFNAME_PREFIX + "*";
    interfaces[0]         = annIface.c_str();

    //Do the TimeServiceClient work
    startTimeClient();

    //Cleanup
    cleanup();

    return 0;
}

//======================================================//
//              TIME SERVICE CLIENT FUNCTIONS           //
//======================================================//

//Search for the object path in the searchIn vector
template <class TSItem>
TimeServiceClientBase* findTsItem(const qcc::String& objPath, const std::vector<TSItem*>& searchIn)
{

    if (objPath.length() == 0) {

        return NULL;
    }

    for (typename std::vector<TSItem*>::const_iterator iter = searchIn.begin(); iter != searchIn.end(); ++iter) {

        TimeServiceClientBase* tsItem = static_cast<TimeServiceClientBase*>(*iter);

        //If object found => return it
        if (tsItem->getObjectPath().compare(objPath) == 0) {

            return *iter;
        }
    }

    return NULL;
}

/**
 * Checks whether received objectPath is created by the AlarmFactory
 *
 * @param objectPath to check
 *
 * @return TRUE if the Alarm was created by the AlarmFactory
 */
bool isDynamicAlarm(const qcc::String& objectPath)
{

    qcc::String prefix("/AlarmFactory");
    return (objectPath.find(prefix) != std::string::npos) ? true : false;
}

/**
 * Checks whether received objectPath is created by the TimerFactory
 *
 * @param objectPath to check
 *
 * @return TRUE if the Timer was created by the TimerFactory
 */
bool isDynamicTimer(const qcc::String& objectPath)
{

    qcc::String prefix("/TimerFactory");
    return (objectPath.find(prefix) != std::string::npos) ? true : false;
}

//Start service
void startService(const std::vector<std::string>& actionArgs)
{

    if (isClientStarted) {

        std::cout << "TimeServiceClient has been started previously " << std::endl;
        return;
    }

    announceHandler       = new TimeClientAnnouncementHandler(onAnnouncement);
    bus->RegisterAboutListener(*announceHandler);
    QStatus status = bus->WhoImplements(interfaces, sizeof(interfaces) / sizeof(interfaces[0]));
    if (ER_OK == status) {
        std::cout << "WhoImplements called." << std::endl;
    } else {
        std::cout << "ERROR - WhoImplements failed." << std::endl;
    }

    if (status != ER_OK) {

        std::cout << "Failed to register AnnouncementHandler" << std::endl;
        delete announceHandler;
        announceHandler = NULL;

        return;
    }

    std::cout << "Initializing TimeServiceClient" << std::endl;
    isClientStarted = true;
}

//Stop service
void stopService(const std::vector<std::string>& actionArgs)
{

    if (!isClientStarted) {

        std::cout << "TimeServiceClient hasn't been started" << std::endl;
        return;
    }

    std::cout << "Stopping TimeServiceClient" << std::endl;
    cleanupTimeObjects();

    isClientStarted     = false;
}

bool isClientUpCheckMandatoryArgumentsCnt(const std::vector<std::string>& actionArgs, uint8_t argCount = 0)
{
    if (!isClientStarted) {

        std::cout << "TimeServiceClient hasn't been started" << std::endl;
        return false;
    }

    if (actionArgs.size() < (uint8_t)(argCount + 1)) {
        std::cout << "Expecting " << int(argCount) << " args, only " << int(actionArgs.size() - 1) << " found" << std::endl;
        return false;
    }

    return true;
}

//Set server key
void setServerKey(const std::vector<std::string>& actionArgs)
{

    if (isClientUpCheckMandatoryArgumentsCnt(actionArgs, 1)) {

        const char* serverKeyChr = actionArgs[1].c_str();
        qcc::String key(serverKeyChr);

        std::map<qcc::String, TimeServiceClient*>::iterator iter =  timeClients.find(serverKeyChr);

        if (timeClients.end() == iter) {

            printf("Unknown server key, use \"Print Devices\" method to get the correct key\n");
            return;
        }

        tsClient = iter->second;
        currentServerKey.assign(serverKeyChr);
        printf("===> Server Key to be used is: '%s', BusName: '%s' <=== \n", serverKeyChr, tsClient->getServerBusName().c_str());
    }
}

//Connect
void connect(const std::vector<std::string>& actionArgs)
{

    if (isClientUpCheckMandatoryArgumentsCnt(actionArgs)) {

        if (!tsClient) {

            std::cout
            << "Server Key hasn't been set. Use \"Set Server Key\" method." << std::endl;
            return;
        }

        if (!sessionListener) {

            sessionListener = new TimeServiceSessionListenerImpl();
        }

        printf("Join session is called asynchronously, to continue working, wait for the \"session joined succeeded\""
               " response message\n");

        QStatus status = tsClient->joinSessionAsync(sessionListener);

        if (status != ER_OK) {

            printf("Failed to join session, Status: '%u'\n", status);
        }
    }
}

//Disconnect
void disconnect(const std::vector<std::string>& actionArgs)
{

    if (isClientUpCheckMandatoryArgumentsCnt(actionArgs)) {

        if (!tsClient) {

            std::cout
            << "Server Key hasn't been set. Use \"Set Server Key\" method."
            << std::endl;
            return;
        }

        QStatus status = tsClient->leaveSession();

        if (status != ER_OK) {

            printf("Failed to leave the session, Status: '%u'\n", status);
        }
    }
}

//Print Devices
void printDevices(const std::vector<std::string>& actionArgs)
{

    printf("Current Time servers: \n=====================\n");

    for (std::map<qcc::String, TimeServiceClient*>::iterator iter = timeClients.begin();
         iter != timeClients.end(); ++iter) {

        printf("%s\n", iter->first.c_str());
    }
}

//Print Clock
void printClock(const std::vector<std::string>& actionArgs)
{
    if (!isClientUpCheckMandatoryArgumentsCnt(actionArgs, 1)) {

        printf("Clock object path is expected \n");
        return;
    }

    if (!tsClient) {

        std::cout
        << "Server Key hasn't been set. Use \"Set Server Key\" method." << std::endl;
        return;
    }

    qcc::String clockObjectPath(actionArgs[1].c_str());

    TimeServiceClientClock* clock = static_cast<TimeServiceClientClock*>(findTsItem(clockObjectPath,
                                                                                    tsClient->getAnnouncedClockList()));

    if (!clock) {

        printf("Clock object not found  at the given ObjectPath: '%s'", clockObjectPath.c_str());
        return;
    }

    QStatus status;

    TimeServiceDateTime dateTime;
    status = clock->retrieveDateTime(&dateTime);
    CHECK_LOG("retrieve DateTime", status);

    bool isSet;
    status = clock->retrieveIsSet(&isSet);
    CHECK_LOG("retrieve IsSet", status);

    //"Clock: Authority: '%s', AuthType: '%s', DateTime: '%s', IsSet: '%s'"
    printf("Clock: ");
    sampleTestUtils::printDateTime(dateTime);
    printf("IsSet: '%s' \n", isSet ? "TRUE" : "FALSE");

    if (clock->isAuthority()) {

        tsConsts::ClockAuthorityType authType;
        status = clock->retrieveAuthorityType(&authType);
        CHECK_LOG("retrieve Authority Type", status);

        const char* authTypes[] = { "OTHER", "NTP", "CELLULAR", "GPS", "USER_DESIGNATED" };
        printf("Auth Type: '%s'", authTypes[authType]);
    }
}


//Set Clock
void setClock(const std::vector<std::string>& actionArgs)
{

    if (!isClientUpCheckMandatoryArgumentsCnt(actionArgs, 1)) {

        printf("Clock object path is expected \n");
        return;
    }

    if (!tsClient) {

        std::cout
        << "Server Key hasn't been set. Use \"Set Server Key\" method." << std::endl;
        return;
    }

    qcc::String clockObjectPath(actionArgs[1].c_str());

    TimeServiceClientClock* clock = static_cast<TimeServiceClientClock*>(findTsItem(clockObjectPath,
                                                                                    tsClient->getAnnouncedClockList()));

    if (!clock) {

        printf("Clock object not found at the given ObjectPath: '%s'", clockObjectPath.c_str());
        return;
    }

    TimeServiceDateTime dateTime;
    sampleTestUtils::dateTimeNow(&dateTime);

    printf("Setting DateTime: ");
    sampleTestUtils::printDateTime(dateTime);

    QStatus status = clock->setDateTime(dateTime);
    CHECK_LOG("set DateTime", status);
}

//Time Sync
void timeSync(const std::vector<std::string>& actionArgs)
{

    if (!isClientUpCheckMandatoryArgumentsCnt(actionArgs, 1)) {

        printf("Clock object path is expected \n");
        return;
    }

    if (!tsClient) {

        std::cout
        << "Server Key hasn't been set. Use \"Set Server Key\" method." << std::endl;
        return;
    }

    qcc::String clockObjectPath(actionArgs[1].c_str());

    TimeServiceClientClock* clock = static_cast<TimeServiceClientClock*>(findTsItem(clockObjectPath,
                                                                                    tsClient->getAnnouncedClockList()));

    if (!clock) {

        printf("Clock object not found at the given ObjectPath: '%s'", clockObjectPath.c_str());
        return;
    }

    if (!signalHandler) {

        signalHandler = new TimeClientSignalHandler();
    }

    QStatus status = clock->registerTimeAuthorityHandler(signalHandler);
    CHECK_LOG("register TimeSync signal handler", status);
}

//Time Sync Stop
void timeSyncStop(const std::vector<std::string>& actionArgs)
{
    if (!isClientUpCheckMandatoryArgumentsCnt(actionArgs, 1)) {

        printf("Clock object path is expected \n");
        return;
    }

    if (!tsClient) {

        std::cout
        << "Server Key hasn't been set. Use \"Set Server Key\" method." << std::endl;
        return;
    }

    qcc::String clockObjectPath(actionArgs[1].c_str());

    TimeServiceClientClock* clock = static_cast<TimeServiceClientClock*>(findTsItem(clockObjectPath,
                                                                                    tsClient->getAnnouncedClockList()));

    if (!clock) {

        printf("Clock object not found at the given ObjectPath: '%s'", clockObjectPath.c_str());
        return;
    }

    clock->unRegisterTimeAuthorityHandler();
}

//Print Alarm
void printAlarm(const std::vector<std::string>& actionArgs)
{

    if (!isClientUpCheckMandatoryArgumentsCnt(actionArgs, 1)) {

        printf("Alarm object path is expected \n");
        return;
    }

    if (!tsClient) {

        std::cout
        << "Server Key hasn't been set. Use \"Set Server Key\" method." << std::endl;
        return;
    }

    qcc::String alarmObjectPath(actionArgs[1].c_str());

    const std::vector<TimeServiceClientAlarm*> alarms = isDynamicAlarm(alarmObjectPath) ? dynamicAlarms :
                                                        tsClient->getAnnouncedAlarmList();

    TimeServiceClientAlarm* alarm = static_cast<TimeServiceClientAlarm*>(findTsItem(alarmObjectPath, alarms));

    if (!alarm) {

        printf("Alarm object not found at the given ObjectPath: '%s'", alarmObjectPath.c_str());
        return;
    }

    const std::vector<qcc::String>& langs = alarm->retrieveDescriptionLanguages();
    qcc::String lang = langs.size() > 0 ? langs[0] : qcc::String::Empty;

    bool isEnabled;
    QStatus status   = alarm->retrieveIsEnabled(&isEnabled);
    CHECK_LOG("retrieve IsEnabled", status);

    TimeServiceSchedule schedule;
    status = alarm->retrieveSchedule(&schedule);
    CHECK_LOG("retrieve Schedule", status);

    qcc::String title;
    status = alarm->retrieveTitle(&title);
    CHECK_LOG("retrieve Title", status);

    printf("Alarm: Enabled: '%s', Title: '%s', Description: '%s' \n",
           isEnabled ? "YES" : "NO", title.c_str(),
           lang.length() > 0 ? alarm->retrieveObjectDescription(lang).c_str() : "");

    sampleTestUtils::printSchedule(schedule);
}

//Set Alarm
void setAlarm(const std::vector<std::string>& actionArgs)
{

    if (!isClientUpCheckMandatoryArgumentsCnt(actionArgs, 1)) {

        printf("Alarm object path is expected \n");
        return;
    }

    if (!tsClient) {

        std::cout
        << "Server Key hasn't been set. Use \"Set Server Key\" method." << std::endl;
        return;
    }

    qcc::String alarmObjectPath(actionArgs[1].c_str());

    const std::vector<TimeServiceClientAlarm*> alarms = isDynamicAlarm(alarmObjectPath) ? dynamicAlarms :
                                                        tsClient->getAnnouncedAlarmList();

    TimeServiceClientAlarm* alarm = static_cast<TimeServiceClientAlarm*>(findTsItem(alarmObjectPath, alarms));


    if (!alarm) {

        printf("Alarm object not found at the given ObjectPath: '%s'",
               alarmObjectPath.c_str());
        return;
    }

    uint8_t weekDays = 0;
    std::string days;
    std::string enable;
    std::string title;

    if (actionArgs.size() >= 3) {

        days = actionArgs[2];
    }

    if (actionArgs.size() >= 4) {

        enable = actionArgs[3];
    }

    if (actionArgs.size() >= 5) {

        title = actionArgs[4];
    }

    TimeServiceDateTime dateTime;
    sampleTestUtils::dateTimeNow(&dateTime, 120); //2 minutes forward

    if (days.length() > 0) {

        weekDays = sampleTestUtils::getWeekdaysNum(days);
    }

    TimeServiceSchedule schedule;
    schedule.init(dateTime.getTime(), weekDays);
    QStatus status = alarm->setSchedule(schedule);

    CHECK_LOG("set Schedule", status);

    if (enable.length() > 0) {

        status = alarm->setEnabled(strcmp(enable.c_str(), "t") == 0 ? true : false);
        CHECK_LOG("set Enable", status);
    }

    if (title.length() > 0) {

        qcc::String myTitle(title.c_str());
        status = alarm->setTitle(myTitle);
        CHECK_LOG("set Title", status);
    }
}

//Alarm Reached
void alarmReached(const std::vector<std::string>& actionArgs)
{

    if (!isClientUpCheckMandatoryArgumentsCnt(actionArgs, 1)) {

        printf("Alarm object path is expected \n");
        return;
    }

    if (!tsClient) {

        std::cout
        << "Server Key hasn't been set. Use \"Set Server Key\" method." << std::endl;
        return;
    }

    qcc::String alarmObjectPath(actionArgs[1].c_str());

    const std::vector<TimeServiceClientAlarm*> alarms = isDynamicAlarm(alarmObjectPath) ? dynamicAlarms :
                                                        tsClient->getAnnouncedAlarmList();

    TimeServiceClientAlarm* alarm = static_cast<TimeServiceClientAlarm*>(findTsItem(alarmObjectPath, alarms));


    if (!alarm) {

        printf("Alarm object not found at the given ObjectPath: '%s'",
               alarmObjectPath.c_str());
        return;
    }

    if (!signalHandler) {

        signalHandler = new TimeClientSignalHandler();
    }

    QStatus status = alarm->registerAlarmHandler(signalHandler);
    CHECK_LOG("register AlarmReached signal handler", status);
}

//Alarm Reached Stop
void alarmReachedStop(const std::vector<std::string>& actionArgs)
{

    if (!isClientUpCheckMandatoryArgumentsCnt(actionArgs, 1)) {

        printf("Alarm object path is expected \n");
        return;
    }

    if (!tsClient) {

        std::cout
        << "Server Key hasn't been set. Use \"Set Server Key\" method." << std::endl;
        return;
    }

    qcc::String alarmObjectPath(actionArgs[1].c_str());

    const std::vector<TimeServiceClientAlarm*> alarms = isDynamicAlarm(alarmObjectPath) ? dynamicAlarms :
                                                        tsClient->getAnnouncedAlarmList();

    TimeServiceClientAlarm* alarm = static_cast<TimeServiceClientAlarm*>(findTsItem(alarmObjectPath, alarms));


    if (!alarm) {

        printf("Alarm object not found at the given ObjectPath: '%s'",
               alarmObjectPath.c_str());
        return;
    }

    alarm->unRegisterAlarmHandler();
}

//Factory New Alarm
void factoryNewAlarm(const std::vector<std::string>& actionArgs)
{

    if (!isClientUpCheckMandatoryArgumentsCnt(actionArgs, 1)) {

        printf("AlarmFactory object path is expected \n");
        return;
    }

    if (!tsClient) {

        std::cout
        << "Server Key hasn't been set. Use \"Set Server Key\" method." << std::endl;
        return;
    }

    qcc::String alarmFactoryObjectPath(actionArgs[1].c_str());

    TimeServiceClientAlarmFactory* alarmFactory = static_cast<TimeServiceClientAlarmFactory*>(findTsItem(alarmFactoryObjectPath,
                                                                                                         tsClient->getAnnouncedAlarmFactoryList()));

    if (!alarmFactory) {

        printf("AlarmFactory object not found at the given ObjectPath: '%s'", alarmFactoryObjectPath.c_str());
        return;
    }

    qcc::String newAlarmObjPath;
    QStatus status = alarmFactory->newAlarm(&newAlarmObjPath);
    CHECK_LOG("create New Alarm", status);

    printf("Created NewAlarm: '%s' \n", newAlarmObjPath.c_str());

    TimeServiceClientAlarm* alarm = new TimeServiceClientAlarm(*tsClient, newAlarmObjPath);
    dynamicAlarms.push_back(alarm);
}

//Factory Delete Alarm
void factoryDeleteAlarm(const std::vector<std::string>& actionArgs)
{

    if (!isClientUpCheckMandatoryArgumentsCnt(actionArgs, 2)) {

        printf("The Object paths of the AlarmFactory and Alarm to be deleted are expected \n");
        return;
    }

    if (!tsClient) {

        std::cout << "Server Key hasn't been set. Use \"Set Server Key\" method." << std::endl;
        return;
    }

    qcc::String alarmFactoryObjectPath(actionArgs[1].c_str());
    qcc::String deleteAlarmObjectPath(actionArgs[2].c_str());

    TimeServiceClientAlarmFactory* alarmFactory = static_cast<TimeServiceClientAlarmFactory*>(findTsItem(alarmFactoryObjectPath,
                                                                                                         tsClient->getAnnouncedAlarmFactoryList()));

    if (!alarmFactory) {

        printf("AlarmFactory object not found at the given ObjectPath: '%s'", alarmFactoryObjectPath.c_str());
        return;
    }

    QStatus status = alarmFactory->deleteAlarm(deleteAlarmObjectPath);
    CHECK_LOG("DeleteAlarm", status);

    std::vector<TimeServiceClientAlarm*>::iterator iter = dynamicAlarms.begin();
    while (iter != dynamicAlarms.end()) {

        if ((*iter)->getObjectPath().compare(deleteAlarmObjectPath) == 0) {

            printf("Deleting Alarm: '%s' locally \n", deleteAlarmObjectPath.c_str());
            delete *iter;
            iter = dynamicAlarms.erase(iter);
        } else {

            ++iter;
        }
    }
}

//Factory Get Alarms
void factoryGetAlarms(const std::vector<std::string>& actionArgs)
{

    if (!isClientUpCheckMandatoryArgumentsCnt(actionArgs, 1)) {

        printf("AlarmFactory object path is expected \n");
        return;
    }

    if (!tsClient) {

        std::cout << "Server Key hasn't been set. Use \"Set Server Key\" method." << std::endl;
        return;
    }

    qcc::String alarmFactoryObjectPath(actionArgs[1].c_str());

    TimeServiceClientAlarmFactory* alarmFactory = static_cast<TimeServiceClientAlarmFactory*>(findTsItem(alarmFactoryObjectPath,
                                                                                                         tsClient->getAnnouncedAlarmFactoryList()));

    if (!alarmFactory) {

        printf("AlarmFactory object not found at the given ObjectPath: '%s'", alarmFactoryObjectPath.c_str());
        return;
    }

    std::vector<qcc::String> factoryAlarms;
    QStatus status = alarmFactory->retrieveAlarmList(&factoryAlarms);
    CHECK_LOG("retrieve AlarmList", status);

    printf("Releasing current Alarms of this factory: '%s' and storing new Alarms list \n", alarmFactoryObjectPath.c_str());
    cleanDynamicTsItems(dynamicAlarms, tsClient);

    for (std::vector<qcc::String>::iterator iter = factoryAlarms.begin(); iter != factoryAlarms.end(); ++iter) {

        printf("New Factory Alarm: '%s' \n", (*iter).c_str());
        TimeServiceClientAlarm* alarm = new TimeServiceClientAlarm(*tsClient, *iter);
        dynamicAlarms.push_back(alarm);
    }
}

//Print Timer
void printTimer(const std::vector<std::string>& actionArgs)
{

    if (!isClientUpCheckMandatoryArgumentsCnt(actionArgs, 1)) {

        printf("Timer object path is expected \n");
        return;
    }

    if (!tsClient) {

        std::cout
        << "Server Key hasn't been set. Use \"Set Server Key\" method." << std::endl;
        return;
    }

    qcc::String timerObjectPath(actionArgs[1].c_str());

    const std::vector<TimeServiceClientTimer*> timers = isDynamicTimer(timerObjectPath) ? dynamicTimers :
                                                        tsClient->getAnnouncedTimerList();

    TimeServiceClientTimer* timer = static_cast<TimeServiceClientTimer*>(findTsItem(timerObjectPath, timers));

    if (!timer) {

        printf("Timer object not found at the given ObjectPath: '%s'", timerObjectPath.c_str());
        return;
    }

    const std::vector<qcc::String>& langs = timer->retrieveDescriptionLanguages();
    qcc::String lang = langs.size() > 0 ? langs[0] : qcc::String::Empty;

    TimeServicePeriod interval;
    QStatus status   = timer->retrieveInterval(&interval);
    CHECK_LOG("retrieve Interval", status);

    TimeServicePeriod timeLeft;
    status   = timer->retrieveTimeLeft(&timeLeft);
    CHECK_LOG("retrieve TimeLeft", status);

    bool isRunning;
    status   = timer->retrieveIsRunning(isRunning);
    CHECK_LOG("retrieve IsRunning", status);

    uint16_t repeat;
    status   = timer->retrieveRepeat(repeat);
    CHECK_LOG("retrieve Repeat", status);

    qcc::String title;
    status = timer->retrieveTitle(&title);
    CHECK_LOG("retrieve Title", status);

    printf("Timer: IsRunning: '%s', Repeat: '%u', Title: '%s', Description: '%s' \n",
           isRunning ? "YES" : "NO",
           (unsigned int) repeat,
           title.c_str(),
           lang.length() > 0 ? timer->retrieveObjectDescription(lang).c_str() : "");

    sampleTestUtils::printPeriod(interval, "Interval");
    sampleTestUtils::printPeriod(timeLeft, "Time left");
}

//Set Timer
void setTimer(const std::vector<std::string>& actionArgs)
{

    QStatus status;

    if (!isClientUpCheckMandatoryArgumentsCnt(actionArgs, 3)) { // 1 more optional params

        printf("Timer input params are expected \n");
        return;
    }

    if (!tsClient) {

        std::cout
        << "Server Key hasn't been set. Use \"Set Server Key\" method." << std::endl;
        return;
    }

    qcc::String timerObjectPath(actionArgs[1].c_str());

    const std::vector<TimeServiceClientTimer*> timers = isDynamicTimer(timerObjectPath) ? dynamicTimers :
                                                        tsClient->getAnnouncedTimerList();

    TimeServiceClientTimer* timer = static_cast<TimeServiceClientTimer*>(findTsItem(timerObjectPath, timers));

    if (!timer) {

        printf("Timer object not found at the given ObjectPath: '%s'", timerObjectPath.c_str());
        return;
    }

    std::string intervalStr;
    std::string repeatsStr;
    std::string titleStr;

    intervalStr = actionArgs[2];

    if (actionArgs.size() >= 4) {

        repeatsStr = actionArgs[3];
    }

    if (actionArgs.size() >= 5) {

        titleStr = actionArgs[4];
    }

    TimeServicePeriod interval;

    if (!sampleTestUtils::periodFromString(intervalStr, interval)) {
        printf("Failed parsing interval\n");
        return;
    }

    status = timer->setInterval(interval);

    CHECK_LOG("set interval", status);

    short repeat = atoi(repeatsStr.c_str());

    if (0 == repeat) {
        repeat = tsConsts::TIMER_REPEAT_FOREVER;
    }

    status = timer->setRepeat(repeat);
    CHECK_LOG("set repeat", status);

    if (titleStr.length() > 0) {

        qcc::String myTitle(titleStr.c_str());
        status = timer->setTitle(myTitle);
        CHECK_LOG("set Title", status);
    }
}

//Timer Signal
void timerSignal(const std::vector<std::string>& actionArgs)
{

    if (!isClientUpCheckMandatoryArgumentsCnt(actionArgs, 1)) {
        printf("Timer object path is expected \n");
        return;
    }

    if (!tsClient) {

        std::cout << "Server Key hasn't been set. Use \"Set Server Key\" method." << std::endl;
        return;
    }

    qcc::String timerObjectPath(actionArgs[1].c_str());

    const std::vector<TimeServiceClientTimer*> timers = isDynamicTimer(timerObjectPath) ? dynamicTimers :
                                                        tsClient->getAnnouncedTimerList();

    TimeServiceClientTimer* timer = static_cast<TimeServiceClientTimer*>(findTsItem(timerObjectPath, timers));

    if (!timer) {

        printf("Timer object not found at the given ObjectPath: '%s'",                timerObjectPath.c_str());
        return;
    }

    if (!signalHandler) {

        signalHandler = new TimeClientSignalHandler();
    }

    QStatus status = timer->registerTimerHandler(signalHandler);
    CHECK_LOG("register timer signal handler", status);
}

//Timer Signal Stop
void timerSignalStop(const std::vector<std::string>& actionArgs)
{

    if (!isClientUpCheckMandatoryArgumentsCnt(actionArgs, 1)) {

        printf("Timer object path is expected \n");
        return;
    }

    if (!tsClient) {

        std::cout
        << "Server Key hasn't been set. Use \"Set Server Key\" method." << std::endl;
        return;
    }

    qcc::String timerObjectPath(actionArgs[1].c_str());

    const std::vector<TimeServiceClientTimer*> timers = isDynamicTimer(timerObjectPath) ? dynamicTimers :
                                                        tsClient->getAnnouncedTimerList();

    TimeServiceClientTimer* timer = static_cast<TimeServiceClientTimer*>(findTsItem(timerObjectPath, timers));

    if (!timer) {

        printf("Timer object not found at the given ObjectPath: '%s'",
               timerObjectPath.c_str());
        return;
    }

    timer->unRegisterTimerHandler();
}

//Timer Start
void timerStart(const std::vector<std::string>& actionArgs)
{

    if (!isClientUpCheckMandatoryArgumentsCnt(actionArgs, 1)) {

        printf("Timer object path is expected \n");
        return;
    }

    if (!tsClient) {

        std::cout
        << "Server Key hasn't been set. Use \"Set Server Key\" method." << std::endl;
        return;
    }

    qcc::String timerObjectPath(actionArgs[1].c_str());

    const std::vector<TimeServiceClientTimer*> timers = isDynamicTimer(timerObjectPath) ? dynamicTimers :
                                                        tsClient->getAnnouncedTimerList();

    TimeServiceClientTimer* timer = static_cast<TimeServiceClientTimer*>(findTsItem(timerObjectPath, timers));

    if (!timer) {

        printf("Timer object not found at the given ObjectPath: '%s'", timerObjectPath.c_str());
        return;
    }

    QStatus status = timer->start();
    CHECK_LOG("start timer", status);
}

//Timer Pause
void timerPause(const std::vector<std::string>& actionArgs)
{

    if (!isClientUpCheckMandatoryArgumentsCnt(actionArgs, 1)) {

        printf("Timer object path is expected \n");
        return;
    }

    if (!tsClient) {

        std::cout << "Server Key hasn't been set. Use \"Set Server Key\" method." << std::endl;
        return;
    }

    qcc::String timerObjectPath(actionArgs[1].c_str());

    const std::vector<TimeServiceClientTimer*> timers = isDynamicTimer(timerObjectPath) ? dynamicTimers :
                                                        tsClient->getAnnouncedTimerList();

    TimeServiceClientTimer* timer = static_cast<TimeServiceClientTimer*>(findTsItem(timerObjectPath, timers));

    if (!timer) {

        printf("Timer object not found at the given ObjectPath: '%s'", timerObjectPath.c_str());
        return;
    }

    QStatus status = timer->pause();
    CHECK_LOG("pause timer", status);
}

//Timer Reset
void timerReset(const std::vector<std::string>& actionArgs)
{
    if (!isClientUpCheckMandatoryArgumentsCnt(actionArgs, 1)) {
        printf("Timer object path is expected \n");
        return;
    }

    if (!tsClient) {

        std::cout
        << "Server Key hasn't been set. Use \"Set Server Key\" method." << std::endl;
        return;
    }

    qcc::String timerObjectPath(actionArgs[1].c_str());

    const std::vector<TimeServiceClientTimer*> timers = isDynamicTimer(timerObjectPath) ? dynamicTimers :
                                                        tsClient->getAnnouncedTimerList();

    TimeServiceClientTimer* timer = static_cast<TimeServiceClientTimer*>(findTsItem(timerObjectPath, timers));

    if (!timer) {

        printf("Timer object not found at the given ObjectPath: '%s'", timerObjectPath.c_str());
        return;
    }

    QStatus status = timer->reset();
    CHECK_LOG("reset timer", status);
}

//Factory New Timer
void factoryNewTimer(const std::vector<std::string>& actionArgs)
{

    if (!isClientUpCheckMandatoryArgumentsCnt(actionArgs, 1)) {

        printf("TimerFactory object path is expected \n");
        return;
    }

    if (!tsClient) {

        std::cout << "Server Key hasn't been set. Use \"Set Server Key\" method." << std::endl;
        return;
    }

    qcc::String timerFactoryObjectPath(actionArgs[1].c_str());

    TimeServiceClientTimerFactory* timerFactory = static_cast<TimeServiceClientTimerFactory*>(findTsItem(timerFactoryObjectPath,
                                                                                                         tsClient->getAnnouncedTimerFactoryList()));

    if (!timerFactory) {

        printf("TimerFactory object not found at the given ObjectPath: '%s'", timerFactoryObjectPath.c_str());
        return;
    }

    qcc::String newTimerObjPath;
    QStatus status = timerFactory->newTimer(&newTimerObjPath);
    CHECK_LOG("create New Timer", status);

    printf("Created NewTimer: '%s' \n", newTimerObjPath.c_str());

    TimeServiceClientTimer* timer = new TimeServiceClientTimer(*tsClient, newTimerObjPath);
    dynamicTimers.push_back(timer);
}

//Factory Delete Timer
void factoryDeleteTimer(const std::vector<std::string>& actionArgs)
{

    if (!isClientUpCheckMandatoryArgumentsCnt(actionArgs, 2)) {

        printf("The Object paths of the TimerFactory and Timer to be deleted are expected \n");
        return;
    }

    if (!tsClient) {

        std::cout << "Server Key hasn't been set. Use \"Set Server Key\" method." << std::endl;
        return;
    }

    qcc::String timerFactoryObjectPath(actionArgs[1].c_str());
    qcc::String deleteTimerObjectPath(actionArgs[2].c_str());

    TimeServiceClientTimerFactory* timerFactory = static_cast<TimeServiceClientTimerFactory*>(findTsItem(timerFactoryObjectPath,
                                                                                                         tsClient->getAnnouncedTimerFactoryList()));

    if (!timerFactory) {

        printf("TimerFactory object not found at the given ObjectPath: '%s'", timerFactoryObjectPath.c_str());
        return;
    }

    QStatus status = timerFactory->deleteTimer(deleteTimerObjectPath);
    CHECK_LOG("DeleteTimer", status);

    std::vector<TimeServiceClientTimer*>::iterator iter = dynamicTimers.begin();
    while (iter != dynamicTimers.end()) {

        if ((*iter)->getObjectPath().compare(deleteTimerObjectPath) == 0) {

            printf("Deleting Timer: '%s' locally \n", deleteTimerObjectPath.c_str());
            delete *iter;
            iter = dynamicTimers.erase(iter);
        } else {

            ++iter;
        }
    }
}

//Factory Get Timers
void factoryGetTimers(const std::vector<std::string>& actionArgs)
{

    if (!isClientUpCheckMandatoryArgumentsCnt(actionArgs, 1)) {

        printf("TimerFactory object path is expected \n");
        return;
    }

    if (!tsClient) {

        std::cout << "Server Key hasn't been set. Use \"Set Server Key\" method." << std::endl;
        return;
    }

    qcc::String timerFactoryObjectPath(actionArgs[1].c_str());

    TimeServiceClientTimerFactory* timerFactory = static_cast<TimeServiceClientTimerFactory*>(findTsItem(timerFactoryObjectPath,
                                                                                                         tsClient->getAnnouncedTimerFactoryList()));

    if (!timerFactory) {

        printf("TimerFactory object not found at the given ObjectPath: '%s'", timerFactoryObjectPath.c_str());
        return;
    }

    std::vector<qcc::String> factoryTimers;
    QStatus status = timerFactory->retrieveTimerList(&factoryTimers);
    CHECK_LOG("retrieve TimerList", status);

    printf("Releasing current Timers of this factory: '%s' and storing new Timers list \n", timerFactoryObjectPath.c_str());
    cleanDynamicTsItems(dynamicTimers, tsClient);

    for (std::vector<qcc::String>::iterator iter = factoryTimers.begin(); iter != factoryTimers.end(); ++iter) {

        printf("New Factory Timer: '%s' \n", (*iter).c_str());
        TimeServiceClientTimer* timer = new TimeServiceClientTimer(*tsClient, *iter);
        dynamicTimers.push_back(timer);
    }
}

/**
 * OnAnnouncement callback
 */
void onAnnouncement(const qcc::String& busName,
                    const qcc::String& deviceId,
                    const qcc::String& appId,
                    const qcc::String& uniqKey,
                    const ajn::AboutObjectDescription& objectDescription)
{

    bus->EnableConcurrentCallbacks();

    printf("Received Announcement from: '%s', Key: '%s' \n", busName.c_str(), uniqKey.c_str());

    bool isCurrentServerKey = false;

    std::map<qcc::String, TimeServiceClient*>::iterator iter = timeClients.find(uniqKey);
    if (iter != timeClients.end()) {

        printf("An additional announcement with the same DeviceId_AppId: '%s' exists, releasing the original TimeServiceClient resources \n", uniqKey.c_str());

        if (0 == strcmp(uniqKey.c_str(), currentServerKey.c_str())) {

            isCurrentServerKey = true;
        }

        //Remove all the TimeService objects that were created dynamically by the factories of the original TimeServiceClient
        //that is going to be deleted below
        cleanDynamicTsItems(dynamicAlarms, iter->second);
        cleanDynamicTsItems(dynamicTimers, iter->second);

        delete iter->second;
        timeClients.erase(iter);
    }

    TimeServiceClient* timeClient = new TimeServiceClient();
    timeClient->init(bus, busName, deviceId, appId, objectDescription);

    timeClients.insert(std::pair<qcc::String, TimeServiceClient*>(uniqKey, timeClient));

    if (isCurrentServerKey) {

        tsClient = timeClient;
    }
}
