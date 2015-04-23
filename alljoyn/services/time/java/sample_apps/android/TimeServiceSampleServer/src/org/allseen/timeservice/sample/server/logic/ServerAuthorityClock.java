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

package org.allseen.timeservice.sample.server.logic;

import org.allseen.timeservice.DateTime;
import org.allseen.timeservice.TimeServiceException;
import org.allseen.timeservice.sample.server.ui.TimeSampleServer;
import org.allseen.timeservice.server.TimeAuthorityClock;

/**
 * Represents an TimeAuthorityClock {@link  org.allseen.timeservice.server.TimeAuthorityClock}.
 */
public class ServerAuthorityClock extends TimeAuthorityClock {

    /**
     * ServerClock storage.
     */
    private final ServerClock clock;

    /**
     * Constructor
     * @param clock
     */
    public ServerAuthorityClock(ServerClock clock) {

        TimeSampleServer.sendMessage(clock.context, TimeSampleServer.CLOCK_EVENT_ACTION, getObjectPath(), "ServerAuthorityClock created");
        this.clock = clock;
    }

    @Override
    public DateTime getDateTime() {

        TimeSampleServer.sendMessage(clock.context, TimeSampleServer.CLOCK_EVENT_ACTION, getObjectPath(), "ServerAuthorityClock getDateTime request");
        return clock.getDateTime();
    }

    @Override
    public void setDateTime(DateTime dateTime) {

        TimeSampleServer.sendMessage(clock.context, TimeSampleServer.CLOCK_EVENT_ACTION, getObjectPath(), "ServerAuthorityClock setDateTime request");
        clock.setDateTime(dateTime);
    }

    @Override
    public boolean getIsSet() {

        return true;
    }

    /**
     * Send TimeSync signal
     */
     @Override
    public void timeSync() throws TimeServiceException {
        TimeSampleServer.sendMessage(clock.context, TimeSampleServer.CLOCK_EVENT_ACTION, getObjectPath(), "ServerAuthorityClock timeSync");

        super.timeSync();
    }
}
