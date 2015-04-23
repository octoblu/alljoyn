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

#include "MyDeviceLightConfirm.h"
#include "ControlPanelProvided.h"


namespace ajn {
namespace services {

MyDeviceLightConfirm::MyDeviceLightConfirm(qcc::String name, Widget* rootWidget) : Dialog(name, rootWidget)
{

}

MyDeviceLightConfirm::~MyDeviceLightConfirm()
{

}

bool MyDeviceLightConfirm::executeAction1CallBack()
{
    AJ_Printf("Execute Action1 was called\n");

    //by default return success
    return true;
}

bool MyDeviceLightConfirm::executeAction2CallBack()
{
    AJ_Printf("Execute Action2 was called\n");

    //by default return success
    return true;
}

bool MyDeviceLightConfirm::executeAction3CallBack()
{
    AJ_Printf("Execute Action3 was called\n");

    //by default return success
    return true;
}

} /* namespace services */
} /* namespace ajn */
