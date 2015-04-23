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

#ifndef TIMESERVICECLIENTALARMFACTORY_H_
#define TIMESERVICECLIENTALARMFACTORY_H_

#include <alljoyn/Status.h>
#include <alljoyn/time/TimeServiceObjectIntrospector.h>

namespace ajn {
namespace services {

/**
 * Time Service Alarm Factory.
 * Use this class to communication with the server side Alarm Factory object.
 */
class TimeServiceClientAlarmFactory : public TimeServiceObjectIntrospector {

  public:

    /**
     * Constructor
     *
     * @param tsClient TimeServiceClient managing this TimeServiceClientAlarmFactory object
     * @param objectPath Object path of the server side AlarmFactory object
     */
    TimeServiceClientAlarmFactory(const TimeServiceClient& tsClient, const qcc::String& objectPath);

    /**
     * Destructor
     */
    ~TimeServiceClientAlarmFactory();

    /**
     * Release object resources
     */
    void release();

    /**
     * Creates new Alarm
     *
     * @param objectPath Out variable. Object path of the new created Alarm.
     * Use this objectPath Path to create TimeServiceClientAlarm.
     *
     * @return status ER_OK, or a failure status if failed to create new Alarm
     */
    QStatus newAlarm(qcc::String* objectPath);

    /**
     * Delete Alarm identified by the given object path.
     *
     * @param objectPath Object path of the Alarm object to be deleted.
     * Retrieve the object path by the call to TimeServiceClientAlarm.getObjectPath()
     *
     * @return status ER_OK, or a failure status if failed to delete the Alarm
     */
    QStatus deleteAlarm(const qcc::String& objectPath);

    /**
     * Retrieves a list of alarm object paths created by the AlarmFactory.
     * Use this object path to create TimeServiceClientAlarm.
     *
     * @param objectPathList Pointer to the vector to be populated with the object paths of the
     * Alarms created by this AlarmFactory
     *
     * @return status ER_OK, or a failure status if failed to retrieve list of Alarms
     */
    QStatus retrieveAlarmList(std::vector<qcc::String>* objectPathList);

  private:

    /**
     * Interface Description of the AlarmFactory interface
     */
    InterfaceDescription* m_AlarmFactoryIface;
};

} /* namespace services */
} /* namespace ajn */

#endif /* TIMESERVICECLIENTALARMFACTORY_H_ */
