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

#include "MyDeviceSet_temperature.h"
#include "ControlPanelProvided.h"


namespace ajn {
namespace services {

MyDeviceSet_temperature::MyDeviceSet_temperature(qcc::String name, Widget* rootWidget, PropertyType propertyType) :
    Property(name, rootWidget, propertyType)
{

}

MyDeviceSet_temperature::~MyDeviceSet_temperature()
{

}

QStatus MyDeviceSet_temperature::setValue(uint16_t value)
{
    setTargetTemperature(value);
    return ER_OK;
}

} /* namespace services */
} /* namespace ajn */
