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

#ifndef HANDLEONBOARDINGSIGNALS_H_
#define HANDLEONBOARDINGSIGNALS_H_


#include <alljoyn/BusAttachment.h>
#include <alljoyn/onboarding/OnboardingClientListener.h>
#include <alljoyn/services_common/AsyncTaskQueue.h>


/**
 * HandleOnboardingSignals
 *
 * Class for Onboarding Client to handle incoming signals
 */
namespace ajn {
namespace services {

/**
 * SignalMsg
 * A wrapper around the ajn:Messgae received in the signal.
 * This wrapper is needed to be able to use the AsyncTask Queue
 */
class SignalMesssage : public TaskData {
  public:
    /**
     * NotificationMsg constructor
     */
    SignalMesssage(const ajn::Message& message) :
        m_signalMessage(message)
    {
    }

    /**
     * NotificationMsg destructor
     */
    ~SignalMesssage()
    {
    }

    ajn::Message m_signalMessage;

};


class HandleOnboardingSignals : public ajn::MessageReceiver, public AsyncTask {

  public:

    /**
     * Constructor
     */
    HandleOnboardingSignals(BusAttachment* bus, OnboardingClientListener* listener);

    /**
     * Destructor
     */
    ~HandleOnboardingSignals();

    /**
     * Register to handle a signal
     */
    QStatus RegisterConnectionResultSignalHandler(const ajn::InterfaceDescription::Member* connectionResultSignalMethod);

    /**
     * Unregister to handle a signal
     */
    QStatus UnregisterConnectionResultSignalHandler(const ajn::InterfaceDescription::Member* connectionResultSignalMethod);


    ///////// MessageReceiver method //////
    /**
     * ConnectionResultSignal is a callback registered to receive AllJoyn Signal.
     * @param[in] member
     * @param[in] srcPath
     * @param[in] message
     */
    void ConnectionResultSignal(const ajn::InterfaceDescription::Member* member, const char* srcPath, ajn::Message& message);

    ////// AsyncTask methods /////////////

    /**
     * callback to handle the case of empty message queue.
     */
    void OnEmptyQueue();

    /**
     * callback to handle the case of new message
     * @param taskdata - object to handle
     */
    void OnTask(TaskData const* taskdata);


  private:

    /**
     * a pointer to the users callback listener
     */
    BusAttachment* m_bus;
    OnboardingClientListener* m_userListener;
    AsyncTaskQueue* m_asyncTaskQueue;

    /**
     * Copy constructor of HandleOnboardingSignals - private. HandleOnboardingSignals is not copy-able
     * @param handler - HandleOnboardingSignals to copy
     */
    HandleOnboardingSignals(const HandleOnboardingSignals& handler);

    /**
     * Assignment operator of HandleOnboardingSignals - private. HandleOnboardingSignals is not assignable
     * @param handler
     * @return HandleOnboardingSignals
     */
    HandleOnboardingSignals& operator=(const HandleOnboardingSignals& handler);
};



}
}
#endif /* HANDLEONBOARDINGSIGNALS_H_ */
