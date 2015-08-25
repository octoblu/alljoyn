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

#ifndef MYDEVICEOVENACTION_H_
#define MYDEVICEOVENACTION_H_

#include <alljoyn/controlpanel/Action.h>

namespace ajn {
namespace services {

/**
 * Generated class - implements Action Widget
 */
class MyDeviceOvenAction : public ajn::services::Action {
  public:
    MyDeviceOvenAction(qcc::String name, Widget* rootWidget);
    virtual ~MyDeviceOvenAction();

    bool executeCallBack();
};
} //namespace services
} //namespace ajn

#endif /* MYDEVICEOVENACTION_H_ */
