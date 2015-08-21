#include "util.h"

#include <alljoyn/AllJoynStd.h>
#include <string.h>

using namespace ajn;
using namespace std;

void msgArgToObject(const ajn::MsgArg* arg, v8::Local<v8::String> key, v8::Local<v8::Object> out){
	switch(arg->typeId){
		// case ALLJOYN_BOOLEAN_ARRAY:
	 //    case ALLJOYN_DOUBLE_ARRAY:
	 //    case ALLJOYN_INT32_ARRAY:
	 //    case ALLJOYN_INT16_ARRAY:
	 //    case ALLJOYN_UINT16_ARRAY:
	 //    case ALLJOYN_UINT64_ARRAY:
	 //    case ALLJOYN_UINT32_ARRAY:
	 //    case ALLJOYN_INT64_ARRAY:
	    case ALLJOYN_BYTE_ARRAY:{
			size_t size = arg->v_scalarArray.numElements;

		    v8::Local<v8::Array> array = Nan::New<v8::Array>(size);

	        for (size_t i = 0; i < size; ++i) {
	        	Nan::Set(array, i, Nan::New<v8::Number>(arg->v_scalarArray.v_byte[i]));
	        }

		  	Nan::Set(out, key, array);

	    }
	    break;
		case ajn::ALLJOYN_ARRAY:{
			size_t size = arg->v_array.GetNumElements();
		    v8::Local<v8::Array> array = Nan::New<v8::Array>(size);

	        for (size_t i = 0; i < size; ++i) {
	        	const ajn::MsgArg *msgArg = &arg->v_array.GetElements()[i];

	        	msgArgToObject(msgArg, i, array);
	        }

		  	Nan::Set(out, key, array);
		  	break;
		}
	  	case ajn::ALLJOYN_VARIANT: {
	  		msgArgToObject(arg->v_variant.val, key, out);
            break;
        }		
		case ajn::ALLJOYN_DICT_ENTRY:{
			v8::Local<v8::String> key = Nan::New<v8::String>(std::string(arg->v_dictEntry.key->v_string.str)).ToLocalChecked();
			v8::Local<v8::Object> inner = Nan::New<v8::Object>();
			msgArgToObject(arg->v_dictEntry.val, key, inner);
			Nan::Set(out, 
			  key, 
			  inner
			);

			break;
		}
		case ajn::ALLJOYN_BOOLEAN:
		    Nan::Set(out, 
		      key, 
		      Nan::New<v8::Boolean>(arg->v_bool)
		    );
		  	break;
		case ajn::ALLJOYN_STRING:
		case ajn::ALLJOYN_OBJECT_PATH:
		    Nan::Set(out, 
		      key, 
		      Nan::New<v8::String>(arg->v_string.str).ToLocalChecked()
		    );
		  	break;
		case ajn::ALLJOYN_INT16:
		    Nan::Set(out, 
		      key, 
		      Nan::New<v8::Integer>(arg->v_int16)
		    );
		  	break;
		case ajn::ALLJOYN_INT32:
		    Nan::Set(out, 
		      key, 
		      Nan::New<v8::Integer>(arg->v_int32)
		    );
		  	break;
		case ajn::ALLJOYN_UINT16:
		    Nan::Set(out, 
		      key, 
		      Nan::New<v8::Integer>(arg->v_uint16)
		    );
		  	break;		  
		case ajn::ALLJOYN_UINT32:
		    Nan::Set(out, 
		      key, 
		      Nan::New<v8::Integer>(arg->v_uint32)
		    );
		  	break;		  
		case ajn::ALLJOYN_DOUBLE:
		    Nan::Set(out, 
		      key, 
		      Nan::New<v8::Number>(arg->v_double)
		    );
		  	break;		  
		case ajn::ALLJOYN_STRUCT:{
			if(arg->v_struct.numMembers > 0){
				v8::Local<v8::Array> inner = Nan::New<v8::Array>();
				for(size_t ix=0; ix<arg->v_struct.numMembers; ix++){
					msgArgToObject(&arg->v_struct.members[ix], ix, inner);
				}
		    	Nan::Set(out, 
		      		key, 
		      		inner
		    	);
		    }
			break;
		}
		case ajn::ALLJOYN_INT64:
		  // Fallthrough to default because V8::Integer cannot be initialized from 64-bit integer
		case ajn::ALLJOYN_UINT64:
		  // Fallthrough to default because V8::Integer cannot be initialized from 64-bit integer
		default:
		  printf("Unhandled type: %c, %u \n", arg->typeId, arg->typeId);
		  break;
	}	
}

