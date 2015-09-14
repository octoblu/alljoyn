#include "nan.h"

#include "NotificationWrapper.h"
#include "InterfaceWrapper.h"
#include "util.h"
#include "BusConnection.h"
#include <string.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

static Nan::Persistent<v8::FunctionTemplate> notification_constructor;

NAN_METHOD(NotificationConstructor) {
  if(info.Length() == 0 || !info[0]->IsString()){
    return Nan::ThrowError("Notification requires an application name, BusAttachment, and port number.");
  }

  v8::Local<v8::Object> obj;
  v8::Local<v8::FunctionTemplate> con = Nan::New<v8::FunctionTemplate>(notification_constructor);

  v8::Handle<v8::Value> argv[] = {
    info[0],
    info[1],
    info[2]
  };
  obj = con->GetFunction()->NewInstance(3, argv);
  info.GetReturnValue().Set(obj);
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
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(NotificationWrapper::New);
  notification_constructor.Reset(tpl);
  tpl->SetClassName(Nan::New<v8::String>("NotificationService").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  Nan::SetPrototypeMethod(tpl, "notify", NotificationWrapper::Notify);
  Nan::SetPrototypeMethod(tpl, "deleteLastMsg", NotificationWrapper::DeleteLastMsg);
}

NAN_METHOD(NotificationWrapper::New) {
  if(info.Length() < 1 || !info[0]->IsString()){
    return Nan::ThrowError("Notification requires an application name.");
  }
  char* name = strdup(*Nan::Utf8String(info[0]));
  BusConnection* busWrapper = Nan::ObjectWrap::Unwrap<BusConnection>(info[1].As<v8::Object>());
  NotificationWrapper* obj = new NotificationWrapper(name, busWrapper->bus, info[2]->Int32Value());
  obj->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(NotificationWrapper::Notify) {
  if(info.Length() < 2){
    return Nan::ThrowError("Notification.Notify requires a message and TTL (in seconds).");
  }
  NotificationWrapper* obj = Nan::ObjectWrap::Unwrap<NotificationWrapper>(info.This());
  ajn::services::NotificationText textToSend("en", strdup(*Nan::Utf8String(info[0])));
  std::vector<ajn::services::NotificationText> msg;
  msg.push_back(textToSend);
  ajn::services::Notification notification(ajn::services::INFO, msg);
  QStatus status = obj->notificationSender->send(notification, info[1]->Int32Value());
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(NotificationWrapper::DeleteLastMsg) {
  NotificationWrapper* obj = Nan::ObjectWrap::Unwrap<NotificationWrapper>(info.This());
  QStatus status = obj->notificationSender->deleteLastMsg(ajn::services::INFO);
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
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



