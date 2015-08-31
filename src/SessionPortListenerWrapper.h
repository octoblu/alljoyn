#ifndef LD_SESSIONPORTLISTENERWRAPPER_H
#define LD_SESSIONPORTLISTENERWRAPPER_H

#include <nan.h>
#include <uv.h>
#include <alljoyn/AllJoynStd.h>
#include "SessionPortListenerImpl.h"

using namespace Nan;  // NOLINT(build/namespaces)


class SessionPortListenerWrapper : public node::ObjectWrap {
  private:
    static NAN_METHOD(New);

    static Persistent<v8::Function> constructor;
  public:
  	SessionPortListenerWrapper(Nan::Callback* accept, Nan::Callback* joined);
  	~SessionPortListenerWrapper();
    static void Init(v8::Handle<v8::Object> target);
    SessionPortListenerImpl* listener;
};

#endif