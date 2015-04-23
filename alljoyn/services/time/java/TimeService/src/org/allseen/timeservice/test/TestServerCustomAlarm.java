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

package org.allseen.timeservice.test;

import org.alljoyn.bus.BusException;
import org.alljoyn.bus.ErrorReplyBusException;
import org.alljoyn.bus.annotation.BusInterface;
import org.alljoyn.bus.annotation.BusMethod;
import org.allseen.timeservice.TimeServiceConst;
import org.allseen.timeservice.ajinterfaces.Alarm;
import org.allseen.timeservice.server.BaseAlarmBusObj;

public class TestServerCustomAlarm extends TestServerAlarm {

    @BusInterface(name=AlarmCoolIface.IFNAME)
    public static interface AlarmCoolIface {

        public static final String IFNAME = TimeServiceConst.IFNAME_PREFIX+".Alarm.CoolAlarm";

        @BusMethod
        String getCoolTitle() throws BusException;
    }

    //====================================================//

    public static class TestServerCustomAlarmBusObj extends BaseAlarmBusObj implements Alarm, AlarmCoolIface {

        @Override
        public String getCoolTitle() throws BusException {

            try {
                return "~~~~~~~~>>>" + getTitle() + "<<<~~~~~~~~~" ;
            } catch (BusException be) {

                throw new ErrorReplyBusException(TimeServiceConst.INVALID_VALUE_ERROR, "Failed to retrieve title");
            }
        }
    }
}
