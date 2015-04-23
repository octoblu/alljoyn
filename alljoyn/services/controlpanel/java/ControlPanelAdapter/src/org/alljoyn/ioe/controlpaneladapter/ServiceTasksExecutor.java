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

package org.alljoyn.ioe.controlpaneladapter;

import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.RejectedExecutionException;

import android.util.Log;

/**
 * Executes {@link Callable} tasks.
 * The class is initialized with a given number of threads or with {@link ServiceTasksExecutor#NUM_THREADS_DEFAULT}
 */
public class ServiceTasksExecutor {
    private final static String TAG = "cpapp" + ServiceTasksExecutor.class.getSimpleName();

    /**
     * Default number of threads
     */
    private static final int NUM_THREADS_DEFAULT = 2;

    /**
     * Executor service
     */
    private final ExecutorService executor;

    /**
     * Create the {@link ServiceTasksExecutor} with {@link ServiceTasksExecutor#NUM_THREADS_DEFAULT}
     * @return {@link ServiceTasksExecutor}
     */
    public static ServiceTasksExecutor createExecutor(){

        return createExecutor(NUM_THREADS_DEFAULT);
    }

    /**
     * Create the {@link ServiceTasksExecutor} with the given number of threads.
     * @param numThreads Create the executor with the given number of threads
     * @return {@link ServiceTasksExecutor}
     */
    public static ServiceTasksExecutor createExecutor(int numThreads){

        return new ServiceTasksExecutor(numThreads);
    }

    /**
     * Constructor
     */
    private ServiceTasksExecutor(int numThreads) {

        Log.d(TAG, "Creating ServiceTaskExecutor with numThreads: '" + numThreads + "'");
        executor = Executors.newFixedThreadPool(numThreads);
    }

    /**
     * Do the best effort to shutdown the executor.
     * @see ExecutorService#shutdownNow()
     */
    public void shutdown() {

        Log.d(TAG, "Shutting down the ServiceTaskExecutor");
        executor.shutdownNow();
    }

    /**
     * Submit the given task for execution
     * @param task The {@link Callable} task to execute
     * @return {@link Future} of the submitted object
     * @throws IllegalArgumentException if the given task in NULL
     * @throws RejectedExecutionException
     * @see ExecutorService#submit(Callable)
     */
    public Future<Object> submit(Callable<Object> task) {

        if ( task == null ) {

            throw new IllegalArgumentException("task is indefined");
        }

        return executor.submit(task);
    }
}

