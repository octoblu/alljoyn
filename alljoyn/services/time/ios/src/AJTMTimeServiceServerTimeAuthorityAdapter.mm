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

/**
 * This class is experimental, and as such has not been tested.
 * Please help make it more robust by contributing fixes if you find issues
 **/

#include "AJTMTimeServiceServerTimeAuthorityAdapter.h"

AJTMTimeServiceServerTimeAuthorityAdapter::AJTMTimeServiceServerTimeAuthorityAdapter(id<AJTMTimeServiceServerClock> timeServiceServerClock)
{
    handle = timeServiceServerClock;
}

AJTMTimeServiceServerTimeAuthorityAdapter::~AJTMTimeServiceServerTimeAuthorityAdapter()
{
}


const ajn::services::TimeServiceDateTime& AJTMTimeServiceServerTimeAuthorityAdapter::getDateTime() const
{
    return *[[handle dateTime] getHandle];

}

void AJTMTimeServiceServerTimeAuthorityAdapter::setDateTime(ajn::services::TimeServiceDateTime const& dateTime)
{
    ajn::services::TimeServiceDateTime *cDateTime = new ajn::services::TimeServiceDateTime();
    cDateTime->init(dateTime.getDate(), dateTime.getTime(), dateTime.getOffsetMinutes());
    AJTMTimeServiceDateTime *dateTimeTmp = [[AJTMTimeServiceDateTime alloc] initWithHandle:cDateTime];
    [handle setDateTime:dateTimeTmp];
}

bool AJTMTimeServiceServerTimeAuthorityAdapter::isSet() const
{
    return [handle isSet];
}
