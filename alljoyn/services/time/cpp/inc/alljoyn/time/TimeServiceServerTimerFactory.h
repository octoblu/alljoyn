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

#ifndef TIMESERVICESERVERTIMERFACTORY_H_
#define TIMESERVICESERVERTIMERFACTORY_H_

#include <alljoyn/time/TimeServiceServerTimer.h>
#include <alljoyn/time/TimeServiceConstants.h>

namespace ajn {
namespace services {

class TimeServiceServer;
class TimeServiceTimerFactoryBusObj;

/**
 * Extend this class to receive events related to this TimerFactory object.
 */
class TimeServiceServerTimerFactory {

    friend class TimeServiceServer;

  public:

    /**
     * Constructor
     */
    TimeServiceServerTimerFactory();

    /**
     * Destructor
     */
    virtual ~TimeServiceServerTimerFactory();

    /**
     * Releases object resources
     */
    void release();

    /**
     * Returns object path of this TimerFactory
     *
     * @return Returns object path of this TimerFactory
     */
    qcc::String const& getObjectPath() const;

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
    virtual QStatus newTimer(TimeServiceServerTimer** ptrTimer, tsConsts::ErrorInfo* errorInfo) = 0;

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
    virtual QStatus deleteTimer(const qcc::String& objectPath, tsConsts::ErrorInfo* errorInfo) = 0;

    /**
     * The method is invoked by the TimeService, when there was a failure to create NewTimer,
     * or to construct a response to the client that has invoked the NewTimer method.
     * Application that implementing this method is expected to invoke TimeServiceServerTimer.release()
     * to release object resources.
     *
     * @param Timer TimeServiceServerTimer that failed to be created
     */
    virtual void handleError(TimeServiceServerTimer* Timer) = 0;

  private:

    /**
     * Private Copy constructor of TimeServiceServerTimerFactory.
     * TimeServiceServerTimerFactory is not copy-able
     *
     * @param serverTimerFactory TimeServiceServerTimerFactory to copy
     */
    TimeServiceServerTimerFactory(const TimeServiceServerTimerFactory& serverTimerFactory);

    /**
     * Private assignment operator of TimeServiceServerTimerFactory.
     * TimeServiceServerTimerFactory is not assignable
     *
     * @param serverTimerFactory
     *
     * @return TimeServiceServerTimerFactory
     */
    TimeServiceServerTimerFactory& operator=(const TimeServiceServerTimerFactory& serverTimerFactory);

    /**
     * TimeServiceTimerFactory implementing AllJoyn functionality
     */
    TimeServiceTimerFactoryBusObj* m_TimerFactoryBusObj;

    /**
     * Associate this TimerFactory with the TimerFactoryBusObj which implements AllJoyn functionality
     *
     * @param TimerFactoryBusObj
     * @return status if Succeeded to set the TimerFactoryBusObj
     */
    QStatus setTimerFactoryBusObj(TimeServiceTimerFactoryBusObj* TimerFactoryBusObj);
};

} /* namespace services */
} /* namespace ajn */

#endif /* TIMESERVICESERVERTIMERFACTORY_H_ */
