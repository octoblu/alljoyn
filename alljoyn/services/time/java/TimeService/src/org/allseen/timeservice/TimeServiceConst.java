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

package org.allseen.timeservice;

import org.allseen.timeservice.server.TimeServiceServer;

/**
 * Constant definitions of the Time Service
 */
public class TimeServiceConst {

    /**
     * This is the prefix for all the interfaces of the Time Service
     */
    public static final String IFNAME_PREFIX       = "org.allseen.Time";

    /**
     * Port number that the {@link TimeServiceServer} listens to
     */
    public static final short PORT_NUM             = 1030;

    /**
     * General Error
     */
    public static final String GENERIC_ERROR       = "org.alljoyn.Error";

    /**
     * Invalid Value Error
     */
    public static final String INVALID_VALUE_ERROR = "org.alljoyn.Error.InvalidValue";

    /**
     * Out Of Range Error
     */
    public static final String OUT_OF_RANGE        = "org.alljoyn.Error.OutOfRange";

    /**
     * No need to initialize the class
     */
    private TimeServiceConst(){}
}
