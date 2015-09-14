#include "util.h"
#include "nan.h"

#include <alljoyn/AllJoynStd.h>
#include <string.h>

void msgArgToObject(const ajn::MsgArg* arg, size_t index, v8::Local<v8::Object> out){
  switch(arg->typeId) {
  case ajn::ALLJOYN_BOOLEAN:
    Nan::Set(out, index, Nan::New<v8::Boolean>(arg->v_bool));
    break;
  case ajn::ALLJOYN_STRING:
    Nan::Set(out, index, Nan::New<v8::String>(arg->v_string.str).ToLocalChecked());
    break;
  case ajn::ALLJOYN_INT16:
    Nan::Set(out, index, Nan::New<v8::Integer>(arg->v_int16));
    break;
  case ajn::ALLJOYN_INT32:
    Nan::Set(out, index, Nan::New<v8::Integer>(arg->v_int32));
    break;
  case ajn::ALLJOYN_UINT16:
    Nan::Set(out, index, Nan::New<v8::Integer>(arg->v_uint16));
    break;
  case ajn::ALLJOYN_UINT32:
    Nan::Set(out, index, Nan::New<v8::Integer>(arg->v_uint32));
    break;
  case ajn::ALLJOYN_DOUBLE:
    Nan::Set(out, index, Nan::New<v8::Number>(arg->v_double));
    break;
  case ajn::ALLJOYN_STRUCT:
    if(arg->v_struct.numMembers > 0){
      v8::Local<v8::Object> inner = Nan::New<v8::Object>();
      for(size_t ix=0; ix<arg->v_struct.numMembers; ix++){
        msgArgToObject(&arg->v_struct.members[ix], ix, inner);
      }
      Nan::Set(out, index, inner);
    }
    break;
  case ajn::ALLJOYN_INT64:
    // Fallthrough to default because V8::Integer cannot be initialized from 64-bit integer
  case ajn::ALLJOYN_UINT64:
    // Fallthrough to default because V8::Integer cannot be initialized from 64-bit integer
  default:
    printf("Unhandled type: %i - arg#%lu", arg->typeId, index);
    break;
  }
}

ajn::MsgArg* objToMsgArg(v8::Local<v8::Value> obj){
  if(obj->IsString()){
    return new ajn::MsgArg("s", strdup(*Nan::Utf8String(obj)));
  }else if(obj->IsInt32()){
    return new ajn::MsgArg("i", obj->Int32Value());
  }else if(obj->IsNumber()){
    return new ajn::MsgArg("d", obj->NumberValue());
  }else if(obj->IsBoolean() || obj->IsBooleanObject()){
    return new ajn::MsgArg("b", obj->BooleanValue());
  }
  //TODO obj/array
}
