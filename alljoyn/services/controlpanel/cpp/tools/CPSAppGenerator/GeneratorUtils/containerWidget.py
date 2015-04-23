# Copyright AllSeen Alliance. All rights reserved.
#
#    Permission to use, copy, modify, and/or distribute this software for any
#    purpose with or without fee is hereby granted, provided that the above
#    copyright notice and this permission notice appear in all copies.
#
#    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
#    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
#    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
#    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
#    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
#    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
#    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
import sys
import propertyWidget as pw
import listPropertyWidget as lpw
import actionWidget as aw
import labelWidget as lw
import commonWidget as common

class Container (common.Widget):

    def __init__(self, generated, element, parentName, languageSetName, isRoot = 0) :
        common.Widget.__init__(self, generated, element, parentName, languageSetName)
        self.isRoot = isRoot
        self.widgetName = "Container"
        if isRoot :
            self.parentAddFunc = "setRootWidget"

    def generate(self) :
        common.Widget.generate(self)
        if self.isRoot:
            self.setDismissable()
        self.generateChildElements()

    def setDismissable(self) :
        if not hasattr(self.element, "dismissable") :
            return
        self.generated.initCode += "    {0}->setIsDismissable({1});\n".format(self.name, self.element.dismissable)

    def generateChildElements (self) :
        elements = self.element.elements.sub_nodes

        for element in elements:
            elementType = element._name

            if elementType == "action" :
                action = aw.Action(self.generated, element, self.name, self.languageSetName)
                action.generate()
            elif elementType == "container" :
                container = Container(self.generated, element, self.name, self.languageSetName)
                container.generate()
            elif elementType == "scalarProperty" or elementType == "stringProperty" or elementType == "booleanProperty" :
                propertyW = pw.Property(self.generated, element, self.name, self.languageSetName)
                propertyW.generate()
            elif elementType == "dateProperty" or elementType == "timeProperty" :
                propertyW = pw.Property(self.generated, element, self.name, self.languageSetName)
                propertyW.generate()
            elif elementType == "labelProperty" :
                label = lw.Label(self.generated, element, self.name, self.languageSetName)
                label.generate()
#            elif elementType == "listProperty" :
#                listProp = lpw.ListProperty(self.generated, element, (self.parentObjectPath + self.objectPathSuffix), self.languageSetName)
#                listProp.generate()
            else :
                print >> sys.stderr, "ERROR - This type is not supported. Exiting " + elementType
                sys.exit(2)




