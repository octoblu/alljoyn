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

#ifndef TIMESERVICESERVERALARMFACTORY_H_
#define TIMESERVICESERVERALARMFACTORY_H_

#include <alljoyn/time/TimeServiceServerAlarm.h>
#include <alljoyn/time/TimeServiceConstants.h>

namespace ajn {
namespace services {

class TimeServiceServer;
class TimeServiceAlarmFactoryBusObj;

/**
 * Extend this class to receive events related to this AlarmFactory object.
 */
class TimeServiceServerAlarmFactory {

    friend class TimeServiceServer;

  public:

    /**
     * Constructor
     */
    TimeServiceServerAlarmFactory();

    /**
     * Destructor
     */
    virtual ~TimeServiceServerAlarmFactory();

    /**
     * Releases object resources
     */
    void release();

    /**
     * Returns object path of this AlarmFactory
     *
     * @return Returns object path of this AlarmFactory
     */
    qcc::String const& getObjectPath() const;

    /**
     * Create new TimeServiceServerAlarm
     *
     * @param ptrAlarm Out variable. Pointer to the TimeServiceServerAlarm
     * @param errorInfo Out variable. If create new alarm fails due to max permitted number of Alarm objects
     * reached, or another reason. Populate ErrorInfo::errorName with tsConsts::OUT_OF_RANGE and
     * the errorInfo::errorDesc with the error description.
     *
     * @return ER_OK if creation of the TimeServiceServerAlarm succeeded, or return a failure status and
     * populate the ErrorInfo
     */
    virtual QStatus newAlarm(TimeServiceServerAlarm** ptrAlarm, tsConsts::ErrorInfo* errorInfo) = 0;

    /**
     * Delete TimeServiceServerAlarm identified by the given objectPath.
     * To delete TimeServiceServerAlarm, call its release() method.
     *
     * @param objectPath of the TimeServiceServerAlarm to be deleted.
     * @param errorInfo To be populated if for any reason failed to delete the TimeServiceServerAlarm
     *
     * @return ER_OK if succeeded to delete TimeServiceServerAlarm or a failure status with population
     * of the ErrorInfo
     */
    virtual QStatus deleteAlarm(const qcc::String& objectPath, tsConsts::ErrorInfo* errorInfo) = 0;

    /**
     * The method is invoked by the TimeService, when there was a failure to create NewAlarm,
     * or to construct a response to the client that has invoked the NewAlarm method.
     * Application that implementing this method is expected to invoke TimeServiceServerAlarm.release()
     * to release object resources.
     *
     * @param alarm TimeServiceServerAlarm that failed to be created
     */
    virtual void handleError(TimeServiceServerAlarm* alarm) = 0;

  private:

    /**
     * Private Copy constructor of TimeServiceServerAlarmFactory.
     * TimeServiceServerAlarmFactory is not copy-able
     *
     * @param alarmFactory TimeServiceServerAlarmFactory to copy
     */
    TimeServiceServerAlarmFactory(const TimeServiceServerAlarmFactory& alarmFactory);

    /**
     * Private assignment operator of TimeServiceServerAlarmFactory.
     * TimeServiceServerAlarmFactory is not assignable
     *
     * @param alarmFactory
     *
     * @return TimeServiceServerAlarmFactory
     */
    TimeServiceServerAlarmFactory& operator=(const TimeServiceServerAlarmFactory& alarmFactory);

    /**
     * TimeServiceAlarmFactory implementing AllJoyn functionality
     */
    TimeServiceAlarmFactoryBusObj* m_AlarmFactoryBusObj;

    /**
     * Associate this AlarmFactory with the AlarmFactoryBusObj which implements AllJoyn functionality
     *
     * @param alarmFactoryBusObj
     * @return status if Succeeded to set the alarmFactoryBusObj
     */
    QStatus setAlarmFactoryBusObj(TimeServiceAlarmFactoryBusObj* alarmFactoryBusObj);
};

} /* namespace services */
} /* namespace ajn */

#endif /* TIMESERVICESERVERALARMFACTORY_H_ */
