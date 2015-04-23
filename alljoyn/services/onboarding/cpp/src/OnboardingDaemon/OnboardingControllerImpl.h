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

#ifndef _ONBOARDINGCONTROLLERIMPL_H
#define _ONBOARDINGCONTROLLERIMPL_H

#include <alljoyn/onboarding/OnboardingControllerAPI.h>
#include <signal.h>
#ifdef _WIN32
#include <Windows.h>
#define pthread_mutex_t CRITICAL_SECTION
#define pthread_cond_t CONDITION_VARIABLE
#else
#include <pthread.h>
#endif
/**
 *  OnboardingControllerAPI  interface class that is implemented  by the Application and controls the WIFI of the system.
 */

class OnboardingControllerImpl : public ajn::services::OnboardingControllerAPI {

  public:

    /**
     * Constructor of OnboardingControllerImpl
     */
    OnboardingControllerImpl(qcc::String scanFile,
                             qcc::String stateFile,
                             qcc::String errorFile,
                             qcc::String configureCmd,
                             qcc::String connectCmd,
                             qcc::String offboardCmd,
                             qcc::String scanCmd,
                             ajn::services::OBConcurrency concurency,
                             ajn::BusAttachment& busAttachment);
    /**
     * Destructor of OnboardingControllerAPI
     */
    virtual ~OnboardingControllerImpl();

    /**
     * ConfigureWiFi passing connection info to connect to WIFI
     * @param[in] SSID  of WIFI AP
     * @param[in] passphrase of WIFI AP in hex
     * @param[in] authType used by  WIFI AP
     * @param[out] status
     * @param[out] error
     * @param[out] errorMessage
     */
    virtual void ConfigureWiFi(qcc::String SSID, qcc::String passphrase, short authType, short& status, qcc::String& error, qcc::String& errorMessage);

    /**
     *	Connect to the WIFI using the ConfigureWiFi details supplied before
     */
    virtual void Connect();

    /**
     * GetScanInfo return a list of  OBScanInfo representing  WIFI scan info
     * @param[out] age time in minutes from the last scan
     * @param[out] scanInfoList list of OBScanInfo*
     * @param[out] scanListNumElements number of elements
     */
    virtual void GetScanInfo(unsigned short& age, ajn::services::OBScanInfo*& scanInfoList, size_t& scanListNumElements);

    /**
     *	Offboard disconnect from the WIFI
     */
    virtual void Offboard();

    /**
     * Getstate return the last state.
     */
    virtual short GetState();

    /**
     * GetLastError returns the last error
     * @return OBLastError
     */
    virtual const ajn::services::OBLastError& GetLastError();

    /*
     * Method that calls the system scanCmd
     */
    void StartScanWifi();

    /**
     * static function to connect device to ap
     * @param obsArg - instance of onboarding controller passed in as arg
     */
    static void* OBS_Connect(void* obsArg);

    /**
     * static function to offboard device
     * @param obsArg - instance of onboarding controller passed in as arg
     */
    static void* OBS_Offboard(void* obsArg);

  private:
    /**
     * operator=
     */
    virtual OnboardingControllerImpl operator=(const OnboardingControllerImpl& otherOnboardingControllerAPI) { return *this; }
    /**
     * copy constructor
     */
    OnboardingControllerImpl(const OnboardingControllerImpl& otherOnboardingControllerImpl) { }

    /*
     * Parse scan info
     */
    virtual void ParseScanInfo();

    /**
     * Cancel Advertisement before system command
     */
    void CancelAdvertise();

    /**
     * ReAdvertise and Announce yourself
     */
    void AdvertiseAndAnnounce();

    /**
     * execute configure cmd
     */
    int execute_configure(const char*SSID, const int authText, const char*passphrase);

    /**
     * trim leading and trailing spaces
     */
    char* Trim(char* str);

    /**
     * Holds the last state
     */
    short m_state;

    /**
     *  Holds the last OBLastError
     */
    ajn::services::OBLastError m_oBLastError;


    /**
     * Map of SSIDs to the ScanInfo with best quality for that SSID
     */
    std::map<qcc::String, ajn::services::OBScanInfo*> m_ScanList;

    /**
     * Array of scan results
     */
    ajn::services::OBScanInfo* m_ScanArray;

    /**
     * BusAttachment to use for cancelAdvertise etc.
     */
    ajn::BusAttachment* m_BusAttachment;

    /*
     * commands to execute onboarding functions
     */
    qcc::String m_scanFile;
    qcc::String m_stateFile;
    qcc::String m_errorFile;
    qcc::String m_configureCmd;
    qcc::String m_connectCmd;
    qcc::String m_offboardCmd;
    qcc::String m_scanCmd;

    /*
     * Concurrency state
     */
    ajn::services::OBConcurrency m_concurrency;

    /**
     * Thread that scans the wifi and creates the wifi_scan_results file
     */
#ifdef _WIN32
    HANDLE m_scanWifiThread;
#else
    pthread_t m_scanWifiThread;
#endif

    /**
     * flag stating if scanWifiThreadIsRunning
     */
    volatile bool m_scanWifiThreadIsRunning;

    /**
     * Method that runs scan wifi in a thread
     */
    static void* ScanWifiThread(void* context);

#ifndef _WIN32
    /**
     * Method that starts a timer to stop scan wifi if it takes too long
     */
    void StartScanWifiTimer();

    /*
     * Callback Method when timer is done
     */
    void ScanWifiTimerDone();

    /*
     * Method called when scan wifi timer is done
     */
    static void TimerDone(union sigval si);

    /**
     *  the WifiScanTimer variables
     */
    timer_t m_scanTimerId;
    struct sigevent m_scanSignalEvent;
    struct itimerspec m_scanTimerSpecs;
#endif
};

#endif
