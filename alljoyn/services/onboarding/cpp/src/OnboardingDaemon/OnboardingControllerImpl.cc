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

#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <errno.h>
#include <algorithm>
#include <time.h>
#ifdef _WIN32
#include <process.h>
#endif

#include <OnboardingControllerImpl.h>
#include <alljoyn/onboarding/LogModule.h>

#define CMD_SIZE 255
#define CLOCKID CLOCK_REALTIME
#define SIG SIGUSR1
#define SCAN_WIFI_MAX_TIME_IN_SEC 30

using namespace ajn;
using namespace services;

static int execute_system(const char*op);

typedef enum {
    CIPHER_NONE,
    CIPHER_TKIP,
    CIPHER_CCMP,
    CIPHER_BOTH
}GroupCiphers;

#ifdef _OPEN_WRT_
#define CASE(_auth) case _auth: return # _auth
static const char* AuthText(short authType)
{
    switch (authType) {
        CASE(WPA2_AUTO);
        CASE(WPA_AUTO);
        CASE(OPEN);
        CASE(WEP);
        CASE(WPA_TKIP);
        CASE(WPA_CCMP);
        CASE(WPA2_TKIP);
        CASE(WPA2_CCMP);
        CASE(WPS);

    default:
        // This function directly feeds the configure scripts - it should return a valid value
        // if all else fails, rather than ANY.
        return "OPEN";
    }
}
#endif

OnboardingControllerImpl::OnboardingControllerImpl(qcc::String scanFile,
                                                   qcc::String stateFile,
                                                   qcc::String errorFile,
                                                   qcc::String configureCmd,
                                                   qcc::String connectCmd,
                                                   qcc::String offboardCmd,
                                                   qcc::String scanCmd,
                                                   OBConcurrency concurrency,
                                                   BusAttachment& busAttachment) :
    m_state(0),
    m_ScanArray(NULL),
    m_BusAttachment(&busAttachment),
    m_scanFile(scanFile),
    m_stateFile(stateFile),
    m_errorFile(errorFile),
    m_configureCmd(configureCmd),
    m_connectCmd(connectCmd),
    m_offboardCmd(offboardCmd),
    m_scanCmd(scanCmd),
    m_concurrency(concurrency),
    m_scanWifiThreadIsRunning(false)
#ifndef _WIN32
    , m_scanTimerId(0)
#endif
{
    // Ignore SIGCHLD so we do not have to wait on the child processes
    //signal(SIGCHLD, SIG_IGN);

    // Read state, error and scan info to create the scan_wifi file
    GetState();
    GetLastError();

    // initiate the creation of the wifi_scan_results file
    QCC_DbgHLPrintf(("Initiating GetScanInfo when service starts running"));
    unsigned short age = 0;
    OBScanInfo* scanList = NULL;
    size_t scanListNumElements = 0;
    GetScanInfo(age, scanList, scanListNumElements);

    // if the m_concurrency values are out of range, set it to min
    if (m_concurrency < CONCURRENCY_MIN || m_concurrency > CONCURRENCY_MAX) {
        m_concurrency = CONCURRENCY_MIN;
    }

}

OnboardingControllerImpl::~OnboardingControllerImpl()
{
    QCC_DbgHLPrintf(("entered %s", __FUNCTION__));
#ifndef _WIN32
    if (m_scanTimerId) {
        timer_delete(m_scanTimerId);
        m_scanTimerId = 0;
    }

    ScanWifiTimerDone();
#endif
    // Scan results are stored in the scan array
    if (m_ScanArray) {
        delete [] m_ScanArray;
        m_ScanArray = NULL;
    }

    // Pointers to each element are stored in the map, so we need not iterate over values and delete
    m_ScanList.clear();
}

/*------------------------------------------------------------------------------
 * METHOD: ConfigureWiFi()
 * This method is called by the ConfigureWiFiHandler with the corresponding
 * input and output arguments. This method is empty, the developer should fill
 * it with the developer's implementation of the ConfigureWiFi method handler.
 *-----------------------------------------------------------------------------*/
