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
#include <iterator>
#include <signal.h>
#include <SrpKeyXListener.h>
#include <CommonSampleUtil.h>
#include <alljoyn/services_common/LogModulesNames.h>
#include <alljoyn/services_common/GuidUtil.h>
#include <alljoyn/time/TimeServiceServer.h>

#include "TestClock.h"
#include "TestAuthorityClock.h"
#include "TestAlarm.h"
#include "TestCustomAlarmBusObject.h"
#include "TestAlarmFactory.h"
#include "TestTimer.h"
#include "TestCustomTimerBusObject.h"
#include "TestTimerFactory.h"

#include <alljoyn/AboutObj.h>
#include <alljoyn/AboutData.h>

using namespace ajn;
using namespace services;
using namespace qcc;

// Set application constants
#define DEVICE_NAME "TimeService Server"
#define APP_NAME "Chronos Server"
#define LANG  "en"
#define SERVICE_PORT 900

BusAttachment* bus                           = NULL;
CommonBusListener* busListener               = NULL;
SrpKeyXListener* srpKeyXListener             = NULL;
AboutData* aboutData = NULL;
AboutObj* aboutObj = NULL;

TimeServiceServer* server                    = NULL;
std::map<qcc::String, TestClock*> clocks;
std::map<qcc::String, TestAuthorityClock*> authClocks;
std::map<qcc::String, TestAlarm*> alarms;
std::map<qcc::String, TestAlarmFactory*> alarmFactories;
std::map<qcc::String, TestTimer*> timers;
std::map<qcc::String, TestTimerFactory*> timerFactories;

static volatile sig_atomic_t s_interrupt     = false;

//===========================================================//

/**
 * Enum of possible TimeServiceServer actions
 */
typedef enum Actions {

    USAGE,
    START,
    STOP,
    ANNOUNCE,
    CREATE_CLOCK,
    CREATE_AUTH_CLOCK,
    SEND_TIME_SYNC,
    CREATE_ALARM,
    CREATE_CUSTOMALARM,
    ALARM_REACHED,
    CREATE_ALARM_FACTORY,
    CREATE_TIMER,
    CREATE_CUSTOMTIMER,
    TIMER_EVENT,
    TIMER_RUN_CHANGED,
    CREATE_TIMER_FACTORY,
    PRINT_OBJECT_PATHS,
} Actions;

//===========================================================//
//Time Service Server actions

//Start service
void startService(const std::vector<std::string>& actionArgs);

//Stop service
void stopService(const std::vector<std::string>& actionArgs);

//Stop service
void announce(const std::vector<std::string>& actionArgs);

//Create Clock
void createClock(const std::vector<std::string>& actionArgs);

//Create AuthClock
void createAuthClock(const std::vector<std::string>& actionArgs);

//Send TimeSync
void sendTimeSync(const std::vector<std::string>& actionArgs);

//Create Alarm
void createAlarm(const std::vector<std::string>& actionArgs);

//Create CustomAlarm
void createCustomAlarm(const std::vector<std::string>& actionArgs);

//Send alarm reached
void sendAlarmReached(const std::vector<std::string>& actionArgs);

//Create AlarmFactory
void createAlarmFactory(const std::vector<std::string>& actionArgs);

//Create Timer
void createTimer(const std::vector<std::string>& actionArgs);

//Create CustomTimer
void createCustomTimer(const std::vector<std::string>& actionArgs);

//Send timer event
void sendTimerEvent(const std::vector<std::string>& actionArgs);

//Send timer run state changed
void sendTimerRunStateChanged(const std::vector<std::string>& actionArgs);

//Create AlarmFactory
void createTimerFactory(const std::vector<std::string>& actionArgs);

//Print Object Paths
void printObjectPaths(const std::vector<std::string>& actionArgs);

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
 * Cleanup clocks
 */
void cleanClocks()
{

    if (clocks.size() == 0) {

        std::cout << "No clocks to cleanup" << std::endl;
        return;
    }

    std::cout << "Cleaning Clocks" << std::endl;

    for (std::map<qcc::String, TestClock*>::iterator i = clocks.begin(); i != clocks.end(); ++i) {

        delete i->second;
    }

    clocks.clear();
}

/**
 * Cleanup clocks
 */
