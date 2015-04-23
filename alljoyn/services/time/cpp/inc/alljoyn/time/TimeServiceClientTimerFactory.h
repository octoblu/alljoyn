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

#ifndef TIMESERVICECLIENTTIMERFACTORY_H_
#define TIMESERVICECLIENTTIMERFACTORY_H_

 #include <alljoyn/Status.h>
 #include <alljoyn/time/TimeServiceObjectIntrospector.h>

namespace ajn {
namespace services {

/**
 * Time Service Timer Factory.
 * Use this class to communication with the server side Timer Factory object.
 */
class TimeServiceClientTimerFactory : public TimeServiceObjectIntrospector {

  public:

    /**
     * Constructor
     *
     * @param tsClient TimeServiceClient managing this TimeServiceClientTimerFactory object
     * @param objectPath Object path of the server side TimerFactory object
     */
    TimeServiceClientTimerFactory(const TimeServiceClient& tsClient,
                                  const qcc::String& objectPath);

    /**
     * Destructor
     */
    ~TimeServiceClientTimerFactory();

    /**
     * Release object resources
     */
    void release();

    /**
     * Creates new Timer
     *
     * @param objectPath Out variable. Object path of the new created Timer.
     * Use this object path to create TimeServiceClientTimer.
     *
     * @return status ER_OK, or a failure status if failed to create new Timer
     */
    QStatus newTimer(qcc::String* objectPath);

    /**
     * Delete Timer identified by the given object path.
     *
     * @param objectPath Object path of the Timer object to be deleted.
     * Retrieve the object path by the call to TimeServiceClientTimer.getObjectPath()
     *
     * @return status ER_OK, or a failure status if failed to delete the Timer
     */
    QStatus deleteTimer(const qcc::String& objectPath);

    /**
     * Retrieves a list of timer object paths created by the TimerFactory.
     * Use this object path to create TimeServiceClientTimer.
     *
     * @param objectPathList Pointer to the vector to be populated with the object paths of the
     * Timers created by this TimerFactory
     *
     * @return status ER_OK, or a failure status if failed to retrieve the list of Timers
     */
    QStatus retrieveTimerList(std::vector<qcc::String>* objectPathList);

  private:

    /**
     * Interface Description of the TimerFactory interface
     */
    InterfaceDescription* m_TimerFactoryIface;


};

} /* namespace services */
} /* namespace ajn */

#endif /* TIMESERVICECLIENTTIMERFACTORY_H_ */
