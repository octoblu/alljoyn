#ifndef LD_ALLJOYN_BINDING_UTIL_H
#define LD_ALLJOYN_BINDING_UTIL_H

#include <nan.h>
#include <alljoyn/AllJoynStd.h>

using namespace Nan;  // NOLINT(build/namespaces)

void msgArgToObject(const ajn::MsgArg* arg, v8::Local<v8::String> key, v8::Local<v8::Object> out);
void msgArgToObject(const ajn::MsgArg* arg, size_t index, v8::Local<v8::Object> out);
void objToMsgArg(const char* signature, v8::Local<v8::Value> obj, ajn::MsgArg *out);
ajn::MsgArg* objToMsgArg(v8::Local<v8::Value> obj);


#endif