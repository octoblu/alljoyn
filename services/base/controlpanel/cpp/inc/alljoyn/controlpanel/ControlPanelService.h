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

#ifndef CONTROLPANELSERVICE_H_
#define CONTROLPANELSERVICE_H_

#include <alljoyn/BusAttachment.h>
#include <alljoyn/controlpanel/ControlPanelControllee.h>
#include <alljoyn/controlpanel/ControlPanelController.h>
#include <alljoyn/controlpanel/ControlPanelBusListener.h>
#include <alljoyn/controlpanel/ControlPanelListener.h>

namespace ajn {
namespace services {

/**
 * ControlPanel Service class. Used to initialize and shutdown the service
 */
class ControlPanelService {

  public:

    /**
     * Get Instance of ControlPanelServiceImpl - singleton implementation
     * @return instance
     */
    static ControlPanelService* getInstance();

    /**
     * Destructor for ControlPanelServiceImpl
     */
    ~ControlPanelService();

    /**
     * Initialize the controllee to be used
     * @param bus - bus used for Controllee
     * @param controlPanelControllee - controllee to initialize
     * @return status - success/failure
     */
    QStatus initControllee(BusAttachment* bus, ControlPanelControllee* controlPanelControllee);

    /**
     * Remove locally stored controllee. Allows a new call to initControllee to be made
     * @return status - success/failure
     */
    QStatus shutdownControllee();

    /**
     * Initialize the controller to be used
     * @param bus - bus used for Controller
     * @param controlPanelController - controller to initialize
     * @param controlPanelListener
     * @return status - success/failure
     */
    QStatus initController(BusAttachment* bus, ControlPanelController* controlPanelController,
                           ControlPanelListener* controlPanelListener);

    /**
     * Remove locally stored controller. Allows a new call to initController to be made
     * @return status - success/failure
     */
    QStatus shutdownController();

    /**
     * Full shutdown - includes removing the current BusAttachment
     * @return status - success/failure
     */
    QStatus shutdown();

    /**
     * Method to get the busAttachment used in the service.
     * @return BusAttachment
     */
    ajn::BusAttachment* getBusAttachment();

    /**
     * Get the BusListener
     * @return ControlPanelBusListener
     */
    ControlPanelBusListener* getBusListener() const;

    /**
     * Get the ControlPanelListener
     * @return ControlPanelListener
     */
    ControlPanelListener* getControlPanelListener() const;

    /**
     * Utility function to split an objectPath based on the / delimeter
     * @param objectPath
     * @return vector of objectPath parts
     */
    static std::vector<qcc::String> SplitObjectPath(qcc::String const& objectPath);

    /**
     * Get the Version of the ControlPanelService
     * @return the ControlPanelService version
     */
    static uint16_t getVersion();

  private:

    /**
     * Default constructor for ControlPanelServiceImpl
     * Private to allow for singleton implementation
     */
    ControlPanelService();

    /**
     * Copy constructor of ControlPanelService - private. ControlPanelService is not copy-able
     * @param handler - ControlPanelService to copy
     */
    ControlPanelService(const ControlPanelService& handler);

    /**
     * Assignment operator of ControlPanelService - private. ControlPanelService is not assignable
     * @param handler
     * @return ControlPanelService
     */
    ControlPanelService& operator=(const ControlPanelService& handler);

    /**
     * Version of the API
     */
    static uint16_t const CONTROLPANEL_SERVICE_VERSION;

    /**
     * Instance variable - ControlPanelServiceImpl is a singleton
     */
    static ControlPanelService* s_Instance;

    /**
     * BusAttachement used in Service
     */
    BusAttachment* m_Bus;

    /**
     * BusListener used in service
     */
    ControlPanelBusListener* m_BusListener;

    /**
     * ControlPanelControllee of service
     */
    ControlPanelControllee* m_ControlPanelControllee;

    /**
     * ControlPanelController of service
     */
    ControlPanelController* m_ControlPanelController;

    /**
     * ControlPanelListener of service
     */
    ControlPanelListener* m_ControlPanelListener;

};
} //namespace services
} //namespace ajn

#endif /* CONTROLPANELSERVICE_H_ */