void OnboardingControllerImpl::ConfigureWiFi(qcc::String SSID, qcc::String passphrase, short authType, short& status, qcc::String&  error, qcc::String& errorMessage) {
    QCC_DbgHLPrintf(("entered %s", __FUNCTION__));

    // Set the return value based on presence of fast switching feature
    status = m_concurrency;

    if (ANY == authType) {
        // The contents of the scan file are used only for authType ANY
        ParseScanInfo();
        std::map<qcc::String, OBScanInfo*>::iterator it = m_ScanList.find(SSID);
        if (it != m_ScanList.end() && it->second->authType != ANY) {
            // We have a scan record available with a known auth type.
            authType = it->second->authType;
        } else {
            // We have never heard of this SSID or couldnt figure out its auth type.
            // Gotta try them all :(
            if (passphrase.empty()) {
                // Try open encryption only
                authType = OPEN;
            } else {
                // Try WPA2, WPA, WEP if the passphrase has a chance of working
                // stop when we succeed
                for (authType = WPS; authType >= WPA2_AUTO; --authType) {
                    switch (authType) {
                    case WPA2_AUTO:
                    case WPA_AUTO:
                    case WPA_TKIP:
                    case WPA_CCMP:
                    case WPA2_TKIP:
                    case WPA2_CCMP:
                        {
                            if (!isValidWPAKey(passphrase)) {
                                continue;
                            }
                        }
                        break;

                    case WEP:
                        {
                            if (!isValidWEPKey(passphrase)) {
                                continue;
                            }
                        }
                        break;
                    }


                    if (!execute_configure(("'" + SSID + "'").c_str(), authType, ("'" + passphrase + "'").c_str())) {
                        return;
                    }
                }
                // Done trying applicable methods
                return;
            }
        }
    }

    execute_configure(("'" + SSID + "'").c_str(), authType, ("'" + passphrase + "'").c_str());
} /* ConfigureWiFi() */

void* OnboardingControllerImpl::OBS_Connect(void* obsArg)
{
    OnboardingControllerImpl* obController = (OnboardingControllerImpl*)obsArg;
    qcc::String connectCmd = obController->m_connectCmd;

    //stopping daemon before connect cmd and restarting it after
    execute_system("/etc/init.d/alljoyn stop");
    execute_system(connectCmd.c_str());
    execute_system("/etc/init.d/alljoyn start");

    return NULL;
}
/*------------------------------------------------------------------------------
 * METHOD: Connect()
 * This method is called by the ConnectHandler with the corresponding input and
 * output arguments. This method is empty, the developer should fill it with the
 * developer's implementation of the Connect method handler.
 *-----------------------------------------------------------------------------*/
void OnboardingControllerImpl::Connect() {
/* Fill in method handler implementation here. */
    QCC_DbgHLPrintf(("entered %s", __FUNCTION__));
    CancelAdvertise();
#ifdef _WIN32
    HANDLE m_handle;
    m_handle = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 256 * 1024, (unsigned int (__stdcall*)(void*)) OnboardingControllerImpl::OBS_Connect, this, 0, NULL));
    CloseHandle(m_handle);
#else
    pthread_t thread;
    pthread_create(&thread, NULL, OnboardingControllerImpl::OBS_Connect, this);
    pthread_detach(thread);
#endif
} /* Connect() */

OBAuthType TranslateToOBAuthType(int authNum, GroupCiphers theCiphers)
{
    switch (authNum) {

    case WPA_AUTO:
        switch (theCiphers) {
        case CIPHER_NONE:
        case CIPHER_BOTH:
            return WPA_AUTO;

        case CIPHER_TKIP:
            return WPA_TKIP;

        case CIPHER_CCMP:
            return WPA_CCMP;
        }
        break;

    case WPA2_AUTO:
        switch (theCiphers) {
        case CIPHER_NONE:
        case CIPHER_BOTH:
            return WPA2_AUTO;

        case CIPHER_TKIP:
            return WPA2_TKIP;

        case CIPHER_CCMP:
            return WPA2_CCMP;
        }
        break;

    default:
        return ANY;
    }
    return ANY;
}

long GetMinElapsedFromLastScan(const char* filename)
{
    long minutes = -1;
    struct stat attrib;
    int result = stat(filename, &attrib);
    if (result == 0) {
        time_t t = time(0);   // get time now
        double seconds = difftime(t, attrib.st_mtime);
        minutes = seconds / 60;
    }
    return minutes;
}

