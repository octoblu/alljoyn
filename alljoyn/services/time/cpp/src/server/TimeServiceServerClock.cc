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

#include <alljoyn/time/TimeServiceServerClock.h>
#include "TimeServiceClockBusObj.h"
#include <alljoyn/time/LogModule.h>

using namespace ajn;
using namespace services;

//Constructor
TimeServiceServerClock::TimeServiceServerClock() : m_ClockBusObj(NULL)
{

    QCC_DbgTrace(("%s", __FUNCTION__));
}

// Destructor
TimeServiceServerClock::~TimeServiceServerClock()
{

    QCC_DbgTrace(("%s", __FUNCTION__));
    release();
}

//Releases object resources
void TimeServiceServerClock::release()
{

    QCC_DbgTrace(("%s", __FUNCTION__));

    if (!m_ClockBusObj) {

        return;
    }

    QCC_DbgHLPrintf(("Releasing the Clock '%s'", m_ClockBusObj->getObjectPath().c_str()));

    delete m_ClockBusObj;
    m_ClockBusObj = NULL;
}

//Returns object path of this Clock
qcc::String const& TimeServiceServerClock::getObjectPath() const
{

    if (!m_ClockBusObj) {

        QCC_LogError(ER_FAIL, ("This Clock hasn't been created yet"));
        return qcc::String::Empty;
    }

    return m_ClockBusObj->getObjectPath();
}

//Set Clock Bus Object
QStatus TimeServiceServerClock::setClockBusObj(TimeServiceClockBusObj* clockBusObj)
{

    if (!clockBusObj) {

        QCC_LogError(ER_FAIL, ("No clockBusObj given"));
        return ER_FAIL;
    }

    if (m_ClockBusObj) {

        QCC_LogError(ER_FAIL, ("clockBusObj has been previously set"));
        return ER_FAIL;
    }

    m_ClockBusObj = clockBusObj;
    return ER_OK;
}

//Returns ClockBusObj
const TimeServiceClockBusObj* TimeServiceServerClock::getClockBusObj()
{

    return m_ClockBusObj;
}
