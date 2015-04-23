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

#ifndef TIMECLIENTANNOUNCEMENTHANDLER_H_
#define TIMECLIENTANNOUNCEMENTHANDLER_H_

#include <qcc/String.h>
#include <alljoyn/AboutObjectDescription.h>
#include <alljoyn/AboutListener.h>

namespace ajn {
namespace services {

typedef void (*OnAnnouncementCallback)(const qcc::String& busName,
                                       const qcc::String& deviceId,
                                       const qcc::String& appId,
                                       const qcc::String& uniqKey,  //[deviceId]_[AppId]
                                       const AboutObjectDescription& objectDescs);

class TimeClientAnnouncementHandler : public ajn::AboutListener {

  public:

    /**
     * Constructor
     */
    TimeClientAnnouncementHandler(OnAnnouncementCallback onAnnounceCb);

    /**
     * Destructor
     */
    ~TimeClientAnnouncementHandler();

    /**
     * Announce callback
     * @param[in] busName              well know name of the remote BusAttachment
     * @param[in] version              version of the Announce signal from the remote About Object
     * @param[in] port                 SessionPort used by the announcer
     * @param[in] objectDescriptionArg  MsgArg the list of object paths and interfaces in the announcement
     * @param[in] aboutDataArg          MsgArg containing a dictionary of Key/Value pairs of the AboutData
     */
    virtual void Announced(const char* busName, uint16_t version, SessionPort port, const MsgArg& objectDescriptionArg, const MsgArg& aboutDataArg);
  private:

    /**
     * Callback to be called when Announcement arrives
     */
    OnAnnouncementCallback m_OnAnnouncementCb;

    /**
     * Unmarshal DeviceId
     *
     * @param busName Announcement sender
     * @param deviceId. Out variable.
     * @param aboutData
     */
    void unmarshalDeviceId(const char* busName, qcc::String* deviceId, const AboutData& aboutData);

    /**
     * Unmarshal AppId
     *
     * @param busName Announcement sender
     * @param appId. Out variable.
     * @param aboutData
     */
    void unmarshalAppId(const char* busName, qcc::String* appId, const AboutData& aboutData);

    /**
     * Creates Unique key
     * @param deviceId
     * @param appId
     *
     * @return [DeviceId]_[AppId]
     */
    qcc::String createUniqueKey(const qcc::String& deviceId, const qcc::String& appId);
};
} /* namespace services */
} /* namespace ajn */

#endif /* TIMECLIENTANNOUNCEMENTHANDLER_H_ */
