#include "nan.h"

#include "BusListenerImpl.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

BusListenerImpl::BusListenerImpl(NanCallback* foundName, NanCallback* lostName, NanCallback* nameChanged)
  :foundNameCallback(foundName),lostNameCallback(lostName),nameChangedCallback(nameChanged){
  loop = uv_default_loop();
  uv_async_init(loop, &found_async, found_callback);
  uv_async_init(loop, &lost_async, lost_callback);
  uv_async_init(loop, &name_change_async, name_change_callback);
}

BusListenerImpl::~BusListenerImpl(){
  printf("BusListenerImpl-destructor\n");
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

void BusListenerImpl::found_callback(uv_async_t *handle, int status) {
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

void BusListenerImpl::lost_callback(uv_async_t *handle, int status) {
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

void BusListenerImpl::name_change_callback(uv_async_t *handle, int status) {
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

void BusListenerImpl::FoundAdvertisedName(const char* name, ajn::TransportMask transport, const char* namePrefix){
    printf("Got FoundAdvertisedName for %s from transport 0x%x\n", name, transport);
    found_async.data = (void*) foundNameCallback;
    uv_async_send(&found_async);
}

void BusListenerImpl::LostAdvertisedName(const char* name, ajn::TransportMask transport, const char* namePrefix){
    printf("Got LostAdvertisedName for %s from transport 0x%x\n", name, transport);
    lost_async.data = (void*) lostNameCallback;
    uv_async_send(&lost_async);
}

void BusListenerImpl::NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner){
    printf("NameOwnerChanged: name=%s, oldOwner=%s, newOwner=%s\n", busName, previousOwner ? previousOwner : "<none>",
           newOwner ? newOwner : "<none>");
    name_change_async.data = (void*) nameChangedCallback;
    uv_async_send(&name_change_async);
}

