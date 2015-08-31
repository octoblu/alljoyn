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
import dialogWidget as dw

class Action (common.Widget):

    def __init__(self, generated, actionElement, parentObjectPath, languageSetName) :
        common.Widget.__init__(self, generated, actionElement, parentObjectPath, languageSetName)
        if hasattr(self.element.onAction, "executeCode") : 
            self.widgetName = self.name[:1].upper() + self.name [1:]
        else :
            self.widgetName = "ActionWithDialog"

    def generate(self) :
        common.Widget.generate(self)
        self.generateOnAction()

    def generateOnAction (self) :
        onAction = self.element.onAction
        if hasattr(onAction, "executeCode") : 
            actionHeaderFile = self.generated.actionHeaderFile
            actionSrcFile = self.generated.actionSrcFile

            regularName = self.widgetName
            capitalName = self.name.upper()

            actionHeaderFile = actionHeaderFile.replace("CAPITAL_NAME_HERE", capitalName)
            actionHeaderFile = actionHeaderFile.replace("REGULAR_NAME_HERE", regularName)

            actionSrcFile = actionSrcFile.replace("CAPITAL_NAME_HERE", capitalName)
            actionSrcFile = actionSrcFile.replace("REGULAR_NAME_HERE", regularName)
            actionSrcFile = actionSrcFile.replace("ADDITIONAL_INCLUDES_HERE", self.generated.srcIncludes)
            actionSrcFile = actionSrcFile.replace("EXECUTE_ACTION_HERE", onAction.executeCode)

            self.generated.headerIncludes += self.generateHeaderInclude()

            genH = open(self.generated.path + "/" + regularName + ".h", 'w')
            genH.write(actionHeaderFile)
            genH.close()
            genC = open(self.generated.path + "/" + regularName + ".cc", 'w')
            genC.write(actionSrcFile)
            genC.close()

        elif hasattr(onAction, "dialog") : 
            dialog = dw.Dialog(self.generated, onAction.dialog, self.name, self.languageSetName)
            dialog.generate()

