#include "nan.h"

#include "NotificationWrapper.h"
#include "InterfaceWrapper.h"
#include "util.h"
#include "BusConnection.h"
#include <string.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

static v8::Persistent<v8::FunctionTemplate> notification_constructor;

NAN_METHOD(NotificationConstructor) {
  NanScope();
  if(args.Length() == 0 || !args[0]->IsString()){
    return NanThrowError("Notification requires an application name, BusAttachment, and port number.");
  }

  v8::Local<v8::Object> obj;
  v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(notification_constructor);

  v8::Handle<v8::Value> argv[] = {
    args[0],
    args[1],
    args[2]
  };
  obj = con->GetFunction()->NewInstance(3, argv);
  NanReturnValue(obj);
}

NotificationWrapper::NotificationWrapper(const char* appName, ajn::BusAttachment* bus, int port):propertyStore(new ajn::services::AboutPropertyStoreImpl()){
  propertyStore->setDeviceId("FEDCBA0987654321");
  propertyStore->setAppId("0123456789ABCDEF");
  propertyStore->setAppName(appName);
  std::vector<qcc::String> languages(1);
  languages[0] = "en";
  propertyStore->setSupportedLangs(languages);
  propertyStore->setDefaultLang("en");
  propertyStore->setDeviceName("node-alljoyn", "en");

  ajn::services::AboutServiceApi::Init(*bus, *propertyStore);
  aboutService = ajn::services::AboutServiceApi::getInstance();

  NotificationBusListener* busListener = new NotificationBusListener();
  busListener->setSessionPort(port);
  bus->RegisterBusListener(*busListener);

  ajn::SessionPort sp = port;
  ajn::SessionOpts opts(ajn::SessionOpts::TRAFFIC_MESSAGES, false, ajn::SessionOpts::PROXIMITY_ANY, ajn::TRANSPORT_ANY);
  bus->BindSessionPort(sp, opts, *busListener);

  aboutService->Register(port);
  bus->RegisterBusObject(*aboutService);

  aboutService->Announce();

  notificationService = ajn::services::NotificationService::getInstance();
  notificationSender = notificationService->initSend(bus, propertyStore);
}

void NotificationWrapper::Init () {
  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(NotificationWrapper::New);
  NanAssignPersistent(notification_constructor, tpl);
  tpl->SetClassName(NanNew<v8::String>("NotificationService"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(tpl, "notify", NotificationWrapper::Notify);
  NODE_SET_PROTOTYPE_METHOD(tpl, "deleteLastMsg", NotificationWrapper::DeleteLastMsg);
}

NAN_METHOD(NotificationWrapper::New) {
  NanScope();
  if(args.Length() < 1 || !args[0]->IsString()){
    return NanThrowError("Notification requires an application name.");
  }
  char* name = strdup(*NanUtf8String(args[0]));
  BusConnection* busWrapper = node::ObjectWrap::Unwrap<BusConnection>(args[1].As<v8::Object>());
  NotificationWrapper* obj = new NotificationWrapper(name, busWrapper->bus, args[2]->Int32Value());
  obj->Wrap(args.This());

  NanReturnValue(args.This());
}

NAN_METHOD(NotificationWrapper::Notify) {
  NanScope();
  if(args.Length() < 2){
    return NanThrowError("Notification.Notify requires a message and TTL (in seconds).");
  }
  NotificationWrapper* obj = node::ObjectWrap::Unwrap<NotificationWrapper>(args.This());
  ajn::services::NotificationText textToSend("en", strdup(*NanUtf8String(args[0])));
  std::vector<ajn::services::NotificationText> msg;
  msg.push_back(textToSend);
  ajn::services::Notification notification(ajn::services::INFO, msg);
  QStatus status = obj->notificationSender->send(notification, args[1]->Int32Value());
  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(NotificationWrapper::DeleteLastMsg) {
  NanScope();
  NotificationWrapper* obj = node::ObjectWrap::Unwrap<NotificationWrapper>(args.This());
  QStatus status = obj->notificationSender->deleteLastMsg(ajn::services::INFO);
  NanReturnValue(NanNew<v8::Integer>(static_cast<int>(status)));
}

//// NotificationBusListener

NotificationBusListener::NotificationBusListener(){}

void NotificationBusListener::setSessionPort(ajn::SessionPort sessionPort){
  this->sessionPort = sessionPort;
}

ajn::SessionPort NotificationBusListener::getSessionPort(){
  return sessionPort;
}

bool NotificationBusListener::AcceptSessionJoiner(ajn::SessionPort port, const char* joiner, const ajn::SessionOpts& opts){
  return true;
}



