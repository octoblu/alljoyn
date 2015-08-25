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
import xml.etree.ElementTree as xml
import commonWidget as common
import containerWidget as cw
import propertyWidget as pw

class ListProperty (common.Widget):

    def __init__(self, generated, listPropElement, parentObjectPath, languageSetName) :
        common.Widget.__init__(self, generated, listPropElement, parentObjectPath, languageSetName)
        self.widgetName = "ListPropertyWidget"
        self.widgetType = "WIDGET_TYPE_LISTPROPERTY"
        self.nonSecuredInterfaceName = "ListPropertyInterfaces"
        self.securedInterfaceName = "SecuredListPropertyInterfaces"
        self.hintPrefix = "LIST_PROPERTY_WIDGET_HINT_"

    def generate(self) :
        common.Widget.generate(self)
        self.generateChildElements()

    def generateDefines(self, capName) :
        common.Widget.generateDefines(self, capName) 
        self.generated.defines += "#define {0}_VALUE_PROPERTY             AJ_APP_PROPERTY_ID({1} + NUM_PRECEDING_OBJECTS, 1, 4)\n".format(capName, self.generated.definesIndx)
        self.generated.defines += "#define {0}_SIGNAL_VALUE_CHANGED       AJ_APP_MESSAGE_ID({1} + NUM_PRECEDING_OBJECTS, 1, 5)\n".format(capName, self.generated.definesIndx)
        self.generated.defines += "#define {0}_ADD                        AJ_APP_MESSAGE_ID({1} + NUM_PRECEDING_OBJECTS, 1, 6)\n".format(capName, self.generated.definesIndx)
        self.generated.defines += "#define {0}_DELETE                     AJ_APP_MESSAGE_ID({1} + NUM_PRECEDING_OBJECTS, 1, 7)\n".format(capName, self.generated.definesIndx)
        self.generated.defines += "#define {0}_VIEW                       AJ_APP_MESSAGE_ID({1} + NUM_PRECEDING_OBJECTS, 1, 8)\n".format(capName, self.generated.definesIndx)
        self.generated.defines += "#define {0}_UPDATE                     AJ_APP_MESSAGE_ID({1} + NUM_PRECEDING_OBJECTS, 1, 9)\n".format(capName, self.generated.definesIndx)
        self.generated.defines += "#define {0}_CONFIRM                    AJ_APP_MESSAGE_ID({1} + NUM_PRECEDING_OBJECTS, 1, 10)\n".format(capName, self.generated.definesIndx)
        self.generated.defines += "#define {0}_CANCEL                     AJ_APP_MESSAGE_ID({1} + NUM_PRECEDING_OBJECTS, 1, 11)\n".format(capName, self.generated.definesIndx)

        self.generated.listPropCases += "        		case {0}_ADD:\\\n".format(capName)
        self.generated.listPropCases += "        		case {0}_DELETE:\\\n".format(capName)
        self.generated.listPropCases += "        		case {0}_VIEW:\\\n".format(capName)
        self.generated.listPropCases += "        		case {0}_UPDATE:\\\n".format(capName)
        self.generated.listPropCases += "        		case {0}_CONFIRM:\\\n".format(capName)
        self.generated.listPropCases += "        		case {0}_CANCEL:\\\n".format(capName)

    def generateIdentify(self, capName, language) :
        common.Widget.generateIdentify(self, capName, language)

        self.generated.identify += """\t\tcase {0}_VALUE_PROPERTY :
            *widgetType = WIDGET_TYPE_LISTPROPERTY;
            *propType = PROPERTY_TYPE_VALUE;
            *language = {2};
            return &{1};\n""".format(capName, self.name, language)
        self.generated.identify += """\t\tcase {0}_ADD :
            *widgetType = WIDGET_TYPE_LISTPROPERTY;
            *propType = METHOD_TYPE_ADD;
            *language = {2};
            return &{1};\n""".format(capName, self.name, language)
        self.generated.identify += """\t\tcase {0}_DELETE :
            *widgetType = WIDGET_TYPE_LISTPROPERTY;
            *propType = METHOD_TYPE_DELETE;
            *language = {2};
            return &{1};\n""".format(capName, self.name, language)
        self.generated.identify += """\t\tcase {0}_VIEW :
            *widgetType = WIDGET_TYPE_LISTPROPERTY;
            *propType = METHOD_TYPE_VIEW;
            *language = {2};
            return &{1};\n""".format(capName, self.name, language)
        self.generated.identify += """\t\tcase {0}_UPDATE :
            *widgetType = WIDGET_TYPE_LISTPROPERTY;
            *propType = METHOD_TYPE_UPDATE;
            *language = {2};
            return &{1};\n""".format(capName, self.name, language)
        self.generated.identify += """\t\tcase {0}_CONFIRM :
            *widgetType = WIDGET_TYPE_LISTPROPERTY;
            *propType = METHOD_TYPE_CONFIRM;
            *language = {2};
            return &{1};\n""".format(capName, self.name, language)
        self.generated.identify += """\t\tcase {0}_CANCEL :
            *widgetType = WIDGET_TYPE_LISTPROPERTY;
            *propType = METHOD_TYPE_CANCEL;
            *language = {2};
            return &{1};\n""".format(capName, self.name, language)

        self.generated.identifySignal += """\t\tcase {0}_SIGNAL_VALUE_CHANGED :
            return &{1};\n""".format(capName, self.name) 

    def generateTests(self, capName) : 
        common.Widget.generateTests(self, capName) 
        self.addmoreTests(capName)

    def generateMandatoryVariables (self) :
        self.generated.initFunction  += "\tinitializeListPropertyWidget(&{0});\n".format(self.name)
        self.setNumLanguages()
        self.setEnabled()
        self.setListPropMethods()
        self.generated.initFunction += "\n"

    def setListPropMethods (self) :
        self.generated.initFunction += "\t{0}.getNumRecords = &{1};\n".format(self.name, self.element.getNumRecords)
        self.generated.initFunction += "\t{0}.getRecordId = &{1};\n".format(self.name, self.element.getRecordId)
        self.generated.initFunction += "\t{0}.getRecordName = &{1};\n".format(self.name, self.element.getRecordName)
        self.generated.initFunction += "\t{0}.viewRecord = &{1};\n".format(self.name, self.element.viewRecord)
        self.generated.initFunction += "\t{0}.updateRecord = &{1};\n".format(self.name, self.element.updateRecord)
        self.generated.initFunction += "\t{0}.deleteRecord = &{1};\n".format(self.name, self.element.deleteRecord)
        self.generated.initFunction += "\t{0}.addRecord = &{1};\n".format(self.name, self.element.addRecord)
        self.generated.initFunction += "\t{0}.confirmRecord = &{1};\n".format(self.name, self.element.confirmRecord)
        self.generated.initFunction += "\t{0}.cancelRecord = &{1};\n".format(self.name, self.element.cancelRecord)
        self.generated.initFunction += "\n"

    def generateChildElements (self) :
        containerChild = self.element.recordContainer
        containerName = containerChild.name
        recordNameChild = self.element.recordName

        propertyW = pw.Property(self.generated, recordNameChild, (self.parentObjectPath + self.objectPathSuffix + "/" + containerName), self.languageSetName)
        propertyW.generate()

        container = cw.Container(self.generated, containerChild, (self.parentObjectPath + self.objectPathSuffix), self.languageSetName)
        container.generate()

    def addmoreTests(self, capName) :
        self.generated.initTests += """    testsToRun[{1}].msgId = {0}_ADD;
    testsToRun[{1}].numParams = 0;\n""".format(capName, self.generated.numTests, self.generated.definesIndx - 1)
        self.generated.numTests =     self.generated.numTests + 1
        self.generated.allReplies += "\t\t\t\tcase AJ_REPLY_ID({0}_ADD):\\\n".format(capName)

        self.generated.initTests += """    testsToRun[{1}].msgId = {0}_CONFIRM;
    testsToRun[{1}].numParams = 0;\n""".format(capName, self.generated.numTests, self.generated.definesIndx - 1)
        self.generated.numTests =     self.generated.numTests + 1
        self.generated.allReplies += "\t\t\t\tcase AJ_REPLY_ID({0}_CONFIRM):\\\n".format(capName)

        self.generated.initTests += """    testsToRun[{1}].msgId = {0}_UPDATE;
    testsToRun[{1}].numParams = 1;
    testsToRun[{1}].param[0] = 1;\n""".format(capName, self.generated.numTests, self.generated.definesIndx - 1)
        self.generated.numTests =     self.generated.numTests + 1
        self.generated.allReplies += "\t\t\t\tcase AJ_REPLY_ID({0}_UPDATE):\\\n".format(capName)

        self.generated.initTests += """    testsToRun[{1}].msgId = {0}_CANCEL;
    testsToRun[{1}].numParams = 0;\n""".format(capName, self.generated.numTests, self.generated.definesIndx - 1)
        self.generated.numTests =     self.generated.numTests + 1
        self.generated.allReplies += "\t\t\t\tcase AJ_REPLY_ID({0}_CANCEL):\\\n".format(capName)

        self.generated.initTests += """    testsToRun[{1}].msgId = {0}_ADD;
    testsToRun[{1}].numParams = 0;\n""".format(capName, self.generated.numTests, self.generated.definesIndx - 1)
        self.generated.numTests =     self.generated.numTests + 1

        self.generated.initTests += """    testsToRun[{1}].msgId = {0}_CONFIRM;
    testsToRun[{1}].numParams = 0;\n""".format(capName, self.generated.numTests, self.generated.definesIndx - 1)
        self.generated.numTests =     self.generated.numTests + 1

        self.generated.initTests += """    testsToRun[{1}].msgId = {0}_DELETE;
    testsToRun[{1}].numParams = 1;
    testsToRun[{1}].param[0] = 2;\n""".format(capName, self.generated.numTests, self.generated.definesIndx - 1)
        self.generated.numTests =     self.generated.numTests + 1
        self.generated.allReplies += "\t\t\t\tcase AJ_REPLY_ID({0}_DELETE):\\\n".format(capName)

        self.generated.initTests += """    testsToRun[{1}].msgId = {0}_VIEW;
    testsToRun[{1}].numParams = 1;
    testsToRun[{1}].param[0] = 1;\n""".format(capName, self.generated.numTests, self.generated.definesIndx - 1)
        self.generated.numTests =     self.generated.numTests + 1
        self.generated.allReplies += "\t\t\t\tcase AJ_REPLY_ID({0}_VIEW):\\\n".format(capName)


