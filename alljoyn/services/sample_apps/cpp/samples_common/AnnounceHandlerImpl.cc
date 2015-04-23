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

#include "AnnounceHandlerImpl.h"
#include <iostream>

using namespace ajn;

AnnounceHandlerImpl::AnnounceHandlerImpl(BasicAnnounceHandlerCallback basicCallback, FullAnnounceHandlerCallback fullCallback) :
    m_BasicCallback(basicCallback), m_FullCallback(fullCallback)
{

}

AnnounceHandlerImpl::~AnnounceHandlerImpl()
{
}

void AnnounceHandlerImpl::Announced(const char* busName, uint16_t version, SessionPort port, const MsgArg& objectDescriptionArg, const MsgArg& aboutDataArg)
{
    std::cout << "*********************************************************************************" << std::endl;
    std::cout << "Announce signal discovered" << std::endl;
    std::cout << "\tFrom bus " << busName << std::endl;
    std::cout << "\tAbout version " << version << std::endl;
    std::cout << "\tSessionPort " << port << std::endl;
    std::cout << "\tObjectDescription" << std::endl << objectDescriptionArg.ToString().c_str() << std::endl;
    std::cout << "\tAboutData:" << std::endl << aboutDataArg.ToString().c_str() << std::endl;
    std::cout << "*********************************************************************************" << std::endl;

    std::cout << "AnnounceHandlerImpl::Announced()" << std::endl;
    if (m_BasicCallback) {
        std::cout << "Calling AnnounceHandler Callback" << std::endl;
        m_BasicCallback(busName, port);
    }

    if (m_FullCallback) {
        std::cout << "Calling AnnounceHandler Callback" << std::endl;
        AboutData aboutData;
        aboutData.CreatefromMsgArg(aboutDataArg);
        AboutObjectDescription aboutObjectDescription;
        aboutObjectDescription.CreateFromMsgArg(objectDescriptionArg);
        m_FullCallback(busName, version, port, aboutObjectDescription, aboutData);
    }
}