char* OnboardingControllerImpl::Trim(char* str)
{
    if (!str || strlen(str) == 0) {
        return str;
    }

    char*end;

    // Trim leading space
    while (isspace(*str)) str++;

    if (*str == 0) { // All spaces?
        return str;
    }

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) end--;

    // Write new null terminator
    *(end + 1) = 0;

    return str;
}

void OnboardingControllerImpl::ParseScanInfo()
{
    // Scan records are already sorted by signal strength
    std::ifstream scanFile;

    scanFile.open(m_scanFile.c_str());
    if (!scanFile.is_open()) {
        return;
    }

    //get lines in file which is the number of ssid's scanned
    //when done, return to the beginning of the file
    int length = std::count(std::istreambuf_iterator<char>(scanFile), std::istreambuf_iterator<char>(), '\n');
    scanFile.clear();
    scanFile.seekg(0, std::ios::beg);

    // Clear out the map and delete array and initialize with the new length
    m_ScanList.clear();

    if (m_ScanArray != NULL) {
        delete [] m_ScanArray;
        m_ScanArray = NULL;
    }
    m_ScanArray = new OBScanInfo[length];

    // start parsing the scan file
    std::string line;
    int current = 0;

    while (std::getline(scanFile, line)) {
        std::stringstream currLine(line);

        char* ssid = strtok((char*)line.c_str(), "\t");
        char* auth = strtok(NULL, "\t");

        if (!ssid) {
            continue;
        }

        char* authType = strtok(auth, "-");
        char* firstCipher = strtok(NULL, "-");
        char* secondCipher = strtok(NULL, "-");

        if (!authType) {
            continue;
        }

        // clear spaces
        ssid = Trim(ssid);
        authType = Trim(authType);
        firstCipher = Trim(firstCipher);
        secondCipher = Trim(secondCipher);

        m_ScanArray[current].SSID = ssid;

        // get auth type
        if (!strcmp(authType, "WEP")) {
            m_ScanArray[current].authType = WEP;
        } else if (!strcmp(authType, "Open")) {
            m_ScanArray[current].authType = OPEN;
        } else if (!strcmp(authType, "WPA2")) {
            m_ScanArray[current].authType = WPA2_AUTO;
        } else if (!strcmp(authType, "WPA")) {
            m_ScanArray[current].authType = WPA_AUTO;
        }

        // still need to set the ciphers for WPA and WPA2
        if ((m_ScanArray[current].authType == WPA_AUTO) || (m_ScanArray[current].authType == WPA2_AUTO)) {
            // example:
            // One cipher CCMP would look like "WPA2-CCMP--PSK"
            // Two ciphers CCMP and TKIP "WPA2-CCMP-TKIP-PSK" we dont know the order of CCMP and TKIP

            GroupCiphers GCiphers = CIPHER_NONE;
            if (firstCipher == NULL) {
                GCiphers = CIPHER_NONE;
            } else if (!strcmp(firstCipher, "PSK")) {
                GCiphers = CIPHER_NONE;
            } else if (!strcmp(firstCipher, "CCMP")) {
                GCiphers = CIPHER_CCMP;
                if (secondCipher != NULL && !strcmp(secondCipher, "TKIP")) {
                    GCiphers = CIPHER_BOTH;
                }
            } else if (!strcmp(firstCipher, "TKIP")) {
                GCiphers = CIPHER_TKIP;
                if (secondCipher != NULL && !strcmp(secondCipher, "CCMP")) {
                    GCiphers = CIPHER_BOTH;
                }
            }

            m_ScanArray[current].authType = TranslateToOBAuthType(m_ScanArray[current].authType, GCiphers);
        }

        // add parsed record to array and map if SSID not duplicated
        std::map<qcc::String, OBScanInfo*>::iterator it = m_ScanList.find(m_ScanArray[current].SSID);
        if (it == m_ScanList.end()) {
            m_ScanList.insert(std::pair<qcc::String, OBScanInfo*>(m_ScanArray[current].SSID, &m_ScanArray[current]));
            ++current;
        }
    }

    scanFile.close();
}

