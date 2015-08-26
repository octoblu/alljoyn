#include "nan.h"

#include "BusObjectWrapper.h"
#include "InterfaceWrapper.h"
#include "util.h"
#include "UVThreadSwitcher.h"
#include <string.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

using namespace v8;
using namespace std;
using namespace ajn;

Nan::Persistent<v8::Function> BusObjectWrapper::constructor;

BusObjectWrapper::BusObjectWrapper(const char* path):object(new BusObjectImpl(path)){}

void BusObjectWrapper::Init (v8::Handle<v8::Object> target) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(BusObjectWrapper::New);
  tpl->SetClassName(Nan::New<v8::String>("BusObject").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  SetPrototypeMethod(tpl, "addInterface", BusObjectWrapper::AddInterfaceInternal);
  SetPrototypeMethod(tpl, "signal", BusObjectWrapper::Signal);

  constructor.Reset(tpl->GetFunction());
  Set(target, Nan::New("BusObject").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(BusObjectWrapper::New) {
  
  if(info.Length() < 1 || !info[0]->IsString()){
    return Nan::ThrowError("BusObject requires a path string.");
  }

  if (info.IsConstructCall()) {
    char* path = strdup(*Utf8String(info[0]));
    BusObjectWrapper* obj = new BusObjectWrapper(path);
    obj->Wrap(info.This());

    info.GetReturnValue().Set(info.This());

  } else {
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = {info[0]};
    v8::Local<v8::Function> cons = Nan::New(constructor);
    info.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }  
}

NAN_METHOD(BusObjectWrapper::AddInterfaceInternal) {
  if(info.Length() < 1){
    return Nan::ThrowError("BusObject.AddInterface requires an Interface.");
  }
  BusObjectWrapper* obj = node::ObjectWrap::Unwrap<BusObjectWrapper>(info.This());
  InterfaceWrapper* interWrapper = node::ObjectWrap::Unwrap<InterfaceWrapper>(info[0].As<v8::Object>());
  QStatus status = obj->object->AddInterface(interWrapper->interface);
  if(info.Length()==1){
  }else{
    if(status==ER_OK){
      Local<Object> v8CallbackObject = info[1].As<Object>();

      status = obj->object->AddMethodHandlers(interWrapper->interface, v8CallbackObject);      
    }
  }
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));  
}

NAN_METHOD(BusObjectWrapper::Signal) {
  
  if(info.Length() < 3){
    return Nan::ThrowError("BusObject.Signal requires a (nullable) destination, SessionId, Interface, member name, and message args.");
  }
  BusObjectWrapper* obj = node::ObjectWrap::Unwrap<BusObjectWrapper>(info.This());
  InterfaceWrapper* interface = node::ObjectWrap::Unwrap<InterfaceWrapper>(info[2].As<v8::Object>());
  const ajn::InterfaceDescription::Member* signalMember = interface->interface->GetMember(*Utf8String(info[3]));
  const char* destination = NULL;
  if(!info[0]->IsNull() && info[0]->IsString()){
    destination = strdup(*Utf8String(info[0]));
  }
  ajn::MsgArg* msgArgs = objToMsgArg(info[4]);
  QStatus status = ER_OK;
  if(info.Length() == 4){
    status = obj->object->Signal(destination, info[1]->Int32Value(), *signalMember, NULL, 0, 0, 0);
  }else if(info.Length() == 5){
    status = obj->object->Signal(destination, info[1]->Int32Value(), *signalMember, msgArgs, 1, 0, 0);
  }else if(info.Length() > 5){
    //TODO handle multi-arg messages
    status = obj->object->Signal(destination, info[1]->Int32Value(), *signalMember, msgArgs, 1, 0, 0);
  }else{
    return Nan::ThrowError("BusObject.Signal requires a SessionId, Interface, member name, and (optionally) destination, message args.");
  }
  info.GetReturnValue().Set(Nan::New<v8::Integer>(static_cast<int>(status)));
}

BusObjectImpl::BusObjectImpl(const char* path):ajn::BusObject(path) {
  switcher = new UVThreadSwitcher(std::bind(&BusObjectImpl::v8MethodHandler, this, std::placeholders::_1));
}
BusObjectImpl::~BusObjectImpl() {
  delete switcher;

  unordered_map<const ajn::InterfaceDescription*, Nan::Callback*>::iterator itor;
  for( itor = v8CallbackMap.begin(); itor != v8CallbackMap.end(); ++itor ){
    delete (*itor).second;
  }
}

