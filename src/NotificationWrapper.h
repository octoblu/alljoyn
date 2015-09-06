#ifndef LD_NOTIFICATIONWRAPPER_H
#define LD_NOTIFICATIONWRAPPER_H

#include <nan.h>
#include <BusListener.h>
#include <SessionPortListener.h>
#include <alljoyn/notification/NotificationService.h>
#include <alljoyn/notification/NotificationSender.h>
#include <alljoyn/notification/NotificationText.h>
#include <alljoyn/notification/NotificationEnums.h>
#include <alljoyn/about/AboutServiceApi.h>
#include <alljoyn/notification/Notification.h>
#include <alljoyn/about/AboutPropertyStoreImpl.h>
#include <alljoyn/AllJoynStd.h>

NAN_METHOD(NotificationConstructor);

class NotificationBusListener : public ajn::BusListener, public ajn::SessionPortListener {

  public:
    NotificationBusListener();
    bool AcceptSessionJoiner(ajn::SessionPort sessionPort, const char* joiner, const ajn::SessionOpts& opts);
    void setSessionPort(ajn::SessionPort sessionPort);
    ajn::SessionPort getSessionPort();
  private:
    ajn::SessionPort sessionPort;
};

class NotificationWrapper : public Nan::ObjectWrap {
  private:

    static NAN_METHOD(New);
    static NAN_METHOD(Notify);
    static NAN_METHOD(DeleteLastMsg);
  public:
    NotificationWrapper(const char* appName, ajn::BusAttachment* bus, int port);
    static void Init ();
    ajn::services::AboutPropertyStoreImpl* propertyStore;
    ajn::services::AboutServiceApi* aboutService;
    ajn::services::NotificationService* notificationService;
    ajn::services::NotificationSender* notificationSender;
};


#endif