void cleanAuthClocks()
{

    if (authClocks.size() == 0) {

        std::cout << "No AuthClocks to cleanup" << std::endl;
        return;
    }

    std::cout << "Cleaning Authority Clocks" << std::endl;

    for (std::map<qcc::String, TestAuthorityClock*>::iterator i = authClocks.begin(); i != authClocks.end(); ++i) {

        delete i->second;
    }

    authClocks.clear();
}


/**
 * Cleanup alarms
 */
void cleanAlarms()
{

    if (alarms.size() == 0) {

        std::cout << "No alarms to cleanup" << std::endl;
        return;
    }

    std::cout << "Cleaning alarms" << std::endl;

    for (std::map<qcc::String, TestAlarm*>::iterator i = alarms.begin(); i != alarms.end(); ++i) {

        delete i->second;
    }

    alarms.clear();
}

/**
 * Cleanup AlarmFactories
 */
void cleanAlarmFactories()
{

    if (alarmFactories.size() == 0) {

        std::cout << "No AlarmFactories to cleanup" << std::endl;
        return;
    }

    std::cout << "Cleaning AlarmFactories" << std::endl;

    for (std::map<qcc::String, TestAlarmFactory*>::iterator i = alarmFactories.begin(); i != alarmFactories.end(); ++i) {

        delete i->second;
    }

    alarmFactories.clear();
}

/**
 * Cleanup alarms
 */
void cleanTimers()
{

    if (alarms.size() == 0) {

        std::cout << "No timers to cleanup" << std::endl;
        return;
    }

    std::cout << "Cleaning timers" << std::endl;

    for (std::map<qcc::String, TestTimer*>::iterator i = timers.begin(); i != timers.end(); ++i) {

        delete i->second;
    }

    timers.clear();
}

/**
 * Cleanup AlarmFactories
 */
void cleanTimerFactories()
{

    if (timerFactories.size() == 0) {

        std::cout << "No AlarmFactories to cleanup" << std::endl;
        return;
    }

    std::cout << "Cleaning AlarmFactories" << std::endl;

    for (std::map<qcc::String, TestTimerFactory*>::iterator i = timerFactories.begin(); i != timerFactories.end(); ++i) {

        delete i->second;
    }

    timerFactories.clear();
}

/**
 * Clean Time Objects
 */
void cleanupTimeObjects()
{

    cleanClocks();
    cleanAuthClocks();
    cleanAlarms();
    cleanAlarmFactories();
    cleanTimers();
    cleanTimerFactories();
}

/**
 * Cleaning up resources
 */
