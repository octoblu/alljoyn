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

#ifndef WIDGET_H_
#define WIDGET_H_

#include <alljoyn/BusAttachment.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/controlpanel/LanguageSet.h>
#include <alljoyn/controlpanel/ControlPanelEnums.h>

namespace ajn {
namespace services {

/*
 * Forward Declaration
 */
class WidgetBusObject;
class ControlPanelDevice;

/**
 * Base Class for all Widgets
 */
class Widget {
  public:

    /**
     * Constructor for Widget class
     * @param name - name of Widget
     * @param rootWidget - the RootWidget of the widget
     * @param widgetType - the type of the widget
     */
    Widget(qcc::String const& name, Widget* rootWidget, WidgetType widgetType);

    /**
     * Constructor for Widget class
     * @param name - name of Widget
     * @param rootWidget - the RootWidget of the widget
     * @param device - the device containing this widget
     * @param widgetType - the type of the widget
     */
    Widget(qcc::String const& name, Widget* rootWidget, ControlPanelDevice* device, WidgetType widgetType);

    /**
     * Destructor for Widget
     */
    virtual ~Widget() = 0;

    /**
     * Pure virtual function. Needs to be implemented by a function that will
     * create and return the appropriate BusObject for the Widget
     * @param bus - the bus used to create the widget
     * @param objectPath - the objectPath of the widget
     * @param langIndx - the language Indx
     * @param status - the status indicating success or failure
     * @return a newly created WidgetBusObject
     */
    virtual WidgetBusObject* createWidgetBusObject(BusAttachment* bus, qcc::String const& objectPath,
                                                   uint16_t langIndx, QStatus& status) = 0;

    /**
     * Get the widgetType of the Widget
     * @return widgetType
     */
    WidgetType getWidgetType() const;

    /**
     * Get the name of the Widget
     * @return widgetName
     */
    qcc::String const& getWidgetName() const;

    /**
     * Get the mode of the Widget
     * @return controlPanelMode
     */
    ControlPanelMode getControlPanelMode() const;

    /**
     * Get this widget's RootWidget
     * @return rootWidget
     */
    Widget* getRootWidget() const;

    /**
     * Get the Device of the widget
     * @return device
     */
    ControlPanelDevice* getDevice() const;

    /**
     * Get the Interface Version of the Widget
     * @return interface Version
     */
    const uint16_t getInterfaceVersion() const;

    /**
     * Set the isSecured boolean
     * @param secured
     */
    void setIsSecured(bool secured);

    /**
     * Get the isSecured boolean
     * @return true/false
     */
    bool getIsSecured() const;

    /**
     * Get IsEnabled boolean
     * @return true/false
     */
    bool getIsEnabled() const;

    /**
     * Get IsWritable boolean
     * @return true/false
     */
    bool getIsWritable() const;

    /**
     * Set the Enabled flag of the Widget
     * @param enabled
     */
    void setEnabled(bool enabled);

    /**
     * Get the GetEnabled function Pointer
     * @return getEnabled function Pointer of the widget
     */
    GetBoolFptr getGetEnabled() const;

    /**
     * Set the GetEnabled function Pointer
     * @param getEnabled - function pointer to get the enabled boolean
     */
    void setGetEnabled(GetBoolFptr getEnabled);

    /**
     * Set the Writable flag of the widget
     * @param writable
     */
    void setWritable(bool writable);

    /**
     * Get the GetWritable function Pointer
     * @return getWritable function Pointer of the widget
     */
    GetBoolFptr getGetWritable() const;

    /**
     * Set the GetWritable function Pointer
     * @param getWritable - function pointer to get the writable boolean
     */
    void setGetWritable(GetBoolFptr getWritable);

    /**
     * Get the States of the Widget
     * @return states
     */
    uint32_t getStates() const;

    /**
     * Set the States of the widget
     * @param enabled - is widget enabled
     * @param writable -  is widget writable
     */
    void setStates(uint8_t enabled, uint8_t writable);

    /**
     * Get the BgColor of the Widget
     * @return bgColor
     */
    uint32_t getBgColor() const;

    /**
     * Set the BgColor of the Widget
     * @param bgColor
     */
    void setBgColor(uint32_t bgColor);

    /**
     * Get the GetBgColor function pointer of the Widget
     * @return GetBgColor function pointer
     */
    GetUint32Fptr getGetBgColor() const;

    /**
     * Set the GetBgColor function pointer of the Widget
     * @param getBgColor
     */
    void setGetBgColor(GetUint32Fptr getBgColor);

    /**
     * Get the Label of the Widget
     * @return label
     */
    virtual const qcc::String& getLabel() const;

    /**
     * Get the Labels vector of the widget
     * @return - label
     */
    virtual const std::vector<qcc::String>& getLabels() const;

    /**
     * Set the labels vector of the widget
     * @param labels - vector of labels
     */
    virtual void setLabels(const std::vector<qcc::String>& labels);

    /**
     * Get the GetLabel function pointer
     * @return GetLabel function pointer
     */
    virtual GetStringFptr getGetLabels() const;

    /**
     * Set the GetLabel function pointer
     * @param getLabels - getLabel function pointer
     */
    virtual void setGetLabels(GetStringFptr getLabels);

    /**
     * Get the vector of Hints for the Widget
     * @return hints Vector
     */
    const std::vector<uint16_t>& getHints() const;

    /**
     * Set the vector of Hints for the Widget
     * @param hints - hints vector
     */
    void setHints(const std::vector<uint16_t>& hints);

