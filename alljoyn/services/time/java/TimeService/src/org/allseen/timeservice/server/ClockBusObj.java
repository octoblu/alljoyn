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

package org.allseen.timeservice.server;

import org.allseen.timeservice.TimeServiceException;
import org.allseen.timeservice.ajinterfaces.Clock;

/**
 * This class implements {@link Clock} interface and realizes AllJoyn
 * communication with this clock.
 */
class ClockBusObj extends BaseClockBusObj implements Clock {

    /**
     * Prefix of the Clock object path
     */
    private static final String OBJ_PATH_PREFIX = "/Clock";

    /**
     * Constructor
     *
     * @param clock
     *            Clock events delegate. This clock receives all the
     *            {@link org.allseen.timeservice.server.Clock} related events.
     */
    ClockBusObj(org.allseen.timeservice.server.Clock clock) throws TimeServiceException {

        super(clock, GlobalStringSequencer.append(OBJ_PATH_PREFIX));

    }

    /**
     * @see org.allseen.timeservice.server.BaseClockBusObj#release()
     */
    @Override
    protected void release() {

        super.release();

    }
}
