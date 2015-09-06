#include "nan.h"

#include "BusListenerImpl.h"
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/AllJoynStd.h>

#include <algorithm>

BusListenerImpl::BusListenerImpl(Nan::Callback* foundNameCallback, Nan::Callback* lostNameCallback, Nan::Callback* nameChangedCallback){
  loop = uv_default_loop();
  foundName.callback = foundNameCallback;
  lostName.callback = lostNameCallback;
  nameChanged.callback = nameChangedCallback;
  uv_async_init(loop, &found_async, callback);
  uv_async_init(loop, &lost_async, callback);
  uv_async_init(loop, &name_change_async, callback);
}

template<typename... Args>
void BusListenerImpl::callback(uv_async_t *handle, Args...) {
    CallbackHolder* holder = (CallbackHolder*) handle->data;

    Nan::HandleScope scope;

    std::queue<std::string> dataqueue;
    uv_mutex_lock(&holder->datalock);
    dataqueue = std::move(holder->dataqueue);
    uv_mutex_unlock(&holder->datalock);

    while (!dataqueue.empty()) {
      v8::Local<v8::Value> argv[] = {
        Nan::New<v8::String>(std::move(dataqueue.front())).ToLocalChecked()
      };
      holder->callback->Call(1, argv);
      dataqueue.pop();
    }
}

void BusListenerImpl::FoundAdvertisedName(const char* name, ajn::TransportMask transport, const char* namePrefix){
    found_async.data = (void*) &foundName;

    uv_mutex_lock(&foundName.datalock);
    foundName.dataqueue.emplace(name);
    uv_mutex_unlock(&foundName.datalock);

    uv_async_send(&found_async);
}

void BusListenerImpl::LostAdvertisedName(const char* name, ajn::TransportMask transport, const char* namePrefix){
    lost_async.data = (void*) &lostName;

    uv_mutex_lock(&lostName.datalock);
    lostName.dataqueue.emplace(name);
    uv_mutex_unlock(&lostName.datalock);

    uv_async_send(&lost_async);
}

void BusListenerImpl::NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner){
    name_change_async.data = (void*) &nameChanged;

    uv_mutex_lock(&nameChanged.datalock);
    nameChanged.dataqueue.emplace(busName);
    uv_mutex_unlock(&nameChanged.datalock);

    uv_async_send(&name_change_async);
}