    /**
     * Register the BusObjects for this Widget
     * @param bus - the bus used to register the busObject
     * @param objectPath - the objectPath of the busObject
     * @return
     */
    virtual QStatus registerObjects(BusAttachment* bus, qcc::String const& objectPath);

    /**
     * Register the BusObjects for this Widget
     * @param bus - the bus used to register the busObjects
     * @param languageSet -  the languageSet to register the busObjects for
     * @param objectPathPrefix - the objectPathPrefix of the busObject
     * @param objectPathSuffix - the objectPathSuffix of the busObject
     * @param isRoot - is this a rootWidget
     * @return status - success/failure
     */
    virtual QStatus registerObjects(BusAttachment* bus, LanguageSet const& languageSet,
                                    qcc::String const& objectPathPrefix, qcc::String const& objectPathSuffix, bool isRoot = false);

    /**
     * Refresh the Widget
     * @param bus - bus used for refreshing the object
     * @return status - success/failure
     */
    QStatus refreshObjects(BusAttachment* bus);

    /**
     * Unregister the BusObjects for this widget
     * @param bus
     * @return status - success/failure
     */
    virtual QStatus unregisterObjects(BusAttachment* bus);

    /**
     * Fill MsgArg passed in with the States
     * @param val - msgArg to fill
     * @param languageIndx - language requested
     * @return status - success/failure
     */
    QStatus fillStatesArg(MsgArg& val, uint16_t languageIndx);

    /**
     * Fill MsgArg passed in with the OptParams
     * @param val - msgArg to fill
     * @param languageIndx - language requested
     * @return status - success/failure
     */
    virtual QStatus fillOptParamsArg(MsgArg& val, uint16_t languageIndx);

    /**
     * Read the MsgArg passed in and fill the Version property
     * @param val - MsgArg passed in
     * @return status - success/failure
     */
    virtual QStatus readVersionArg(MsgArg* val);

    /**
     * Read the MsgArg passed in and fill the States property
     * @param val - MsgArg passed in
     * @return status - success/failure
     */
    virtual QStatus readStatesArg(MsgArg* val);

    /**
     * Read the MsgArg passed in and fill the OptParams
     * @param val - MsgArg passed in
     * @return status - success/failure
     */
    virtual QStatus readOptParamsArg(MsgArg* val);

    /**
     * Set a Property Changed Signal
     * @return status - success/failure
     */
    QStatus SendPropertyChangedSignal();

    /**
     *  Callback when PropertyChanged signal is received
     */
    void PropertyChanged();

  protected:

    /**
     * The Name of the Widget
     */
    qcc::String m_Name;

    /**
     * The WidgetType of the Widget
     */
    WidgetType m_WidgetType;

    /**
     * The RootWidget of this Widget
     */
    Widget* m_RootWidget;

    /**
     * Is the Widget Secured
     */
    bool m_IsSecured;

    /**
     * Version of the Widget
     */
    uint16_t m_Version;

    /**
     * States of the Widget
     */
    uint32_t m_States;

    /**
     * GetEnabled function Pointer
     */
    GetBoolFptr m_GetEnabled;

    /**
     * Get Writable Function Pointer
     */
    GetBoolFptr m_GetWritable;

    /**
     * BgColor of the Widget
     */
    uint32_t m_BgColor;

    /**
     * GetBgColor function Pointer of the Widget
     */
    GetUint32Fptr m_GetBgColor;

    /**
     * Label
     */
    qcc::String m_Label;

    /**
     * Vector of Labels
     */
    std::vector<qcc::String> m_Labels;

    /**
     * GetLabel functionPointer
     */
    GetStringFptr m_GetLabels;

    /**
     * Vector of Hints
     */
    std::vector<uint16_t> m_Hints;

    /**
     * The BusObjects of the Widget
     */
    std::vector<WidgetBusObject*> m_BusObjects;

    /**
     * Mode widget is in
     */
    ControlPanelMode m_ControlPanelMode;

    /**
     * The Device that contains this widget
     */
    ControlPanelDevice* m_Device;

    /**
     * Function to fill the OptParams of the Widget Base class
     * @param optParams - msgArg Array to fill
     * @param languageIndx - the language requested
     * @param optParamIndx - the index of the optParams array to start from
     * @return status - success/failure
     */
    QStatus fillOptParamsArg(MsgArg* optParams, uint16_t languageIndx, size_t& optParamIndx);

    /**
     * Read the MsgArg passed in and fill the OptParam
     * @param key - the optParam to fill
     * @param val - the MsgArg passed in
     * @return status - success/failure
     */
    virtual QStatus readOptParamsArg(uint16_t key, MsgArg* val);

    /**
     * Check Versions of Remote Widget
     * @return status - success/failure
     */
    virtual QStatus checkVersions();

    /**
     * Add Children for Widget
     * @param bus - bus to use
     * @return status - success/failure
     */
    virtual QStatus addChildren(BusAttachment* bus);

    /**
     * Refresh the Children of this Widget
     * @param bus - bus to use
     * @return status - success/failure
     */
    virtual QStatus refreshChildren(BusAttachment* bus);

    /**
     * FillProperties for Widget
     * @return status - success/failure
     */
    virtual QStatus fillProperties();

  private:

    /**
     * Copy constructor of widget - private. widget is not copy-able
     * @param widget - widget to copy
     */
    Widget(const Widget& widget);

    /**
     * Assignment operator of widget - private. widget is not assignable
     * @param widget
     * @return
     */
    Widget& operator=(const Widget& widget);
};
} //namespace services
} //namespace ajn

#endif /* WIDGET_H_ */
