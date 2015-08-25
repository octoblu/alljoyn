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

class Dialog (common.Widget):

    def __init__(self, generated, dialogElement, parentName, languageSetName, isRoot = 0) :
        common.Widget.__init__(self, generated, dialogElement, parentName, languageSetName)
        self.isRoot = 1
        self.widgetName = self.name[:1].upper() + self.name[1:]
        if not isRoot :
            self.parentAddFunc = "addChildDialog"
        else :
            self.parentAddFunc = "setRootWidget"

    def generate(self) :
        common.Widget.generate(self)
        self.generateOnAction()

    def generateMandatoryVariables (self) :
        common.Widget.generateMandatoryVariables(self)
        self.setMessage()
        self.setNumActions() 

    def setMessage (self) :
        self.setCodeOrValueString ("message", "setMessages", "setGetMessages")

    def setNumActions (self) :
        self.generated.initCode += "    {0}->setNumActions({1});\n".format(self.name, len(self.element.button))

    def setLabelAction (self, i) :
        self.setCodeOrValueString ('label', "setLabelsAction%i" % (i+1), "setGetLabelsAction%i" % (i+1), "LabelAction%i" % (i+1), self.element.button[i])


    def generateOnAction (self) :
        dialogHeaderFile = self.generated.dialogHeaderFile
        dialogSrcFile = self.generated.dialogSrcFile

        regularName = self.widgetName
        capitalName = self.name.upper()

        dialogHeaderFile = dialogHeaderFile.replace("CAPITAL_NAME_HERE", capitalName)
        dialogHeaderFile = dialogHeaderFile.replace("REGULAR_NAME_HERE", regularName)

        dialogSrcFile = dialogSrcFile.replace("CAPITAL_NAME_HERE", capitalName)
        dialogSrcFile = dialogSrcFile.replace("REGULAR_NAME_HERE", regularName)
        dialogSrcFile = dialogSrcFile.replace("ADDITIONAL_INCLUDES_HERE", self.generated.srcIncludes)

        self.generated.headerIncludes += self.generateHeaderInclude()

        notDefined = "return executeActionNotDefined();"
        for i in range(0, len(self.element.button)):
            self.setLabelAction(i)
            dialogSrcFile = dialogSrcFile.replace("CODE_OF_EXECUTE_ACTION%i_HERE" % (i+1), self.element.button[i].executeCode)            
        for i in range(len(self.element.button), 3):    
            dialogSrcFile = dialogSrcFile.replace("CODE_OF_EXECUTE_ACTION%i_HERE" % (i+1), notDefined)            

        genH = open(self.generated.path + "/" + regularName + ".h", 'w')
        genH.write(dialogHeaderFile)
        genH.close()
        genC = open(self.generated.path + "/" + regularName + ".cc", 'w')
        genC.write(dialogSrcFile)
        genC.close()