void msgArgToObject(const ajn::MsgArg* arg, size_t index, v8::Local<v8::Object> out){
	switch(arg->typeId){
		// case ALLJOYN_BOOLEAN_ARRAY:
	 //    case ALLJOYN_DOUBLE_ARRAY:
	 //    case ALLJOYN_INT32_ARRAY:
	 //    case ALLJOYN_INT16_ARRAY:
	 //    case ALLJOYN_UINT16_ARRAY:
	 //    case ALLJOYN_UINT64_ARRAY:
	 //    case ALLJOYN_UINT32_ARRAY:
	 //    case ALLJOYN_INT64_ARRAY:
	    case ALLJOYN_BYTE_ARRAY:{
			size_t size = arg->v_scalarArray.numElements;

		    v8::Local<v8::Array> array = Nan::New<v8::Array>(size);

	        for (size_t i = 0; i < size; ++i) {
	        	Nan::Set(array, i, Nan::New<v8::Number>(arg->v_scalarArray.v_byte[i]));
	        }

		  	Nan::Set(out, index, array);

	    }
	    break;
		case ajn::ALLJOYN_ARRAY:{
			size_t size = arg->v_array.GetNumElements();
		    v8::Local<v8::Array> array = Nan::New<v8::Array>(size);

	        for (size_t i = 0; i < size; ++i) {
	        	const ajn::MsgArg *msgArg = &arg->v_array.GetElements()[i];

	        	msgArgToObject(msgArg, i, array);
	        }

		  	Nan::Set(out, index, array);
		  	break;
		}
	  	case ajn::ALLJOYN_VARIANT: {
	  		msgArgToObject(arg->v_variant.val, 0, out);
            break;
        }		
		case ajn::ALLJOYN_DICT_ENTRY:{
			v8::Local<v8::String> key = Nan::New<v8::String>(std::string(arg->v_dictEntry.key->v_string.str)).ToLocalChecked();

			v8::Local<v8::Object> inner = Nan::New<v8::Object>();

			msgArgToObject(arg->v_dictEntry.val, key, inner);

			Nan::Set(out, 
			  index, 
			  inner
			);

			break;
		}
		case ajn::ALLJOYN_BOOLEAN:
		    Nan::Set(out, 
		      index, 
		      Nan::New<v8::Boolean>(arg->v_bool)
		    );
		  	break;
		case ajn::ALLJOYN_STRING:
		case ajn::ALLJOYN_OBJECT_PATH:
		    Nan::Set(out, 
		      index, 
		      Nan::New<v8::String>(arg->v_string.str).ToLocalChecked()
		    );
		  	break;
		case ajn::ALLJOYN_INT16:
		    Nan::Set(out, 
		      index, 
		      Nan::New<v8::Integer>(arg->v_int16)
		    );
		  	break;
		case ajn::ALLJOYN_INT32:
		    Nan::Set(out, 
		      index, 
		      Nan::New<v8::Integer>(arg->v_int32)
		    );
		  	break;
		case ajn::ALLJOYN_UINT16:
		    Nan::Set(out, 
		      index, 
		      Nan::New<v8::Integer>(arg->v_uint16)
		    );
		  	break;		  
		case ajn::ALLJOYN_UINT32:
		    Nan::Set(out, 
		      index, 
		      Nan::New<v8::Integer>(arg->v_uint32)
		    );
		  	break;		  
		case ajn::ALLJOYN_DOUBLE:
		    Nan::Set(out, 
		      index, 
		      Nan::New<v8::Number>(arg->v_double)
		    );
		  	break;		  
		case ajn::ALLJOYN_STRUCT:
			if(arg->v_struct.numMembers > 0){
			  v8::Local<v8::Array> inner = Nan::New<v8::Array>();
			  for(size_t ix=0; ix<arg->v_struct.numMembers; ix++){
			  	msgArgToObject(&arg->v_struct.members[ix], ix, inner);
			  }
			    Nan::Set(out, 
			      index, 
			      inner
			    );
			}
		  break;

		case ajn::ALLJOYN_INT64:
		  // Fallthrough to default because V8::Integer cannot be initialized from 64-bit integer
		case ajn::ALLJOYN_UINT64:
		  // Fallthrough to default because V8::Integer cannot be initialized from 64-bit integer
		default:
		  printf("Unhandled type: %c, %u - arg#%lu \n", arg->typeId, arg->typeId, index);
		  break;
	}
}

ajn::MsgArg* objToMsgArg(v8::Local<v8::Value> obj){
	if(obj->IsString()){
		return new ajn::MsgArg("s", strdup(*Utf8String(obj)));
	}else if(obj->IsInt32()){
		return new ajn::MsgArg("i", obj->Int32Value());
	}else if(obj->IsNumber()){
		return new ajn::MsgArg("d", obj->NumberValue());
	}else if(obj->IsBoolean() || obj->IsBooleanObject()){
		return new ajn::MsgArg("b", obj->BooleanValue());
	}
	//TODO obj/array
}
