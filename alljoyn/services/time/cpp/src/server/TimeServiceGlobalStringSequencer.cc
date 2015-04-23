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

#include "TimeServiceGlobalStringSequencer.h"
#include <sstream>

using namespace ajn;
using namespace services;

//Initialize self pointer
TimeServiceGlobalStringSequencer* TimeServiceGlobalStringSequencer::s_Instance(NULL);

//Private Constructor
TimeServiceGlobalStringSequencer::TimeServiceGlobalStringSequencer() : m_Mutex(), m_Current(0)
{
}

//Desctructor
TimeServiceGlobalStringSequencer::~TimeServiceGlobalStringSequencer() {
}

//Get the object instance
TimeServiceGlobalStringSequencer* TimeServiceGlobalStringSequencer::getInstance()
{

    if (!s_Instance) {

        s_Instance = new TimeServiceGlobalStringSequencer();
    }

    return s_Instance;
}

// Initialize the sequencer
QStatus TimeServiceGlobalStringSequencer::init()
{

    QStatus status = m_Mutex.Lock();
    if (status != ER_OK) {

        return status;
    }

    m_Current = 0;

    status = m_Mutex.Unlock();

    return status;
}

// Appends to the given prefix the current number.
QStatus TimeServiceGlobalStringSequencer::append(qcc::String* objPath)
{

    if (!objPath) {

        return ER_FAIL;
    }

    uint32_t num;

    QStatus status = m_Mutex.Lock();

    if (status != ER_OK) {

        return status;
    }

    ++m_Current;
    num = m_Current;

    status  = m_Mutex.Unlock();

    qcc::String numStr;
    numToString(num,  &numStr);

    *objPath += numStr;

    return status;
}

//Converts received number to string
void TimeServiceGlobalStringSequencer::numToString(uint32_t num, qcc::String* convString)
{

    std::ostringstream strStream;
    strStream << num;

    convString->assign(strStream.str().c_str());
}
