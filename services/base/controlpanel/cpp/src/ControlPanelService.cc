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

#include <algorithm>
#include <sstream>

#include <qcc/Debug.h>
#include <alljoyn/controlpanel/ControlPanelService.h>
#include "ControlPanelConstants.h"
#include <alljoyn/controlpanel/LogModule.h>

namespace ajn {
namespace services {

using namespace qcc;
using namespace cpsConsts;

ControlPanelService* ControlPanelService::s_Instance(0);
uint16_t const ControlPanelService::CONTROLPANEL_SERVICE_VERSION = 1;

ControlPanelService* ControlPanelService::getInstance()
{
    if (!s_Instance) {
        s_Instance = new ControlPanelService();
    }

    return s_Instance;
}

ControlPanelService::ControlPanelService() :
    m_Bus(0), m_BusListener(0), m_ControlPanelControllee(0),
    m_ControlPanelController(0), m_ControlPanelListener(0)
{
}

ControlPanelService::~ControlPanelService()
{
    QCC_DbgPrintf(("Shutting down"));

    if (m_BusListener) {
        if (m_Bus) {
            m_Bus->UnregisterBusListener(*m_BusListener);
        }
        delete m_BusListener;
        m_BusListener = 0;
    }

    if (this == s_Instance) {
        s_Instance = 0;
    }
}

uint16_t ControlPanelService::getVersion()
{
    return CONTROLPANEL_SERVICE_VERSION;
}

QStatus ControlPanelService::initControllee(BusAttachment* bus, ControlPanelControllee* controlPanelControllee)
{
    QCC_DbgPrintf(("Initializing Controllee"));

    if (!bus) {
        QCC_DbgHLPrintf(("Bus cannot be NULL"));
        return ER_BAD_ARG_1;
    }

    if (!bus->IsStarted()) {
        QCC_DbgHLPrintf(("Bus is not started"));
        return ER_BAD_ARG_1;
    }

    if (!bus->IsConnected()) {
        QCC_DbgHLPrintf(("Bus is not connected"));
        return ER_BAD_ARG_1;
    }

    if (m_Bus && m_Bus->GetUniqueName().compare(bus->GetUniqueName()) != 0) {
        QCC_DbgHLPrintf(("Bus is already set to different BusAttachment"));
        return ER_BAD_ARG_1;
    }

    if (!controlPanelControllee) {
        QCC_DbgHLPrintf(("ControlPanelControllee cannot be null"));
        return ER_BAD_ARG_2;
    }

    if (m_ControlPanelControllee) {
        QCC_DbgHLPrintf(("ControlPanelControllee already initialized"));
        return ER_BUS_OBJ_ALREADY_EXISTS;
    }

    if (m_BusListener) {
        QCC_DbgHLPrintf(("BusListener already initialized"));
        return ER_BUS_OBJ_ALREADY_EXISTS;
    }

    m_Bus = bus;
    m_ControlPanelControllee = controlPanelControllee;

    QStatus status = controlPanelControllee->registerObjects(bus);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not register the BusObjects"));
        return status;
    }

    m_BusListener = new ControlPanelBusListener();
    m_BusListener->setSessionPort(CONTROLPANELSERVICE_PORT);
    m_Bus->RegisterBusListener(*m_BusListener);

    SessionPort servicePort = CONTROLPANELSERVICE_PORT;
    SessionOpts sessionOpts(SessionOpts::TRAFFIC_MESSAGES, true, SessionOpts::PROXIMITY_ANY, TRANSPORT_ANY);

    status = m_Bus->BindSessionPort(servicePort, sessionOpts, *m_BusListener);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not bind Session Port successfully"));
        return status;
    }
    QCC_DbgPrintf(("Initialized Controllee successfully"));
    return status;
}

QStatus ControlPanelService::shutdownControllee()
{
    if (!m_ControlPanelControllee) {
        QCC_DbgHLPrintf(("ControlPanelControllee not initialized. Returning"));
        return ER_OK;
    }

    if (!m_Bus) {
        QCC_DbgHLPrintf(("Bus not set."));
        return ER_BUS_BUS_NOT_STARTED;
    }

    if (m_BusListener) {
        m_Bus->UnregisterBusListener(*m_BusListener);
        delete m_BusListener;
        m_BusListener = 0;
    }

    TransportMask transportMask = TRANSPORT_ANY;
    SessionPort sp = CONTROLPANELSERVICE_PORT;
    SessionOpts opts(SessionOpts::TRAFFIC_MESSAGES, false, SessionOpts::PROXIMITY_ANY, transportMask);

    QStatus returnStatus = ER_OK;
    QStatus status = m_Bus->UnbindSessionPort(sp);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not unbind the SessionPort"));
        returnStatus = status;
    }

    status = m_ControlPanelControllee->unregisterObjects(m_Bus);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not unregister the BusObjects"));
        returnStatus = status;
    }

    m_ControlPanelControllee = 0;
    return returnStatus;
}

