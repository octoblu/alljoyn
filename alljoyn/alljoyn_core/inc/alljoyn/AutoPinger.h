/**
 * @file
 *
 * AutoPinger
 */

/******************************************************************************
 * Copyright (c) 2014, AllSeen Alliance. All rights reserved.
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

#ifndef _ALLJOYN_AUTOPINGER_H_
#define _ALLJOYN_AUTOPINGER_H_

#ifndef __cplusplus
#error Only include AutoPinger.h in C++ code.
#endif

#include <map>
#include <qcc/Timer.h>
#include <qcc/String.h>
#include <qcc/Mutex.h>
#include <qcc/Debug.h>
#include <alljoyn/Status.h>
#include <alljoyn/PingListener.h>

namespace ajn {
/// @cond ALLJOYN_DEV
/** @internal Forward references */
class BusAttachment;
struct PingGroup;
/// @endcond

/**
 * AutoPinger class
 */
class AutoPinger : public qcc::AlarmListener {
  public:


    /**
     * Create instance of autopinger
     *
     * @param busAttachment reference to the BusAttachment associated with this
     *                      autopinger.
     *
     */
    AutoPinger(BusAttachment& busAttachment);

    /**
     * Destructor
     */
    virtual ~AutoPinger();

    /**
     * Pause all ping actions
     */
    void Pause();

    /**
     * Resume ping actions
     */
    void Resume();

    /**
     * Define new ping group
     *
     * @param  group Ping group name
     * @param  listener Listener called when a change was detected in the reachability of a destination
     * @param  pingInterval Ping interval in seconds
     */
    void AddPingGroup(const qcc::String& group, PingListener& listener, uint32_t pingInterval = 5);

    /**
     * Remove complete ping group, including all destinations
     *
     * @param  group Ping group name
     */
    void RemovePingGroup(const qcc::String& group);

    /**
     * Set ping interval of the specified group
     *
     * @param  group Ping group name
     * @param  pingInterval Ping interval in seconds
     * @return
     *  - #ER_OK: Interval updated
     *  - #ER_BUS_PING_GROUP_NOT_FOUND: group did not exist
     */
    QStatus SetPingInterval(const qcc::String& group, uint32_t pingInterval);

    /**
     * Add a destination to the specified ping group
     * Destinations are refcounted and must be removed N times if they were added N times
     *
     * @param  group Ping group name
     * @param  destination Destination name to be pinged
     * @return
     *  - #ER_OK: destination added
     *  - #ER_BUS_PING_GROUP_NOT_FOUND: group did not exist
     */
    QStatus AddDestination(const qcc::String& group, const qcc::String& destination);

    /**
     * Remove a destination from the specified ping group
     * This will lower the refcount by one and only remove the destination when the refcount reaches zero
     *
     * @param  group Ping group name
     * @param  destination Destination name to be removed
     * @param  removeAll Rather than decrementing the refcount by one, set refcount to zero and remove
     * @return
     *  - #ER_OK: destination removed or was not present
     *  - #ER_BUS_PING_GROUP_NOT_FOUND: group did not exist
     */
    QStatus RemoveDestination(const qcc::String& group, const qcc::String& destination, bool removeAll = false);

  private:
    friend class AutoPingAsyncCB;
    friend struct Destination;
    friend class PingAsyncContext;

    enum PingState {
        UNKNOWN,
        LOST,
        AVAILABLE
    };

    AutoPinger(const AutoPinger&);
    void operator=(const AutoPinger&);

    bool UpdatePingStateOfDestination(const qcc::String& group, const qcc::String& destination, const AutoPinger::PingState state);
    void PingGroupDestinations(const qcc::String& group);
    bool IsRunning();
    void AlarmTriggered(const qcc::Alarm& alarm, QStatus reason);

    qcc::Timer timer; /* Single Timerthread */
    BusAttachment& busAttachment;
    qcc::Mutex pingerMutex;
    std::map<qcc::String, PingGroup*> pingGroups;

    bool pausing;
};
static class AutoPingerInit {
  public:
    AutoPingerInit();
    ~AutoPingerInit();
    static void Cleanup();
  private:
    static bool cleanedup;
} autoPingerInit;
}
#undef QCC_MODULE
#endif /* AUTOPINGER_H_ */
