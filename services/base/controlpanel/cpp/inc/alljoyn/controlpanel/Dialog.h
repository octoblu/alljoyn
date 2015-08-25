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

#ifndef DIALOG_H_
#define DIALOG_H_

#include <alljoyn/controlpanel/RootWidget.h>

namespace ajn {
namespace services {

/**
 * Action Class. Used to display a Dialog.
 * This is an abstract class. The executeActionCallBack functions need to be overridden.
 */
class Dialog : public RootWidget {
  public:

    /**
     * Constructor for Dialog Widget
     * @param name - name of Widget
     * @param rootWidget - the RootWidget of the widget
     */
    Dialog(qcc::String const& name, Widget* rootWidget);

    /**
     * Constructor for Dialog Widget
     * @param name - name of Widget
     * @param rootWidget - the RootWidget of the widget
     * @param objectPath - objectPath of Widget
     * @param device - device that contains this Widget
     */
    Dialog(qcc::String const& name, Widget* rootWidget, qcc::String const& objectPath, ControlPanelDevice* device);

    /**
     * Constructor for Dialog Widget
     * @param name - name of Widget
     * @param rootWidget - the RootWidget of the widget
     * @param device - device that contains this Widget
     */
    Dialog(qcc::String const& name, Widget* rootWidget, ControlPanelDevice* device);

    /**
     * Destructor for Dialog Widget
     */
    virtual ~Dialog();

    /**
     * creates and returns the appropriate BusObject for this Widget
     * @param bus - the bus used to create the widget
     * @param objectPath - the objectPath of the widget
     * @param langIndx - the language Indx
     * @param status - the status indicating success or failure
     * @return a newly created WidgetBusObject
     */
    WidgetBusObject* createWidgetBusObject(BusAttachment* bus, qcc::String const& objectPath,
                                           uint16_t langIndx, QStatus& status);

    /**
     * Get the Number of Actions in the Dialog
     * @return Number of Actions
     */
    const uint16_t getNumActions() const;

    /**
     * Set the number of Actions in the Dialog
     * @param numActions - number of Actions
     */
    void setNumActions(const uint16_t numActions);

    /**
     * Get the Message of the Dialog
     * @return the message
     */
    const qcc::String& getMessage() const;

    /**
     * Get the Message Vector of the Dialog
     * @return the Messages
     */
    const std::vector<qcc::String>& getMessages() const;

    /**
     * Set the Message vector of the Dialog
     * @param messages - the messages
     */
    void setMessages(const std::vector<qcc::String>& messages);

    /**
     * Get the GetMessage Function Pointer for the Dialog
     * @return GetMessage function pointer
     */
    GetStringFptr getGetMessages() const;

    /**
     * Set the GetMessage Function Pointer for the Dialog
     * @param getMessages - the GetMessage function pointer
     */
    void setGetMessages(GetStringFptr getMessages);

    /**
     * Get the LabelAction1 of the Dialog
     * @return the message
     */
    const qcc::String& getLabelAction1() const;

    /**
     * Get the LabelAction2 of the Dialog
     * @return the message
     */
    const qcc::String& getLabelAction2() const;

    /**
     * Get the LabelAction3 of the Dialog
     * @return the message
     */
    const qcc::String& getLabelAction3() const;

    /**
     * Get the Vector of Labels for Action 1
     * @return label for Action 1
     */
    const std::vector<qcc::String>& getLabelsAction1() const;

    /**
     * Set the Vector of Labels for Action 1
     * @param labelsAction1
     */
    void setLabelsAction1(const std::vector<qcc::String>& labelsAction1);

    /**
     * Get the GetLabel Function Pointer for Action 1
     * @return GetLabel Function Pointer for Action 1
     */
    GetStringFptr getGetLabelsAction1() const;

    /**
     * Set the GetLabel function Pointer for Action 1
     * @param getLabelsAction1
     */
    void setGetLabelsAction1(GetStringFptr getLabelsAction1);

    /**
     * Get the Vector of Labels for Action 2
     * @return label for Action 3
     */
    const std::vector<qcc::String>& getLabelsAction2() const;

    /**
     * Set the Vector of Labels for Action 2
     * @param labelsAction2
     */
    void setLabelsAction2(const std::vector<qcc::String>& labelsAction2);

    /**
     * Set the GetLabel function Pointer for Action 2
     * @return getLabelAction2
     */
    GetStringFptr getGetLabelsAction2() const;

    /**
     * Set the GetLabel function Pointer for Action 2
     * @param getLabelsAction2
     */
    void setGetLabelsAction2(GetStringFptr getLabelsAction2);