QStatus 
BusObjectImpl::AddInterface(ajn::InterfaceDescription* interface){
    return ajn::BusObject::AddInterface(*interface);
}


QStatus
BusObjectImpl::AddMethodHandlers(ajn::InterfaceDescription* interface, Local<Object> v8CallbackObject){
  Local<Array> keys = v8CallbackObject->GetPropertyNames();

  vector<BusObject::MethodEntry> methodEntries;

  size_t size = keys->Length();

  for(size_t i=0;i<size;i++){
    Local<Value> key = Nan::Get(keys, i).ToLocalChecked();
    Local<Function> value = Nan::Get(v8CallbackObject, key).ToLocalChecked().As<Function>();

    v8CallbackMap.insert(make_pair(interface, new Nan::Callback(value)));

    BusObject::MethodEntry entry = {
      interface->GetMember(*Utf8String(key)),
      static_cast<MessageReceiver::MethodHandler>(&BusObjectImpl::MethodHandler)
    };

    methodEntries.push_back(entry);
  }

  return BusObject::AddMethodHandlers((const MethodEntry*)&methodEntries[0], size);      
}

struct BusMethodData{
  ajn::Message *message;
  const ajn::InterfaceDescription *interface;
};

void 
BusObjectImpl::MethodHandler(const ajn::InterfaceDescription::Member* member, ajn::Message& msg){
  BusMethodData *userData = new BusMethodData();
  userData->message = new ajn::Message(msg);
  userData->interface = member->iface;

  switcher->execute((void *)userData);
}

void
BusObjectImpl::v8MethodHandler(void *userData){
  BusMethodData *busMethodData = (BusMethodData *) userData;

  ajn::Message *ajnMsg = busMethodData->message;

  v8::Local<v8::Object> msg = Nan::New<v8::Object>();
  size_t msgIndex = 0;
  const ajn::MsgArg* arg = (*ajnMsg)->GetArg(msgIndex);
  while(arg != NULL){
    msgArgToObject(arg, msgIndex, msg);
    msgIndex++;
    arg = (*ajnMsg)->GetArg(msgIndex);
  }


  v8::Local<v8::Object> sender = Nan::New<v8::Object>();

  Nan::Set(sender, 
    Nan::New<v8::String>("sender").ToLocalChecked(), 
    Nan::New<v8::String>((*ajnMsg)->GetSender()).ToLocalChecked()
  );
  Nan::Set(sender, 
    Nan::New<v8::String>("session_id").ToLocalChecked(), 
    Nan::New<v8::Integer>((*ajnMsg)->GetSessionId())
  );
  Nan::Set(sender, 
    Nan::New<v8::String>("timestamp").ToLocalChecked(), 
    Nan::New<v8::Integer>((*ajnMsg)->GetTimeStamp())
  );
  Nan::Set(sender, 
    Nan::New<v8::String>("member_name").ToLocalChecked(), 
    Nan::New<v8::String>((*ajnMsg)->GetMemberName()).ToLocalChecked()
  );
  Nan::Set(sender, 
    Nan::New<v8::String>("object_path").ToLocalChecked(), 
    Nan::New<v8::String>((*ajnMsg)->GetObjectPath()).ToLocalChecked()
  );
  Nan::Set(sender, 
    Nan::New<v8::String>("signature").ToLocalChecked(), 
    Nan::New<v8::String>((*ajnMsg)->GetSignature()).ToLocalChecked()
  );    

  v8::Local<v8::Value> argv[] = {
    sender,
    msg    
  };

  Nan::Callback *v8Callback = (*v8CallbackMap.find(busMethodData->interface)).second;

  v8::Handle<v8::Value> rval = v8Callback->Call(2, argv);

  MsgArg outArg("s", *Utf8String(rval));

  QStatus status = BusObject::MethodReply((*ajnMsg), &outArg, 1);
  if (ER_OK != status) {
      printf("Ping: Error sending reply.\n");
  }

  delete ajnMsg;

  delete busMethodData;
}



