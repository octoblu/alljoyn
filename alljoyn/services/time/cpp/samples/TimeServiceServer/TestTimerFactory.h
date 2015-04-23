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

#ifndef TESTTIMERFACTORY_H_
#define TESTTIMERFACTORY_H_

#include <vector>
#include <alljoyn/time/TimeServiceServerTimerFactory.h>
#include <alljoyn/time/TimeServiceServerTimer.h>
#include "TestTimer.h"

namespace ajn {
namespace services {

class TestTimerFactory : public TimeServiceServerTimerFactory {

  public:

    /**
     * Constructor
     */
    TestTimerFactory();

    /**
     * Destructor
     */
    ~TestTimerFactory();

    /**
     * Create new TimeServiceServerTimer
     *
     * @param ptrTimer Out variable. Pointer to the TimeServiceServerTimer
     * @param errorInfo Out variable. If create new Timer fails due to max permitted number of Timer objects
     * reached, or another reason. Populate ErrorInfo::errorName with tsConsts::OUT_OF_RANGE and
     * the errorInfo::errorDesc with the error description.
     *
     * @return ER_OK if creation of the TimeServiceServerTimer succeeded, or return a failure status and
     * populate the ErrorInfo
     */
    QStatus newTimer(TimeServiceServerTimer** ptrTimer, tsConsts::ErrorInfo* errorInfo);

    /**
     * Delete TimeServiceServerTimer identified by the given objectPath.
     * To delete TimeServiceServerTimer, call its release() method.
     *
     * @param objectPath of the TimeServiceServerTimer to be deleted.
     * @param errorInfo To be populated if for any reason failed to delete the TimeServiceServerTimer
     *
     * @return ER_OK if succeeded to delete TimeServiceServerTimer or a failure status with population
     * of the ErrorInfo
     */
    QStatus deleteTimer(const qcc::String& objectPath, tsConsts::ErrorInfo* errorInfo);

    /**
     * The method is invoked by the TimeService, when there was a failure to create NewTimer,
     * or to construct a response to the client that has invoked the NewTimer method.
     * Application that implementing this method is expected to invoke TimeServiceServerTimer.release()
     * to release object resources.
     *
     * @param Timer TimeServiceServerTimer that failed to be created
     */
    void handleError(TimeServiceServerTimer* Timer);

    /**
     * Find Timer by the given object path
     *
     * @param ObjectPath of the Timer to search
     * @param ptrTimer Out variable. Pointer to the found Timer
     *
     * @returns iterator where the Timer was found
     */
    std::vector<TestTimer*>::iterator findTimer(const qcc::String& objectPath, TestTimer** ptrTimer);

    /**
     * Print the current object paths associated with this factory
     */
    void printTimerObjectPaths();

  private:

    /**
     * Timers created by this TimerFactory
     */
    std::vector<TestTimer*> m_Timers;

    /**
     * Releases object resources
     */
    void testRelease();
};

} /* namespace services */
} /* namespace ajn */

#endif /* TESTTIMERFACTORY_H_ */
