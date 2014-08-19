#include "util.h"
#include "nan.h"

#include <alljoyn/AllJoynStd.h>
#include <string.h>

void msgArgToObject(const ajn::MsgArg* arg, size_t index, v8::Local<v8::Object> out){
	switch(arg->typeId){
		case ajn::ALLJOYN_BOOLEAN:
		  out->Set(index, NanNew<v8::Boolean>(arg->v_bool));
		  break;
		case ajn::ALLJOYN_STRING:
		  out->Set(index, NanNew<v8::String>(arg->v_string.str));
		  break;
		case ajn::ALLJOYN_INT16:
		  out->Set(index, NanNew<v8::Integer>(arg->v_int16));
		  break;
		case ajn::ALLJOYN_INT32:
		  out->Set(index, NanNew<v8::Integer>(arg->v_int32));
		  break;
		case ajn::ALLJOYN_INT64:
		  out->Set(index, NanNew<v8::Integer>(arg->v_int64));
		  break;
		case ajn::ALLJOYN_UINT16:
		  out->Set(index, NanNew<v8::Integer>(arg->v_uint16));
		  break;
		case ajn::ALLJOYN_UINT32:
		  out->Set(index, NanNew<v8::Integer>(arg->v_uint32));
		  break;
		case ajn::ALLJOYN_UINT64:
		  out->Set(index, NanNew<v8::Integer>(arg->v_uint64));
		  break;
		case ajn::ALLJOYN_DOUBLE:
		  out->Set(index, NanNew<v8::Number>(arg->v_double));
		  break;
		case ajn::ALLJOYN_STRUCT:
			if(arg->v_struct.numMembers > 0){
			  v8::Local<v8::Object> inner = v8::Object::New();
			  for(size_t ix=0; ix<arg->v_struct.numMembers; ix++){
			  	msgArgToObject(&arg->v_struct.members[ix], ix, inner);
			  }
			  out->Set(index, inner);
			}
		  break;
		default:
		  printf("Unhandled type: %i - arg#%lu", arg->typeId, index);
		  break;
	}
}

ajn::MsgArg* objToMsgArg(v8::Local<v8::Value> obj){
	if(obj->IsString()){
		return new ajn::MsgArg("s", strdup(*NanUtf8String(obj)));
	}else if(obj->IsInt32()){
		return new ajn::MsgArg("i", obj->Int32Value());
	}else if(obj->IsNumber()){
		return new ajn::MsgArg("d", obj->NumberValue());
	}else if(obj->IsBoolean() || obj->IsBooleanObject()){
		return new ajn::MsgArg("b", obj->BooleanValue());
	}
	//TODO obj/array
}

