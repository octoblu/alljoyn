#include "nan.h"

#include "BusListenerWrapper.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

static v8::Persistent<v8::FunctionTemplate> listener_constructor;

v8::Handle<v8::Value> BusListenerWrapper::NewInstance() {
    NanScope();

    v8::Local<v8::Object> obj;
    v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(listener_constructor);
    obj = con->GetFunction()->NewInstance(0, NULL);
    return obj;
}

NAN_METHOD(BusListenerConstructor) {
  NanScope();
  if(args.Length() < 3){
    return NanThrowError("BusListener requires callbacks for FoundAdvertisedName, LostAdvertisedName, and NameOwnerChanged.");
  }
  v8::Local<v8::Object> obj;
  v8::Local<v8::FunctionTemplate> con = NanNew<v8::FunctionTemplate>(listener_constructor);

  v8::Handle<v8::Value> argv[] = {
    args[0],
    args[1],
    args[2]
  };
  obj = con->GetFunction()->NewInstance(3, argv);
  NanReturnValue(obj);
}

BusListenerWrapper::BusListenerWrapper(NanCallback* foundName, NanCallback* lostName, NanCallback* nameChanged)
  :foundNameCallback(foundName),lostNameCallback(lostName),nameChangedCallback(nameChanged){
  loop = uv_default_loop();
  uv_async_init(loop, &found_async, found_callback);
  uv_async_init(loop, &lost_async, lost_callback);
  uv_async_init(loop, &name_change_async, name_change_callback);
}

BusListenerWrapper::~BusListenerWrapper(){
  printf("BusListener-destructor\n");
  if(foundNameCallback){
    delete foundNameCallback;
  }
  if(lostNameCallback){
    delete lostNameCallback;
  }
  if(nameChangedCallback){
    delete nameChangedCallback;
  }
}

void BusListenerWrapper::Init () {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(BusListenerWrapper::New);
  NanAssignPersistent(listener_constructor, tpl);
  tpl->SetClassName(NanSymbol("BusListener"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
}

NAN_METHOD(BusListenerWrapper::New) {
  NanScope();
  if(args.Length() < 3){
    return NanThrowError("BusListener requires callbacks for FoundAdvertisedName, LostAdvertisedName, and NameOwnerChanged.");
  }
  v8::Local<v8::Function> foundName = args[0].As<v8::Function>();
  NanCallback *foundNameCall = new NanCallback(foundName);
  v8::Local<v8::Function> lostName = args[1].As<v8::Function>();
  NanCallback *lostNameCall = new NanCallback(lostName);
  v8::Local<v8::Function> changeName = args[2].As<v8::Function>();
  NanCallback *nameChangeCall = new NanCallback(changeName);

  BusListenerWrapper* obj = new BusListenerWrapper(NULL, NULL, NULL);//foundNameCall, lostNameCall, nameChangeCall);
  obj->Wrap(args.This());

  NanReturnValue(args.This());
}

void BusListenerWrapper::found_callback(uv_async_t *handle, int status) {
    // char* name = (char*) handle->data;
    // printf("Found-Callback:  %s\n", name);
    // v8::Handle<v8::Value> argv[] = {
    //   NanNull(),
    //   NanNew<v8::String>(name)
    // };
    printf("Calling found callback");
    NanCallback* callback = (NanCallback*) handle->data;
    callback->Call(0, NULL);
}

void BusListenerWrapper::lost_callback(uv_async_t *handle, int status) {
    // char* name = (char*) handle->data;
    // printf("Found-Callback:  %s\n", name);
    // v8::Handle<v8::Value> argv[] = {
    //   NanNull(),
    //   NanNew<v8::String>(name)
    // };
    printf("Calling lost callback");
    NanCallback* callback = (NanCallback*) handle->data;
    callback->Call(0, NULL);
}

void BusListenerWrapper::name_change_callback(uv_async_t *handle, int status) {
    // char* name = (char*) handle->data;
    // printf("NameChange-Callback:  %s\n", name);
    // v8::Handle<v8::Value> argv[] = {
    //   NanNull(),
    //   NanNew<v8::String>(name)
    // };
    printf("Calling name change callback");
    NanCallback* callback = (NanCallback*) handle->data;
    callback->Call(0, NULL);
}

void BusListenerWrapper::FoundAdvertisedName(const char* name, ajn::TransportMask transport, const char* namePrefix){
    printf("Got FoundAdvertisedName for %s from transport 0x%x\n", name, transport);
    found_async.data = (void*) foundNameCallback;
    uv_async_send(&found_async);
}

void BusListenerWrapper::LostAdvertisedName(const char* name, ajn::TransportMask transport, const char* namePrefix){
    printf("Got LostAdvertisedName for %s from transport 0x%x\n", name, transport);
    lost_async.data = (void*) lostNameCallback;
    uv_async_send(&lost_async);
}

void BusListenerWrapper::NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner){
    printf("NameOwnerChanged: name=%s, oldOwner=%s, newOwner=%s\n", busName, previousOwner ? previousOwner : "<none>",
           newOwner ? newOwner : "<none>");
    name_change_async.data = (void*) nameChangedCallback;
    uv_async_send(&name_change_async);
}

