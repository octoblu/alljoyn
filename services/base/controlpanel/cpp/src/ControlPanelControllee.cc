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

#include <alljoyn/controlpanel/ControlPanelControllee.h>
#include <alljoyn/controlpanel/ControlPanelService.h>
#include "ControlPanelConstants.h"
#include <alljoyn/controlpanel/LogModule.h>

namespace ajn {
namespace services {
using namespace cpsConsts;

ControlPanelControllee::ControlPanelControllee()
{
}

ControlPanelControllee::~ControlPanelControllee()
{

}

QStatus ControlPanelControllee::registerObjects(BusAttachment* bus)
{
    QStatus status = ER_OK;

    for (size_t indx = 0; indx < m_Units.size(); indx++) {
        status = m_Units[indx]->registerObjects(bus);
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not register Objects for the Units"));
            return status;
        }
    }
    return status;
}

QStatus ControlPanelControllee::unregisterObjects(BusAttachment* bus)
{
    QStatus returnStatus = ER_OK;

    for (size_t indx = 0; indx < m_Units.size(); indx++) {
        QStatus status = m_Units[indx]->unregisterObjects(bus);
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not register Objects for the Units"));
            returnStatus = status;
        }
    }

    return returnStatus;
}

QStatus ControlPanelControllee::addControlPanelUnit(ControlPanelControlleeUnit* unit)
{
    if (!unit) {
        QCC_DbgHLPrintf(("Could not add a NULL unit"));
        return ER_BAD_ARG_1;
    }

    m_Units.push_back(unit);
    return ER_OK;
}

const std::vector<ControlPanelControlleeUnit*>& ControlPanelControllee::getControlleeUnits() const
{
    return m_Units;
}

} /* namespace services */
} /* namespace ajn */
