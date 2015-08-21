#include "nan.h"

#include "NotificationWrapper.h"
#include "InterfaceWrapper.h"
#include "util.h"
#include "BusConnection.h"
#include <string.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

Persistent<v8::Function> NotificationWrapper::constructor;

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

void NotificationWrapper::Init(v8::Handle<v8::Object> target) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(NotificationWrapper::New);
  tpl->SetClassName(Nan::New<v8::String>("NotificationService").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  SetPrototypeMethod(tpl, "notify", NotificationWrapper::Notify);
  SetPrototypeMethod(tpl, "deleteLastMsg", NotificationWrapper::DeleteLastMsg);

  constructor.Reset(tpl->GetFunction());
  Set(target, Nan::New("NotificationService").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(NotificationWrapper::New) {
  
  if(info.Length() < 1 || !info[0]->IsString()){
    return Nan::ThrowError("Notification requires an application name.");
  }
  char* name = strdup(*Utf8String(info[0]));
  BusConnection* busWrapper = node::ObjectWrap::Unwrap<BusConnection>(info[1].As<v8::Object>());
  NotificationWrapper* obj = new NotificationWrapper(name, busWrapper->bus, info[2]->Int32Value());
  obj->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(NotificationWrapper::Notify) {
  
  if(info.Length() < 2){
    return Nan::ThrowError("Notification.Notify requires a message and TTL (in seconds).");
  }
  NotificationWrapper* obj = node::ObjectWrap::Unwrap<NotificationWrapper>(info.This());
  ajn::services::NotificationText textToSend("en", strdup(*Utf8String(info[0])));
  std::vector<ajn::services::NotificationText> msg;
  msg.push_back(textToSend);
  ajn::services::Notification notification(ajn::services::INFO, msg);
  QStatus status = obj->notificationSender->send(notification, info[1]->Int32Value());
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

NAN_METHOD(NotificationWrapper::DeleteLastMsg) {
  
  NotificationWrapper* obj = node::ObjectWrap::Unwrap<NotificationWrapper>(info.This());
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



