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

#ifndef ANNOUNCEHANDLERSAMPLE_H_
#define ANNOUNCEHANDLERSAMPLE_H_
#include <alljoyn/AboutListener.h>
#include <alljoyn/AboutData.h>

typedef void (*BasicAnnounceHandlerCallback)(qcc::String const& busName, unsigned short port);
typedef void (*FullAnnounceHandlerCallback)(qcc::String const& busName, unsigned short version,
                                            unsigned short port, const ajn::AboutObjectDescription& objectDescription,
                                            const ajn::AboutData& aboutData);
/**
 * class AnnounceHandlerImpl
 */
class AnnounceHandlerImpl : public ajn::AboutListener {

  public:

    /**
     * Announced - announced callback
     * @param busName
     * @param version
     * @param port
     * @param objectDescriptionArg
     * @param aboutDataArg
     */
    virtual void Announced(const char* busName, uint16_t version, ajn::SessionPort port, const ajn::MsgArg& objectDescriptionArg, const ajn::MsgArg& aboutDataArg);
    /**
     * AnnounceHandlerImpl
     * @param basicCallback
     * @param fullCallback
     */
    AnnounceHandlerImpl(BasicAnnounceHandlerCallback basicCallback = 0, FullAnnounceHandlerCallback fullCallback = 0);

    /**
     * ~AnnounceHandlerImpl
     */
    ~AnnounceHandlerImpl();

  private:

    BasicAnnounceHandlerCallback m_BasicCallback;

    FullAnnounceHandlerCallback m_FullCallback;
};

#endif /* ANNOUNCEHANDLERSAMPLE_H_ */
