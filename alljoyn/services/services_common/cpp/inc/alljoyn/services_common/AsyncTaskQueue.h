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


#ifndef ASYNC_TASK_QUEUE_H_
#define ASYNC_TASK_QUEUE_H_

#include <pthread.h>
#include <queue>

namespace ajn {
namespace services {

/**
 * class TaskData
 * Base class to represent message
 */

class TaskData {
  public:
    virtual ~TaskData();
};

/**
 * class AsyncTask
 * User must derived from this class and implement its virtual functions
 */

class AsyncTask {
  public:
    AsyncTask() { }
    virtual ~AsyncTask() { };
    /**
     * OnEmptyQueue - handling 'queue become empty' events.
     */
    virtual void OnEmptyQueue() = 0;
    /**
     * OnEmptyQueue - handling 'queue got new message' events.
     *  @param taskdata - pointer to the data that currently processed.
     */
    virtual void OnTask(TaskData const* taskdata) = 0;
};

/**
 * class FeedbackAsyncTask provides option to process asynchronous tasks
 */
class AsyncTaskQueue {
  public:
    /**
     * AsyncTaskQueue constructor
     *  @param asyncTask - pointer to the class which callbacks will be called.
     *  @param ownersheap - if true, the queue will delete the data after calling to callbacks.
     */
    AsyncTaskQueue(AsyncTask* asyncTask, bool ownersheap = true);
    /**
     * AsyncTaskQueue destructor
     */
    ~AsyncTaskQueue();
    /**
     * Start
     */
    void Start();
    /**
     * Stop
     */
    void Stop();
    /**
     * Enqueue data
     */
    void Enqueue(TaskData const* taskdata);
  private:
    /**
     * The thread responsible for receiving messages
     */
    pthread_t m_Thread;

    /**
     * A Queue that holds the messages
     */
    std::queue<TaskData const*> m_MessageQueue;

    /**
     * The mutex Lock
     */
    pthread_mutex_t m_Lock;

    /**
     * The Queue Changed thread condition
     */
    pthread_cond_t m_QueueChanged;

    /**
     * is the thread in the process of shutting down
     */
    bool m_IsStopping;

    /**
     * A wrapper for the receiver Thread
     * @param context
     */
    static void* ReceiverThreadWrapper(void* context);

    /**
     * The function run in the Receiver
     */
    void Receiver();

    /**
     * class to report about events to the client
     */
    AsyncTask* m_AsyncTask;

    /**
     * is the thread is the owner of the objects in the queue and those will delete them.
     */
    bool m_ownersheap;
};

} //namespace services
} //namespace ajn




#endif /* ASYNC_TASK_QUEUE_H_ */