#ifndef _WIN32
void OnboardingControllerImpl::StartScanWifiTimer()
{
    QCC_DbgHLPrintf(("entered %s", __FUNCTION__));
    memset(&m_scanSignalEvent, 0, sizeof(m_scanSignalEvent));

    m_scanSignalEvent.sigev_notify            = SIGEV_THREAD;
    m_scanSignalEvent.sigev_value.sival_ptr   = (void*)this;
    m_scanSignalEvent.sigev_notify_function   = TimerDone;
    m_scanSignalEvent.sigev_notify_attributes = NULL;


    if (timer_create(CLOCK_REALTIME, &m_scanSignalEvent, &m_scanTimerId) != 0) {
        QCC_DbgTrace(("Error creating timer."));
        return;
    }

    m_scanTimerSpecs = { { 0, 0 }, { SCAN_WIFI_MAX_TIME_IN_SEC, 0 } };
    if (timer_settime(m_scanTimerId, 0, &m_scanTimerSpecs, NULL) == -1) {
        QCC_DbgTrace(("Could not start timer:"));
    }
}

void OnboardingControllerImpl::TimerDone(union sigval si)
{
    QCC_DbgHLPrintf(("entered %s", __FUNCTION__));

    OnboardingControllerImpl* thisClass = reinterpret_cast<OnboardingControllerImpl*>(si.sival_ptr);
    thisClass->ScanWifiTimerDone();
}

void OnboardingControllerImpl::ScanWifiTimerDone()
{
    QCC_DbgHLPrintf(("entered %s", __FUNCTION__));
    if (m_scanWifiThreadIsRunning) {
        pthread_cancel(m_scanWifiThread);
        m_scanWifiThreadIsRunning = false;
        QCC_DbgTrace(("ScanWifi timed out and is being canceled"));
    }
}
#endif

void OnboardingControllerImpl::StartScanWifi()
{
    QCC_DbgHLPrintf(("entered %s", __FUNCTION__));
    m_scanWifiThreadIsRunning = true;
    execute_system(m_scanCmd.c_str());
#ifndef _WIN32
    if (m_scanTimerId) {
        timer_delete(m_scanTimerId);
        m_scanTimerId = 0;
    }
#endif
    m_scanWifiThreadIsRunning = false;
}

/*
 * ScanWifiThread
 * A method called when m_scanWifiThread completes
 */
void* OnboardingControllerImpl::ScanWifiThread(void* context)
{
    OnboardingControllerImpl* thisClass = reinterpret_cast<OnboardingControllerImpl*>(context);
    thisClass->StartScanWifi();
    return NULL;
}

/*------------------------------------------------------------------------------
 * METHOD: GetScanInfo()
 * This method is called by the GetScanInfoHandler with the corresponding input
 * and output arguments. This method is empty, the developer should fill it with
 * the developer's implementation of the GetScanInfo method handler.
 *-----------------------------------------------------------------------------*/
void OnboardingControllerImpl::GetScanInfo(unsigned short& age, OBScanInfo*& scanList, size_t& scanListNumElements)
{
    QCC_DbgHLPrintf(("entered %s", __FUNCTION__));
    ParseScanInfo();

    scanListNumElements = m_ScanList.size();
    scanList = m_ScanArray;

    // maxes out at around 45 days
    long minold = GetMinElapsedFromLastScan(m_scanFile.c_str());
    if (minold < 0xFFFF) {
        age = (short)minold;
    } else {
        age = 0xFFFF;
    }

    // Spawn a thread to scan the wifi and update the wifi_scan_results
    if (!m_scanWifiThreadIsRunning) {
#ifdef _WIN32
        m_scanWifiThread = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 256 * 1024, (unsigned int (__stdcall*)(void*))ScanWifiThread, this, 0, NULL));
        CloseHandle(m_scanWifiThread);
#else
        StartScanWifiTimer();
        pthread_create(&m_scanWifiThread, NULL, ScanWifiThread, this);
        pthread_detach(m_scanWifiThread);
#endif
    }

} /* GetScanInfo() */

