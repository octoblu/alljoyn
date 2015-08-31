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

#ifndef CONTROLPANELCONTROLLER_H_
#define CONTROLPANELCONTROLLER_H_

#include <alljoyn/controlpanel/ControlPanelDevice.h>
#include <map>
#include <alljoyn/about/AnnounceHandler.h>

namespace ajn {
namespace services {

/**
 * Class to facilitate controlling of remote ControlPanels
 */
class ControlPanelController {
  public:
    /**
     * ControlPanelController()
     */
    ControlPanelController();

    /**
     * ~ControlPanelController()
     */
    virtual ~ControlPanelController();

    /**
     * create a controllable device by parsing announce descriptions.
     * @param sender
     * @param objectDescs - ObjectDescriptions received in announce
     * @return a ControlPanelDevice
     */
    ControlPanelDevice* createControllableDevice(qcc::String const& sender,
                                                 const AboutObjectDescription& objectDescs);

    /**
     * create a controllable device by parsing announce descriptions.
     * @param deviceBusName - BusName of device received in announce
     * @param objectDescs - ObjectDescriptions received in announce
     * @return a ControlPanelDevice
     */
    ControlPanelDevice* createControllableDevice(qcc::String const& deviceBusName, const AnnounceHandler::ObjectDescriptions& objectDescs);

    /**
     * GetControllableDevice - get a device using the busName - creates it if it doesn't exist
     * @param deviceBusName - deviceName to get
     * @return ControlPanelDevice* - returns the Device
     */
    ControlPanelDevice* getControllableDevice(qcc::String const& deviceBusName);

    /**
     * deleteControllableDevice - shutdown a controllable device and delete it from the Controller
     * @param deviceBusName - deviceName to delete
     * @return status - success-failure
     */
    QStatus deleteControllableDevice(qcc::String const& deviceBusName);

    /**
     * deleteAllControllableDevices - shutdown and delete all controllable devices from the controller
     * @return status - success-failure
     */
    QStatus deleteAllControllableDevices();

    /**
     * Get map of All Controllable Devices
     * @return controllable Devices map
     */
    const std::map<qcc::String, ControlPanelDevice*>& getControllableDevices() const;

  private:

    /**
     * Map of ControllableDevices added to Controller
     */
    std::map<qcc::String, ControlPanelDevice*> m_ControllableDevices;
};

} /* namespace services */
} /* namespace ajn */
#endif /* CONTROLPANELCONTROLLER_H_ */
