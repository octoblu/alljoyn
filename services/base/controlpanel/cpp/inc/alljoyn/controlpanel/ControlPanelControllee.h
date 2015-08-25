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

#ifndef CONTROLPANELCONTROLLEE_H_
#define CONTROLPANELCONTROLLEE_H_

#include <alljoyn/controlpanel/ControlPanelControlleeUnit.h>

namespace ajn {
namespace services {

/**
 * ControlPanelControllee Class.
 * Allows creation of a controllee to be controller by a controller
 * Made up of ControlPanels and/or NotificationWithActions and possibly
 * a HTTPControl
 */
class ControlPanelControllee {
  public:

    /**
     * Constructor of ControlPanelControllee
     */
    ControlPanelControllee();

    /**
     * Destructor of ControlPanelControllee
     */
    virtual ~ControlPanelControllee();

    /**
     * Add a ControlPanelUnit to the ControlPanelControllee
     * @param unit - unit to add
     * @return status - success/failure
     */
    QStatus addControlPanelUnit(ControlPanelControlleeUnit* unit);

    /**
     * Register the busObjects of the ControlPanelControllee
     * @param bus - the bus to register the Objects with
     * @return status - success/failure
     */
    QStatus registerObjects(BusAttachment* bus);

    /**
     * Unregister the busObjects of the ControlPanelControllee
     * @param bus - the bus to unregister the Objects with
     * @return status - success/failure
     */
    QStatus unregisterObjects(BusAttachment* bus);

    /**
     * Get the Units of the Controllee
     * @return the Controllee Units
     */
    const std::vector<ControlPanelControlleeUnit*>& getControlleeUnits() const;

  private:

    /**
     * Vector containing the Devices Units
     */
    std::vector<ControlPanelControlleeUnit*> m_Units;
};

} /* namespace services */
} /* namespace ajn */
#endif /* CONTROLPANELCONTROLLEE_H_ */