void* OnboardingControllerImpl::OBS_Offboard(void* obsArg)
{
    OnboardingControllerImpl* obController = (OnboardingControllerImpl*)obsArg;
    qcc::String offboardCmd = obController->m_offboardCmd;

    //stopping daemon before offboard cmd and restarting it after
    execute_system("/etc/init.d/alljoyn stop");
    execute_system(offboardCmd.c_str());
    execute_system("/etc/init.d/alljoyn start");

    return NULL;
}

/*------------------------------------------------------------------------------
 * METHOD: Offboard()
 * This method is called by the OffboardHandler with the corresponding input and
 * output arguments. This method is empty, the developer should fill it with the
 * developer's implementation of the Offboard method handler.
 *-----------------------------------------------------------------------------*/
void OnboardingControllerImpl::Offboard()
{
    QCC_DbgHLPrintf(("entered %s", __FUNCTION__));
    CancelAdvertise();
#ifdef _WIN32
    m_scanWifiThread = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 256 * 1024, (unsigned int (__stdcall*)(void*))ScanWifiThread, this, 0, NULL));
    CloseHandle(m_scanWifiThread);
#else
    pthread_t thread;
    pthread_create(&thread, NULL, OnboardingControllerImpl::OBS_Offboard, this);
    pthread_detach(thread);
#endif
} /* Offboard() */

short OnboardingControllerImpl::GetState()
{
    QCC_DbgHLPrintf(("entered %s", __FUNCTION__));
    std::ifstream stateFile(m_stateFile.c_str());
    if (stateFile.is_open()) {
        std::string line;
        getline(stateFile, line);
        std::istringstream iss(line);
        iss >> m_state;
        stateFile.close();
    }
    return m_state;
}

const OBLastError& OnboardingControllerImpl::GetLastError()
{
    QCC_DbgHLPrintf(("entered %s", __FUNCTION__));
    std::ifstream errorFile(m_errorFile.c_str());
    if (errorFile.is_open()) {
        std::string line;
        getline(errorFile, line);
        std::istringstream iss(line);
        iss >> m_oBLastError.validationState;
        getline(errorFile, line);
        QCC_DbgHLPrintf(("%s", line.c_str()));
        m_oBLastError.message.assign(line.c_str());
        errorFile.close();
    }

    return m_oBLastError;
}

int OnboardingControllerImpl::execute_configure(const char* SSID, const int authType, const char* passphrase) {
    QCC_DbgHLPrintf(("entered %s", __FUNCTION__));
#ifdef _OPEN_WRT_
    char cmd[CMD_SIZE] = { 0 };
    qcc::String authTypeString = qcc::String("'") + AuthText(authType) + qcc::String("'");
    snprintf(cmd, CMD_SIZE, m_configureCmd.c_str(), SSID, authTypeString.c_str(), passphrase);
    return execute_system(cmd);
#else
    return 0;
#endif
}

void OnboardingControllerImpl::CancelAdvertise()
{
    QCC_DbgHLPrintf(("entered %s", __FUNCTION__));
    m_BusAttachment->EnableConcurrentCallbacks();
    if (m_BusAttachment->IsConnected() && m_BusAttachment->GetUniqueName().size() > 0) {
        QStatus status = m_BusAttachment->CancelAdvertiseName(m_BusAttachment->GetUniqueName().c_str(), TRANSPORT_ANY);
        (void)status;
        QCC_DbgHLPrintf(("CancelAdvertiseName for %s = %s", m_BusAttachment->GetUniqueName().c_str(), QCC_StatusText(status)));
        (void)status;  // Suppress unused warning from G++ when building is release mode.
    }
}

static int execute_system(const char*cmd)
{
    QCC_DbgHLPrintf(("entered %s", __FUNCTION__));

#ifdef _OPEN_WRT_
    if (!cmd) {
        return -1;
    }
    QCC_DbgHLPrintf(("executing %s", cmd));
    int result = system(cmd);
    result = WEXITSTATUS(result);
    QCC_DbgHLPrintf(("system result=%d", result));
    if (-1 == result) {
        QCC_DbgHLPrintf(("Error executing system: %d", strerror(errno)));
    }
    return result;
#else
    return 0;
#endif
}

