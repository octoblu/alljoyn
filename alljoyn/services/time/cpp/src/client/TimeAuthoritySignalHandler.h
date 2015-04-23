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


#ifndef __TIMECLIENT__TIMESERVICESIGNALHANDLER__
#define __TIMECLIENT__TIMESERVICESIGNALHANDLER__

#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/MessageReceiver.h>
#include <alljoyn/services_common/AsyncTaskQueue.h>
#include <alljoyn/time/TimeServiceClientClock.h>
#include "../common/Mutex.h"

namespace ajn {
namespace services {

class TimeAuthoritySignalTask;

class TimeAuthoritySignalHandler : public MessageReceiver {

  public:

    /**
     * GetInstance
     * @return TimeServiceSignalHandler created only once.
     */
    static TimeAuthoritySignalHandler* getInstance();

    /**
     * Adds TimeServiceClientClock event listener
     * @param clock
     * @return status
     */
    QStatus registerClock(TimeServiceClientClock*clock);

    /**
     * Remove TimeServiceClientClock event listener
     * @param clock
     */
    void unRegisterClock(TimeServiceClientClock*clock);

  private:

    /**
     * Constructor
     */
    TimeAuthoritySignalHandler();

    /**
     * Desctructor
     */
    virtual ~TimeAuthoritySignalHandler();

    /**
     *  Pointer to the TimeAuthoritySignalHandler
     */
    static TimeAuthoritySignalHandler* s_Instance;

    std::vector<TimeServiceClientClock*> m_Clocks;

    Mutex m_Protector;

    const ajn::InterfaceDescription::Member* m_ClockSignalMethod;

    static AsyncTaskQueue s_SignalQueue;

    static TimeAuthoritySignalTask s_SignalTask;

    void handleClockSignal(const ajn::InterfaceDescription::Member* member,
                           const char* srcPath, ajn::Message& msg);

    TimeServiceClientClock* findClockByObjectPathForDevice(qcc::String objPath, qcc::String sender);
};

} /* namespace services */
} /* namespace ajn */

#endif /* defined(__TIMECLIENT__TIMESERVICESIGNALHANDLER__) */
