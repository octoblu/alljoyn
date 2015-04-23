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

#include <alljoyn/time/TimeServiceServerTimer.h>
#include <alljoyn/time/TimeServiceTimerBusObj.h>

using namespace ajn;
using namespace services;

//Constructor
TimeServiceServerTimer::TimeServiceServerTimer() : m_TimerBusObj(NULL)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
}

//Destructor
TimeServiceServerTimer::~TimeServiceServerTimer()
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    release();
}

//Releases object resources
void TimeServiceServerTimer::release()
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!m_TimerBusObj) {

        return;
    }

    QCC_DbgHLPrintf(("Releasing the Timer '%s'", m_TimerBusObj->getObjectPath().c_str()));

    delete m_TimerBusObj;
    m_TimerBusObj = NULL;
}

//Returns object path of this Timer
qcc::String const& TimeServiceServerTimer::getObjectPath() const
{

    if (!m_TimerBusObj) {

        QCC_LogError(ER_FAIL, ("This Timer hasn't been created yet"));
        return qcc::String::Empty;
    }

    return m_TimerBusObj->getObjectPath();
}

//Set Timer Bus Object
QStatus TimeServiceServerTimer::setTimerBusObj(TimeServiceTimerBusObj* TimerBusObj)
{

    if (!TimerBusObj) {

        QCC_LogError(ER_FAIL, ("No TimerBusObj given"));
        return ER_FAIL;
    }

    if (m_TimerBusObj) {

        QCC_LogError(ER_FAIL, ("TimerBusObj has been previously set"));
        return ER_FAIL;
    }

    m_TimerBusObj = TimerBusObj;
    return ER_OK;
}

//Returns TimerBusObj
const TimeServiceTimerBusObj* TimeServiceServerTimer::getTimerBusObj()
{

    return m_TimerBusObj;
}

//Timer reached
QStatus TimeServiceServerTimer::timerEvent()
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!m_TimerBusObj) {

        QCC_LogError(ER_FAIL, ("This Timer hasn't been created yet"));
        return ER_FAIL;
    }

    return m_TimerBusObj->sendTimerEvent();
}


//Timer RunStateChanged
QStatus TimeServiceServerTimer::runStateChanged(bool isRunning)
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!m_TimerBusObj) {

        QCC_LogError(ER_FAIL, ("This Timer hasn't been created yet"));
        return ER_FAIL;
    }

    return m_TimerBusObj->sendRunStateChanged(isRunning);
}