void cleanup()
{

    if (bus && busListener) {
        if (AboutObjApi::getInstance()) {
            AboutObjApi::DestroyInstance();
        }
    }

    if (aboutData) {
        delete aboutData;
        aboutData = NULL;
    }

    if (aboutObj) {
        delete aboutObj;
        aboutObj = NULL;
    }

    if (server) {

        cleanupTimeObjects();

        delete server;
        server = NULL;
    }

    if (bus) {

        delete bus;
        bus = NULL;
    }

    if (busListener) {

        delete busListener;
        busListener = NULL;
    }

    if (srpKeyXListener) {

        delete srpKeyXListener;
        srpKeyXListener = NULL;
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

    std::cout << "******************************************************************" << std::endl;
    std::cout << "* Select possible action to execute and press [Enter],           *" << std::endl;
    std::cout << "* to stop this application press [Ctrl+C] and [Enter].           *" << std::endl;
    std::cout << "* o Denotes optional param                                       *" << std::endl;
    std::cout << "* Usage                     '0'                                  *" << std::endl;
    std::cout << "* Start Service             '1'                                  *" << std::endl;
    std::cout << "* Stop Service              '2'                                  *" << std::endl;
    std::cout << "* Announcement              '3'                                  *" << std::endl;
    std::cout << "* Create Clock              '4'                                  *" << std::endl;
    std::cout << "* Create Authority Clock    '5' oAuthType: 0-4                   *" << std::endl;
    std::cout << "* Send TimeSync             '6' OBJ: '/TimeAuthority...'         *" << std::endl;
    std::cout << "* Create Alarm              '7' oDESC: 'regularCool'             *" << std::endl;
    std::cout << "* Create Custom Alarm       '8' oDESC: 'superCool'               *" << std::endl;
    std::cout << "* Send AlarmReached         '9' OBJ: '/Alarm...'                 *" << std::endl;
    std::cout << "* Create Alarm Factory     '10' oDESC: 'CoolFactory'             *" << std::endl;
    std::cout << "* Create Timer             '11' oDESC: 'regularTimer'            *" << std::endl;
    std::cout << "* Create Custom Timer      '12' oDESC: 'customTimer'             *" << std::endl;
    std::cout << "* Send TimerEvent          '13' OBJ: '/Timer...'                 *" << std::endl;
    std::cout << "* Send RunStateChanged     '14' OBJ: '/Timer...' BOOL:0/1        *" << std::endl;
    std::cout << "* Create Timer Factory     '15' oDESC: 'timerFactory'            *" << std::endl;
    std::cout << "* Print Object Paths       '16'                                  *" << std::endl;
    std::cout << "******************************************************************" << std::endl;
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
 * Starts Time Server.
 * Receives commands from STDIN
 */
void startTimeServer()
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
 * Populate TimeServiceServer actions
 */
void populateActions()
{

    actions.insert(std::pair<Actions, ActionFunction>(START, startService));
    actions.insert(std::pair<Actions, ActionFunction>(STOP, stopService));
    actions.insert(std::pair<Actions, ActionFunction>(ANNOUNCE, announce));
    actions.insert(std::pair<Actions, ActionFunction>(CREATE_CLOCK, createClock));
    actions.insert(std::pair<Actions, ActionFunction>(CREATE_AUTH_CLOCK, createAuthClock));
    actions.insert(std::pair<Actions, ActionFunction>(SEND_TIME_SYNC, sendTimeSync));
    actions.insert(std::pair<Actions, ActionFunction>(CREATE_ALARM, createAlarm));
    actions.insert(std::pair<Actions, ActionFunction>(CREATE_CUSTOMALARM, createCustomAlarm));
    actions.insert(std::pair<Actions, ActionFunction>(ALARM_REACHED, sendAlarmReached));
    actions.insert(std::pair<Actions, ActionFunction>(CREATE_ALARM_FACTORY, createAlarmFactory));
    actions.insert(std::pair<Actions, ActionFunction>(CREATE_TIMER, createTimer));
    actions.insert(std::pair<Actions, ActionFunction>(CREATE_CUSTOMTIMER, createCustomTimer));
    actions.insert(std::pair<Actions, ActionFunction>(TIMER_EVENT, sendTimerEvent));
    actions.insert(std::pair<Actions, ActionFunction>(TIMER_RUN_CHANGED, sendTimerRunStateChanged));
    actions.insert(std::pair<Actions, ActionFunction>(CREATE_TIMER_FACTORY, createTimerFactory));
    actions.insert(std::pair<Actions, ActionFunction>(PRINT_OBJECT_PATHS, printObjectPaths));
}

/**
 * Main
 */
int main()
{
    // Allow CTRL+C to end application
    signal(SIGINT, SigIntHandler);
    std::cout << "Beginning TimeService Server Application. (Press CTRL+C and enter or CTRL+D to end application)" << std::endl;

    QCC_SetDebugLevel(logModules::TIME_MODULE_LOG_NAME, logModules::ALL_LOG_LEVELS);

    //Initialize AuthListener
    srpKeyXListener = new SrpKeyXListener();

    //Create BusAttachment
    bus = CommonSampleUtil::prepareBusAttachment(srpKeyXListener);
    if (bus == NULL) {

        std::cout << "Could not initialize BusAttachment." << std::endl;
        return 1;
    }

    qcc::String deviceId;
    GuidUtil::GetInstance()->GetDeviceIdString(&deviceId);
    qcc::String appId;
    GuidUtil::GetInstance()->GenerateGUID(&appId);

    //Initialize and fill About Data
    DeviceNamesType deviceNames;
    deviceNames.insert(std::pair<qcc::String, qcc::String>(LANG, DEVICE_NAME));
    aboutData = new AboutData(LANG);

    QStatus status = CommonSampleUtil::fillAboutData(aboutData, appId, APP_NAME, deviceId, deviceNames);
    if (status != ER_OK) {

        std::cout << "Could not fill About Data." << std::endl;
        cleanup();
        return 1;
    }

    //Initialize About Server
    busListener = new CommonBusListener();
    aboutObj = new AboutObj(*bus, BusObject::ANNOUNCED);
    status      = CommonSampleUtil::prepareAboutService(bus, aboutData, aboutObj,
                                                        busListener, SERVICE_PORT);
    if (status != ER_OK) {

        std::cout << "Could not set up the AboutService." << std::endl;
        cleanup();
        return 1;
    }

    std::cout << "Created BusAttachment bus: '" << bus->GetUniqueName() << "'" << std::endl;

    //Populate actions
    populateActions();

    //Do the TimeServiceServer work
    startTimeServer();

    //Cleanup
    cleanup();

    return 0;
}

//======================================================//
//              TIME SERVICE SERVER FUNCTIONS           //
//======================================================//

/**
 * Start the Time Service Server
 */
void startService(const std::vector<std::string>& actionArgs)
{

    std::cout << "Initializing TimeServiceServer" << std::endl;

    //TimeServiceServer::getInstance();
    server           = TimeServiceServer::getInstance();
    QStatus status   =  server->init(bus);
    if (status != ER_OK) {

        std::cout << "Failed to start the TimeServiceServer " << status << std::endl;
        return;
    }
}

/**
 * Stop the TimeServiceServer
 */
void stopService(const std::vector<std::string>& actionArgs)
{

    if (server) {

        cleanupTimeObjects();
        server->shutdown();
    } else {

        std::cout <<  "The server hasn't been initialized" << std::endl;
    }
}

/**
 * Send Announcement
 */
void announce(const std::vector<std::string>& actionArgs)
{

    if (!server || !server->isStarted()) {

        std::cout << "Server hasn't been started" << std::endl;
        return;
    }

    std::cout << "Sending Announcement" << std::endl;

    QStatus status = CommonSampleUtil::aboutServiceAnnounce();
    if (status != ER_OK) {

        std::cout << "Failed to send Announcement " << status << std::endl;
    }
}

/**
 * Create Clock
 */
void createClock(const std::vector<std::string>& actionArgs)
{

    if (!server || !server->isStarted()) {

        std::cout << "Server hasn't been started" << std::endl;
        return;
    }

    TestClock* clock = new TestClock();
    QStatus status   = server->activateClock(clock);

    if (status != ER_OK) {

        std::cout << "Failed to create the Clock" << std::endl;
        delete clock;

        return;
    }

    clocks.insert(std::pair<qcc::String, TestClock*>(clock->getObjectPath(), clock));
}

/**
 * Create Auth Clock
 */
void createAuthClock(const std::vector<std::string>& actionArgs)
{

    if (!server || !server->isStarted()) {

        std::cout << "Server hasn't been started" << std::endl;
        return;
    }

    tsConsts::ClockAuthorityType authType;
    int argType;

    if (actionArgs.size() > 1) {

        argType = strToInt(actionArgs[1]);
    } else {

        argType = 0;
    }

    authType = static_cast<tsConsts::ClockAuthorityType>(argType);

    TestAuthorityClock* clock = new TestAuthorityClock();
    QStatus status            = server->activateTimeAuthorityClock(clock, authType);

    if (status != ER_OK) {

        std::cout << "Failed to create the AuthorityClock" << std::endl;
        delete clock;

        return;
    }

    authClocks.insert(std::pair<qcc::String, TestAuthorityClock*>(clock->getObjectPath(), clock));
}

/**
 * Send TimeSync signal
 */
void sendTimeSync(const std::vector<std::string>& actionArgs)
{

    if (!server || !server->isStarted()) {

        std::cout << "Server hasn't been started" << std::endl;
        return;
    }

    if (actionArgs.size() <= 1) {

        std::cout << "TimeAuthority object path is expected" << std::endl;
        return;
    }

    qcc::String objPath(actionArgs[1].c_str());

    std::map<qcc::String, TestAuthorityClock*>::iterator iter = authClocks.find(objPath);

    if (iter == authClocks.end()) {

        std::cout << "Not found AuthorityClock with objectPath: '" << objPath.c_str() << "'" << std::endl;
        return;
    }

    TestAuthorityClock* authClock = iter->second;

    std::cout << "Emitting TimeSync signal" << std::endl;
    QStatus status = authClock->timeSync();

    if (status != ER_OK) {

        std::cout << "Failed to emit TimeSync signal, status: '" << status << std::endl;
    }
}

/**
 * Create Alarm
 */
void createAlarm(const std::vector<std::string>& actionArgs)
{

    if (!server || !server->isStarted()) {

        std::cout << "Server hasn't been started" << std::endl;
        return;
    }

    TestAlarm* alarm = new TestAlarm();
    QStatus status;

    if (actionArgs.size() <= 1) {

        status   = server->activateAlarm(alarm);
    } else {

        status   = server->activateAlarm(alarm, actionArgs[1].c_str(), "en", NULL);
    }

    if (status != ER_OK) {

        std::cout << "Failed to create the Alarm" << std::endl;
        delete alarm;

        return;
    }

    alarms.insert(std::pair<qcc::String, TestAlarm*>(alarm->getObjectPath(), alarm));
}

/**
 * Create Alarm
 */
void createCustomAlarm(const std::vector<std::string>& actionArgs)
{
    QStatus status;

    if (!server || !server->isStarted()) {

        std::cout << "Server hasn't been started" << std::endl;
        return;
    }

    TestAlarm* alarm = new TestAlarm();

    qcc::String objectPath("/coolAlarm");
    server->generateObjectPath(&objectPath);
    TestCustomAlarmBusObject* alarmBusObj = new TestCustomAlarmBusObject(objectPath);

    std::vector<qcc::String> notAnnounced;
    // to stop the custom interface from being announced use: notAnnounced.push_back("org.allseen.Time.Alarm.CoolAlarm");
    notAnnounced.push_back("");

    if (actionArgs.size() <= 1) {

        status   = server->registerCustomAlarm(alarmBusObj, alarm, notAnnounced);
    } else {

        status   = server->registerCustomAlarm(alarmBusObj, alarm, notAnnounced, actionArgs[1].c_str(), "en", NULL);
    }

    if (status != ER_OK) {

        std::cout << "Failed to create the Alarm" << std::endl;
        delete alarm;
        delete alarmBusObj;

        return;
    }

    alarms.insert(std::pair<qcc::String, TestAlarm*>(alarm->getObjectPath(), alarm));
}

/**
 * If object path is AlarmFactory so search for the Alarm in the
 * appropriate AlarmFactory object, otherwise
 * Search for the Alarm in the alarms map
 *
 * @param objPath Alarm object path to look for
 * @return TestAlarm* or NULL if the object wasn't found
 */
TestAlarm* findAlarm(const qcc::String& objPath)
{

    const qcc::String factoryPrefix("/AlarmFactory");
    qcc::String lookupPath = objPath.substr(0, factoryPrefix.size());

    //Check that the objectPath belongs to the "/AlarmFactory"
    if (0 == strcmp(factoryPrefix.c_str(), lookupPath.c_str())) {

        //Cut the AlarmFactory object path - cut from 0 index until "/" character
        lookupPath = objPath.substr(0, objPath.find("/", 1));

        //Search for the AlarmFactory that created the Alarm
        std::map<qcc::String, TestAlarmFactory*>::iterator iter = alarmFactories.find(lookupPath);

        if (iter == alarmFactories.end()) {

            std::cout << "Not found AlarmFactory with objectPath: '" << lookupPath.c_str() << "'" << std::endl;
            return NULL;
        }

        //Search for the Alarm in the AlarmFactory's list
        TestAlarm* alarm = NULL;
        iter->second->findAlarm(objPath, &alarm);

        return alarm;
    }

    //The Alarm doesn't belong to the AlarmFactory
    std::map<qcc::String, TestAlarm*>::iterator iter = alarms.find(objPath);

    if (iter == alarms.end()) {

        std::cout << "Not found Alarm with objectPath: '" << objPath.c_str() << "'" << std::endl;
        return NULL;
    }

    return iter->second;
}

/**
 * If object path is TimerFactory so search for the Timer in the
 * appropriate TiemrFactory object, otherwise
 * Search for the Timer in the timers map
 *
 * @param objPath Timer object path to look for
 * @return TestTimer* or NULL if the object wasn't found
 */
TestTimer* findTimer(const qcc::String& objPath)
{

    const qcc::String factoryPrefix("/TimerFactory");
    qcc::String lookupPath = objPath.substr(0, factoryPrefix.size());

    //Check that the objectPath belongs to the "/TimerFactory"
    if (0 == strcmp(factoryPrefix.c_str(), lookupPath.c_str())) {

        //Cut the TimerFactory object path - cut from 0 index until "/" character
        lookupPath = objPath.substr(0, objPath.find("/", 1));

        //Search for the TimerFactory that created the Timer
        std::map<qcc::String, TestTimerFactory*>::iterator iter = timerFactories.find(lookupPath);

        if (iter == timerFactories.end()) {

            std::cout << "Not found TimerFactory with objectPath: '" << lookupPath.c_str() << "'" << std::endl;
            return NULL;
        }

        //Search for the Timer in the TimerFactory's list
        TestTimer* timer = NULL;
        iter->second->findTimer(objPath, &timer);

        return timer;
    }

    //The Timer doesn't belong to the TimerFactory
    std::map<qcc::String, TestTimer*>::iterator iter = timers.find(objPath);

    if (iter == timers.end()) {

        std::cout << "Not found Timer with objectPath: '" << objPath.c_str() << "'" << std::endl;
        return NULL;
    }

    return iter->second;
}

/**
 * Create CustomAlarm
 */
void sendAlarmReached(const std::vector<std::string>& actionArgs)
{

    if (!server || !server->isStarted()) {

        std::cout << "Server hasn't been started" << std::endl;
        return;
    }

    if (actionArgs.size() <= 1) {

        std::cout << "Alarm object path is expected" << std::endl;
        return;
    }

    qcc::String objPath(actionArgs[1].c_str());
    TestAlarm* alarm = findAlarm(objPath);

    if (!alarm) {

        std::cout << "Not found Alarm with objectPath: '" << objPath.c_str() << "'" << std::endl;
        return;
    }

    std::cout << "Emitting AlarmReached signal" << std::endl;
    QStatus status = alarm->alarmReached();

    if (status != ER_OK) {

        std::cout << "Failed to emit AlarmReached signal, status: '" << status << std::endl;
    }
}

/**
 * Create AlarmFactory
 */
void createAlarmFactory(const std::vector<std::string>& actionArgs)
{

    QStatus status;

    if (!server || !server->isStarted()) {

        std::cout << "Server hasn't been started" << std::endl;
        return;
    }

    TestAlarmFactory* alarmFactory = new TestAlarmFactory();

    if (actionArgs.size() <= 1) {

        status = server->activateAlarmFactory(alarmFactory);
    } else {

        status = server->activateAlarmFactory(alarmFactory, actionArgs[1].c_str(), "en", NULL);
    }

    if (status != ER_OK) {

        std::cout << "Failed to create the AlarmFactory" << std::endl;

        delete alarmFactory;
        return;
    }

    alarmFactories.insert(std::pair<qcc::String, TestAlarmFactory*>(alarmFactory->getObjectPath(), alarmFactory));
}

/**
 * Create Timer
 */
void createTimer(const std::vector<std::string>& actionArgs)
{

    if (!server || !server->isStarted()) {

        std::cout << "Server hasn't been started" << std::endl;
        return;
    }

    TestTimer* Timer = new TestTimer();
    QStatus status;

    if (actionArgs.size() <= 1) {

        status   = server->activateTimer(Timer);
    } else {

        status   = server->activateTimer(Timer, actionArgs[1].c_str(), "en", NULL);
    }

    if (status != ER_OK) {

        std::cout << "Failed to create the Timer" << std::endl;
        delete Timer;

        return;
    }

    timers.insert(std::pair<qcc::String, TestTimer*>(Timer->getObjectPath(), Timer));
}

/**
 * Create Timer
 */
void createCustomTimer(const std::vector<std::string>& actionArgs)
{
    QStatus status;

    if (!server || !server->isStarted()) {

        std::cout << "Server hasn't been started" << std::endl;
        return;
    }

    TestTimer* timer = new TestTimer();

    qcc::String objectPath("/coolTimer");
    server->generateObjectPath(&objectPath);
    TestCustomTimerBusObject* TimerBusObj = new TestCustomTimerBusObject(objectPath);

    std::vector<qcc::String> notAnnounced;
    // to stop the custom interface from being announced use: notAnnounced.push_back("org.allseen.Time.Timer.CoolTimer");
    notAnnounced.push_back("");

    if (actionArgs.size() <= 1) {

        status   = server->registerCustomTimer(TimerBusObj, timer, notAnnounced);
    } else {

        status   = server->registerCustomTimer(TimerBusObj, timer, notAnnounced, actionArgs[1].c_str(), "en", NULL);
    }

    if (status != ER_OK) {

        std::cout << "Failed to create the Timer" << std::endl;
        delete timer;
        delete TimerBusObj;

        return;
    }

    timers.insert(std::pair<qcc::String, TestTimer*>(timer->getObjectPath(), timer));
}

/**
 * Send timer event
 */
void sendTimerEvent(const std::vector<std::string>& actionArgs)
{

    if (!server || !server->isStarted()) {

        std::cout << "Server hasn't been started" << std::endl;
        return;
    }

    if (actionArgs.size() <= 1) {

        std::cout << "Timer object path is expected" << std::endl;
        return;
    }

    qcc::String objPath(actionArgs[1].c_str());

    TestTimer* timer = findTimer(objPath);

    if (!timer) {

        std::cout << "Not found Timer with objectPath: '" << objPath.c_str() << "'" << std::endl;
        return;
    }

    std::cout << "Emitting TimerReached signal" << std::endl;
    QStatus status = timer->timerEvent();

    if (status != ER_OK) {

        std::cout << "Failed to emit TimerReached signal, status: '" << status << std::endl;
    }
}

/**
 * Send timer run state changed
 */
void sendTimerRunStateChanged(const std::vector<std::string>& actionArgs)
{

    if (!server || !server->isStarted()) {

        std::cout << "Server hasn't been started" << std::endl;
        return;
    }

    if (actionArgs.size() <= 2) {

        std::cout << "Timer object path and is running bool are expected" << std::endl;
        return;
    }

    qcc::String objPath(actionArgs[1].c_str());

    TestTimer* timer = findTimer(objPath);

    if (!timer) {

        std::cout << "Not found Timer with objectPath: '" << objPath.c_str() << "'" << std::endl;
        return;
    }

    bool isRunning = true;

    if ((actionArgs[2].c_str()[0] == '0') || (toupper(actionArgs[2].c_str()[0]) == 'N')) {
        isRunning = false;
    }

    std::cout << "Emitting TimerReached signal" << std::endl;
    QStatus status = timer->runStateChanged(isRunning);

    if (status != ER_OK) {

        std::cout << "Failed to emit TimerReached signal, status: '" << status << "'" << std::endl;
    }
}

/**
 * Create TimerFactory
 */
void createTimerFactory(const std::vector<std::string>& actionArgs)
{

    QStatus status;

    if (!server || !server->isStarted()) {

        std::cout << "Server hasn't been started" << std::endl;
        return;
    }

    TestTimerFactory* timerFactory = new TestTimerFactory();

    if (actionArgs.size() <= 1) {

        status = server->activateTimerFactory(timerFactory);
    } else {

        status = server->activateTimerFactory(timerFactory, actionArgs[1].c_str(), "en", NULL);
    }

    if (status != ER_OK) {

        std::cout << "Failed to create the TimerFactory" << std::endl;

        delete timerFactory;
        return;
    }

    timerFactories.insert(std::pair<qcc::String, TestTimerFactory*>(timerFactory->getObjectPath(), timerFactory));
}

/**
 * Create TimerFactory
 */
void printObjectPaths(const std::vector<std::string>& actionArgs)
{
    if (!server || !server->isStarted()) {

        std::cout << "Server hasn't been started" << std::endl;
        return;
    }

    printf("Clocks:\n");
    for (std::map<qcc::String, TestClock*>::iterator i = clocks.begin(); i != clocks.end(); ++i) {

        printf("\t%s\n", i->first.c_str());
    }

    printf("authClocks:\n");
    for (std::map<qcc::String, TestAuthorityClock*>::iterator i = authClocks.begin(); i != authClocks.end(); ++i) {

        printf("\t%s\n", i->first.c_str());
    }

    printf("Alarms:\n");
    for (std::map<qcc::String, TestAlarm*>::iterator i = alarms.begin(); i != alarms.end(); ++i) {

        printf("\t%s\n", i->first.c_str());
    }

    printf("alarmFactories:\n");
    for (std::map<qcc::String, TestAlarmFactory*>::iterator i = alarmFactories.begin(); i != alarmFactories.end(); ++i) {

        printf("\t%s:\n", i->first.c_str());
        i->second->printAlarmObjectPaths();
    }

    printf("Timers:\n");
    for (std::map<qcc::String, TestTimer*>::iterator i = timers.begin(); i != timers.end(); ++i) {

        printf("\t%s\n", i->first.c_str());
    }

    printf("timerFactories:\n");
    for (std::map<qcc::String, TestTimerFactory*>::iterator i = timerFactories.begin(); i != timerFactories.end(); ++i) {

        printf("\t%s:\n", i->first.c_str());
        i->second->printTimerObjectPaths();

    }
}

