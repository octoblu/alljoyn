/******************************************************************************
 * Copyright (c) 2013-2014, AllSeen Alliance. All rights reserved.
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


#ifndef NOTIFICATION_ASYNCTASK_EVENTS_H_
#define NOTIFICATION_ASYNCTASK_EVENTS_H_

#include <pthread.h>
#include <queue>
#include <qcc/String.h>
#include <iostream>
#include <alljoyn/notification/NotificationService.h>
#include <alljoyn/services_common/AsyncTaskQueue.h>

namespace ajn {
namespace services {

/**
 * Notification async task events.
 * Class implementing callbacks fto handle messages.
 */
class NotificationAsyncTaskEvents : public AsyncTask {
  public:
    /**
     * constructor of NotificationAsyncTaskEvents
     */
    NotificationAsyncTaskEvents();
    /**
     * destructor of NotificationAsyncTaskEvents
     */
    virtual ~NotificationAsyncTaskEvents();
    /**
     * callback to handle the case of empty message queue.
     */
    virtual void OnEmptyQueue();
    /**
     * callback to handle the case of new message
     * @param taskdata - object to handle
     */
    virtual void OnTask(TaskData const* taskdata);

  private:
    /**
     * send dismiss signal
     * @param asyncTaskQueue - a template type of message
     */
    void sendDismissSignal(TaskData const* taskdata);

};

} //namespace services
} //namespace ajn



#endif /* NOTIFICATION_ASYNCTASK_EVENTS_H_ */
