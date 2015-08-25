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

#ifndef CONTROLPANELSESSIONHANDLER_H_
#define CONTROLPANELSESSIONHANDLER_H_

#include <alljoyn/BusAttachment.h>
#include <alljoyn/SessionListener.h>

namespace ajn {
namespace services {

class ControlPanelDevice;

/**
 * class ControlPanelSessionHandler
 */
class ControlPanelSessionHandler : public ajn::BusAttachment::JoinSessionAsyncCB, public ajn::SessionListener {
  public:

    /**
     * Constructor for ControlPanelSessionHandler
     */
    ControlPanelSessionHandler(ControlPanelDevice* device);

    /**
     * Destructor for ControlPanelSessionHandler
     */
    virtual ~ControlPanelSessionHandler();

    /**
     * SessionLost
     * @param sessionId
     */
    void SessionLost(ajn::SessionId sessionId);

    /**
     * JoinSessionCB
     * @param status
     * @param id
     * @param opts
     * @param context
     */
    void JoinSessionCB(QStatus status, ajn::SessionId id, const ajn::SessionOpts& opts, void* context);

    /**
     * getSessionId
     * @return SessionId
     */
    ajn::SessionId getSessionId() const;

  private:

    /**
     * SessionId for this Device
     */
    ajn::SessionId m_SessionId;

    /**
     * The device of this Session Handler
     */
    ControlPanelDevice* m_Device;

};

} /* namespace services */
} /* namespace ajn */

#endif /* CONTROLPANELSESSIONHANDLER_H_ */
