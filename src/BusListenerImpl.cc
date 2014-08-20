#include "nan.h"

#include "BusListenerImpl.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

BusListenerImpl::BusListenerImpl(NanCallback* foundNameCallback, NanCallback* lostNameCallback, NanCallback* nameChangedCallback){
  loop = uv_default_loop();
  foundName.callback = foundNameCallback;
  lostName.callback = lostNameCallback;
  nameChanged.callback = nameChangedCallback;
  uv_async_init(loop, &found_async, found_callback);
  uv_async_init(loop, &lost_async, lost_callback);
  uv_async_init(loop, &name_change_async, name_change_callback);
}

BusListenerImpl::~BusListenerImpl(){
}

void BusListenerImpl::found_callback(uv_async_t *handle, int status) {
    CallbackHolder* holder = (CallbackHolder*) handle->data;

    v8::Handle<v8::Value> argv[] = {
      NanNew<v8::String>(holder->data)
    };
    holder->callback->Call(1, argv);
}

void BusListenerImpl::lost_callback(uv_async_t *handle, int status) {
    CallbackHolder* holder = (CallbackHolder*) handle->data;

    v8::Handle<v8::Value> argv[] = {
      NanNew<v8::String>(holder->data)
    };
    holder->callback->Call(1, argv);
}

void BusListenerImpl::name_change_callback(uv_async_t *handle, int status) {
    CallbackHolder* holder = (CallbackHolder*) handle->data;

    v8::Handle<v8::Value> argv[] = {
      NanNew<v8::String>(holder->data)
    };
    holder->callback->Call(1, argv);
}

void BusListenerImpl::FoundAdvertisedName(const char* name, ajn::TransportMask transport, const char* namePrefix){
    found_async.data = (void*) &foundName;
    foundName.data = name;
    uv_async_send(&found_async);
}

void BusListenerImpl::LostAdvertisedName(const char* name, ajn::TransportMask transport, const char* namePrefix){
    lost_async.data = (void*) &lostName;
    lostName.data = name;
    uv_async_send(&lost_async);
}

void BusListenerImpl::NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner){
    name_change_async.data = (void*) &nameChanged;
    nameChanged.data = busName;
    uv_async_send(&name_change_async);
}

