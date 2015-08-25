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

#include "IntrospectionNode.h"

namespace ajn {
namespace services {

IntrospectionNode::IntrospectionNode(qcc::String const& objectPath, WidgetType widgetType, bool secured) :
    m_ObjectPath(objectPath), m_WidgetType(widgetType), m_Secured(secured)
{

}

IntrospectionNode::~IntrospectionNode()
{

}

const qcc::String& IntrospectionNode::getObjectPath() const
{
    return m_ObjectPath;
}

void IntrospectionNode::setObjectPath(const qcc::String& objectPath)
{
    m_ObjectPath = objectPath;
}

bool IntrospectionNode::isSecured() const
{
    return m_Secured;
}

void IntrospectionNode::setSecured(bool secured)
{
    m_Secured = secured;
}

WidgetType IntrospectionNode::getWidgetType() const
{
    return m_WidgetType;
}

void IntrospectionNode::setWidgetType(WidgetType widgetType)
{
    m_WidgetType = widgetType;
}

} /* namespace services */
} /* namespace ajn */