QStatus ControlPanelService::initController(BusAttachment* bus, ControlPanelController* controlPanelController,
                                            ControlPanelListener* controlPanelListener)
{
    QCC_DbgTrace(("Initializing Controller"));

    if (!bus) {
        QCC_DbgHLPrintf(("Bus cannot be NULL"));
        return ER_BAD_ARG_1;
    }

    if (!bus->IsStarted()) {
        QCC_DbgHLPrintf(("Bus is not started"));
        return ER_BAD_ARG_1;
    }

    if (!bus->IsConnected()) {
        QCC_DbgHLPrintf(("Bus is not connected"));
        return ER_BAD_ARG_1;
    }

    if (m_Bus && m_Bus->GetUniqueName().compare(bus->GetUniqueName()) != 0) {
        QCC_DbgHLPrintf(("Bus is already set to different BusAttachment"));
        return ER_BAD_ARG_1;
    }

    if (!controlPanelController) {
        QCC_DbgHLPrintf(("ControlPanelController cannot be null"));
        return ER_BAD_ARG_2;
    }

    if (m_ControlPanelController) {
        QCC_DbgHLPrintf(("ControlPanelController already initialized"));
        return ER_BUS_OBJ_ALREADY_EXISTS;
    }

    if (!controlPanelListener) {
        QCC_DbgHLPrintf(("ControlPanelListener cannot be null"));
        return ER_BAD_ARG_3;
    }

    if (m_ControlPanelListener) {
        QCC_DbgHLPrintf(("m_ControlPanelListener already initialized"));
        return ER_BUS_OBJ_ALREADY_EXISTS;
    }

    m_Bus = bus;
    m_ControlPanelController = controlPanelController;
    m_ControlPanelListener = controlPanelListener;

    QCC_DbgPrintf(("Initialized Controller successfully"));
    return ER_OK;
}

QStatus ControlPanelService::shutdownController()
{
    if (!m_ControlPanelController) {
        QCC_DbgHLPrintf(("ControlPanelControllee not initialized. Returning"));
        return ER_OK;
    }

    if (!m_Bus) {
        QCC_DbgHLPrintf(("Bus not set."));
        return ER_BUS_BUS_NOT_STARTED;
    }

    QStatus status = m_ControlPanelController->deleteAllControllableDevices();
    if (status != ER_OK) {
        QCC_DbgHLPrintf(("Could not stop all Controllable Devices"));
    }

    m_ControlPanelController = 0;
    m_ControlPanelListener = 0;

    return status;
}

QStatus ControlPanelService::shutdown()
{
    QStatus returnStatus = ER_OK;

    QStatus status = shutdownController();
    if (status != ER_OK) {
        returnStatus = status;
        QCC_DbgHLPrintf(("Could not shutdown Controller successfully"));
    }

    status = shutdownControllee();
    if (status != ER_OK) {
        returnStatus = status;
        QCC_DbgHLPrintf(("Could not shutdown Controllee successfully"));
    }

    m_Bus = 0;
    return returnStatus;
}

BusAttachment* ControlPanelService::getBusAttachment()
{
    return m_Bus;
}

ControlPanelBusListener* ControlPanelService::getBusListener() const
{
    return m_BusListener;
}

ControlPanelListener* ControlPanelService::getControlPanelListener() const
{
    return m_ControlPanelListener;
}

std::vector<qcc::String> ControlPanelService::SplitObjectPath(qcc::String const& objectPath)
{
    std::vector<qcc::String> results;

    size_t prev = 0;
    size_t next = 0;

    while ((next = objectPath.find_first_of("/", prev)) != qcc::String::npos) {
        if (next - prev != 0) {
            results.push_back(objectPath.substr(prev, next - prev));
        }
        prev = next + 1;
    }

    if (prev < objectPath.size()) {
        results.push_back(objectPath.substr(prev));
    }

    return results;
}

} /* namespace services */
} /* namespace ajn */
