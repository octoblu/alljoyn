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

import java.util.concurrent.atomic.AtomicInteger;

/**
 * This is utility class that is used to append a unique number to received string.
 * This class is not designed to be initialized.
 */
class GlobalStringSequencer {

    /**
     * Current number
     */
    private static final AtomicInteger CURRENT = new AtomicInteger(0);

    /**
     * Private Constructor
     */
    private GlobalStringSequencer() {
    }

    /**
     * Initialize CURRENT value
     */
    public static void init() {

        CURRENT.set(0);
    }

    /**
     * Appends to the given prefix the current number.
     * @param prefix String to be appended
     * @return Appended string
     */
    public static String append(String prefix) {

        return prefix + CURRENT.incrementAndGet();
    }
}
