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

import org.allseen.timeservice.server.TimeAuthorityClock;

/**
 * The different sources a {@link TimeAuthorityClock} can synchronize its time against.
 */
public enum AuthorityType {

    OTHER((byte)0),
    NTP((byte)1),
    CELLULAR((byte)2),
    GPS((byte)3),
    USER_DESIGNATED((byte)4)
    ;

    /**
     * Authority type
     */
    private final byte TYPE_VALUE;

    /**
     * Constructor
     * @param type Authority type
     */
    private AuthorityType(byte type) {

        TYPE_VALUE = type;
    }

    /**
     * Return the numeric value of the {@link AuthorityType}
     * @return authority type numeric value
     */
    public byte getValue() {

        return TYPE_VALUE;
    }

    /**
     * Looks for {@link AuthorityType} with the given type value
     * @param typeValue The value of the {@link AuthorityType} to look for
     * @return {@link AuthorityType} if found otherwise NULL is returned
     */
    public static AuthorityType getType(byte typeValue) {

        AuthorityType[] types = AuthorityType.values();
        for ( AuthorityType type : types ) {

            if ( type.TYPE_VALUE == typeValue ) {

                return type;
            }
        }

        return null;
    }
}
