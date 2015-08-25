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

#ifndef INTROSPECTIONNODE_H_
#define INTROSPECTIONNODE_H_

#include <qcc/String.h>
#include <alljoyn/controlpanel/ControlPanelEnums.h>

namespace ajn {
namespace services {

/*
 *
 */
class IntrospectionNode {

  public:

    IntrospectionNode(qcc::String const& objectPath, WidgetType widgetType, bool secured = false);

    virtual ~IntrospectionNode();

    const qcc::String& getObjectPath() const;

    void setObjectPath(const qcc::String& objectPath);

    bool isSecured() const;

    void setSecured(bool secured);

    WidgetType getWidgetType() const;

    void setWidgetType(WidgetType widgetType);

  private:

    qcc::String m_ObjectPath;

    WidgetType m_WidgetType;

    bool m_Secured;
};

} /* namespace services */
} /* namespace ajn */

#endif /* INTROSPECTIONNODE_H_ */