    /**
     * Get the Vector of Labels for Action 3
     * @return label for Action 3
     */
    const std::vector<qcc::String>& getLabelsAction3() const;

    /**
     * Set the Vector of Labels for Action 3
     * @param labelsAction3
     */
    void setLabelsAction3(const std::vector<qcc::String>& labelsAction3);

    /**
     * Set the GetLabel function Pointer for Action 3
     * @return getLabelAction3
     */
    GetStringFptr getGetLabelsAction3() const;

    /**
     * Set the GetLabel function Pointer for Action 3
     * @param getLabelsAction3
     */
    void setGetLabelsAction3(GetStringFptr getLabelsAction3);

    /**
     * Fill the MsgArg passed in with the OptionalParams
     * @param val - msgArg to fill
     * @param languageIndx - the languageIndx used for the Labels
     * @return status - success/failure
     */
    QStatus fillOptParamsArg(MsgArg& val, uint16_t languageIndx);

    /**
     * Fill the MsgArg passed in with the Message Param
     * @param val - msgArg to fill
     * @param languageIndx - the languageIndx used for the Message
     * @return status - success/failure
     */
    QStatus fillMessageArg(MsgArg& val, uint16_t languageIndx);

    /**
     * Fill the MsgArg passed in with the NumActions
     * @param val - msgArg to fill
     * @param languageIndx - the languageIndx
     * @return status - success/failure
     */
    QStatus fillNumActionArg(MsgArg& val, uint16_t languageIndx);

    /**
     * Read the MsgArg passed in to fill the Message property
     * @param val - MsgArg passed in
     * @return status - success/failure
     */
    QStatus readMessageArg(MsgArg* val);

    /**
     * Read the MsgArg passed in to fill the NumActions property
     * @param val - MsgArg passed in
     * @return status - success/failure
     */
    QStatus readNumActionsArg(MsgArg* val);

    /**
     * Call to execute this Dialog's Action 1 remotely
     * @return status - success/failure
     */
    QStatus executeAction1();

    /**
     * Call to execute this Dialog's Action 2 remotely
     * @return status - success/failure
     */
    QStatus executeAction2();

    /**
     * Call to execute this Dialog's Action 3 remotely
     * @return status - success/failure
     */
    QStatus executeAction3();

    /**
     * Pure Virtual Method to be overridden
     * Callback for execute of Action1
     * @return success/failure
     */
    virtual bool executeAction1CallBack();

    /**
     * Pure Virtual Method to be overridden
     * Callback for execute of Action2
     * @return success/failure
     */
    virtual bool executeAction2CallBack();

    /**
     * Pure Virtual Method to be overridden
     * Callback for execute of Action3
     * @return success/failure
     */
    virtual bool executeAction3CallBack();

  protected:

    /**
     * Number of Actions in Dialog
     */
    uint16_t m_NumActions;

    /**
     * The message of the dialog
     */
    qcc::String m_Message;

    /**
     * The message vector of the dialog
     */
    std::vector<qcc::String> m_Messages;

    /**
     * The GetMessage Function Pointer of the Dialog
     */
    GetStringFptr m_GetMessages;

    /**
     * The Label of the dialog for Action 1
     */
    qcc::String m_LabelAction1;

    /**
     * The Label vector for Action1
     */
    std::vector<qcc::String> m_LabelsAction1;

    /**
     * The GetLabel function pointer for Action 1
     */
    GetStringFptr m_GetLabelsAction1;

    /**
     * The Label of the dialog for Action 2
     */
    qcc::String m_LabelAction2;

    /**
     * The Label vector for Action2
     */
    std::vector<qcc::String> m_LabelsAction2;

    /**
     * The GetLabel function pointer for Action 2
     */
    GetStringFptr m_GetLabelsAction2;

    /**
     * The Label of the dialog for Action 3
     */
    qcc::String m_LabelAction3;

    /**
     * The Label vector for Action3
     */
    std::vector<qcc::String> m_LabelsAction3;

    /**
     * The GetLabel function pointer for Action 3
     */
    GetStringFptr m_GetLabelsAction3;

    /**
     * Read the MsgArg passed in and fill the OptParam
     * @param key - the optParam to fill
     * @param val - the MsgArg passed in
     * @return status - success/failure
     */
    virtual QStatus readOptParamsArg(uint16_t key, MsgArg* val);

    /**
     * Private method used to log a call to an Action that is Not Defined
     * @return false - for failure
     */
    bool executeActionNotDefined();
};
} //namespace services
} //namespace ajn

#endif /* DIALOG_H_ */
