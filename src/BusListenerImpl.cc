#include <nan.h>

#include "BusListenerImpl.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

using namespace Nan;  // NOLINT(build/namespaces)

BusListenerImpl::BusListenerImpl(Nan::Callback* foundNameCallback, Nan::Callback* lostNameCallback, Nan::Callback* nameChangedCallback){
  loop = uv_default_loop();
  foundName.callback = foundNameCallback;
  lostName.callback = lostNameCallback;
  nameChanged.callback = nameChangedCallback;
  uv_async_init(loop, &found_async, (uv_async_cb) found_callback);
  uv_async_init(loop, &lost_async, (uv_async_cb) lost_callback);
  uv_async_init(loop, &name_change_async, (uv_async_cb) name_change_callback);
  
  foundName.data = 0;
  lostName.data = 0;
  nameChanged.data = 0;
  uv_rwlock_init(&foundName.datalock);
  uv_rwlock_init(&lostName.datalock);
  uv_rwlock_init(&nameChanged.datalock);
}

BusListenerImpl::~BusListenerImpl(){
  uv_rwlock_destroy(&foundName.datalock);
  uv_rwlock_destroy(&lostName.datalock);
  uv_rwlock_destroy(&nameChanged.datalock);
  free(foundName.data);
  free(lostName.data);
  free(nameChanged.data);
}

void BusListenerImpl::found_callback(uv_async_t *handle, int status) {
    CallbackHolder* holder = (CallbackHolder*) handle->data;

    uv_rwlock_rdlock(&holder->datalock);
    v8::Local<v8::Value> argv[] = {
      // New<v8::Integer>(*reinterpret_cast<int*>(const_cast<char*>(data)))
      Nan::New(holder->data).ToLocalChecked()
    };
    uv_rwlock_rdunlock(&holder->datalock);
    holder->callback->Call(1, argv);
}

void BusListenerImpl::lost_callback(uv_async_t *handle, int status) {
    CallbackHolder* holder = (CallbackHolder*) handle->data;

    uv_rwlock_rdlock(&holder->datalock);
    v8::Local<v8::Value> argv[] = {
      Nan::New(holder->data).ToLocalChecked()
    };
    uv_rwlock_rdunlock(&holder->datalock);
    holder->callback->Call(1, argv);
}

void BusListenerImpl::name_change_callback(uv_async_t *handle, int status) {
    CallbackHolder* holder = (CallbackHolder*) handle->data;

    uv_rwlock_rdlock(&holder->datalock);
    v8::Local<v8::Value> argv[] = {
      Nan::New(holder->data).ToLocalChecked()
    };
    uv_rwlock_rdunlock(&holder->datalock);
    holder->callback->Call(1, argv);
}

void BusListenerImpl::FoundAdvertisedName(const char* name, ajn::TransportMask transport, const char* namePrefix){
    found_async.data = (void*) &foundName;

    uv_rwlock_wrlock(&foundName.datalock);
    free(foundName.data);
    foundName.data = strdup(name);
    uv_rwlock_wrunlock(&foundName.datalock);

    uv_async_send(&found_async);
}

void BusListenerImpl::LostAdvertisedName(const char* name, ajn::TransportMask transport, const char* namePrefix){
    lost_async.data = (void*) &lostName;
    
    uv_rwlock_wrlock(&lostName.datalock);
    free(lostName.data);
    lostName.data = strdup(name);
    uv_rwlock_wrunlock(&lostName.datalock);

    uv_async_send(&lost_async);
}

void BusListenerImpl::NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner){
    name_change_async.data = (void*) &nameChanged;

    uv_rwlock_wrlock(&nameChanged.datalock);
    free(nameChanged.data);
    nameChanged.data = strdup(busName);
    uv_rwlock_wrunlock(&nameChanged.datalock);

    uv_async_send(&name_change_async);
}

