#include "AboutListenerImpl.h"
#include "util.h"

AboutListenerImpl::AboutListenerImpl(Callback *callback){
  loop = uv_default_loop();
  aboutCallback.callback = callback;
  uv_async_init(loop, &about_async, (uv_async_cb) about_callback);
  uv_rwlock_init(&calllock);
};


void
AboutListenerImpl::about_callback(uv_async_t *handle, int status){
  CallbackHolder* holder = (CallbackHolder*) handle->data;

  HandleScope scope;

  if(holder->objectDescriptionArg==NULL) printf("objectDescriptionArg null\n");
  if(holder->aboutDataArg==NULL) printf("aboutDataArg null\n");

  size_t descriptionSize = holder->objectDescriptionArg->v_array.GetNumElements();
  v8::Local<v8::Array> description = Nan::New<v8::Array>(descriptionSize);

  for(size_t i=0;i<descriptionSize;i++){
    msgArgToObject(&holder->objectDescriptionArg->v_array.GetElements()[i], i, description);
  }

  size_t aboutDataSize = holder->aboutDataArg->v_array.GetNumElements();


  v8::Local<v8::Object> aboutData = Nan::New<v8::Object>();

  for(size_t i=0;i<aboutDataSize;i++){
    const MsgArg *dictMsgArg = &holder->aboutDataArg->v_array.GetElements()[i];
    v8::Local<v8::String> key = Nan::New<v8::String>(dictMsgArg->v_dictEntry.key->v_string.str).ToLocalChecked();
    msgArgToObject(dictMsgArg->v_dictEntry.val, key, aboutData);
  }

  std::string busName(holder->busName);

  v8::Local<v8::Value> argv[] = {
    Nan::New<v8::String>(busName).ToLocalChecked(),
    Nan::New<v8::Number>(holder->version),
    Nan::New<v8::Number>(holder->port),
    description,
    aboutData
  };


  holder->callback->Call(5, argv);

  if(holder->objectDescriptionArg){
    delete holder->objectDescriptionArg;
    holder->objectDescriptionArg = NULL;
  }
  if(holder->aboutDataArg){
    delete holder->aboutDataArg;
    holder->aboutDataArg = NULL;
  }

  if(holder->busName){
    free((void *)holder->busName);
    holder->busName = NULL;
  }

}

void 
AboutListenerImpl::Announced(const char* busName, uint16_t version, SessionPort port, const MsgArg& objectDescriptionArg, const MsgArg& aboutDataArg) {
  about_async.data = (void*) &aboutCallback;
  aboutCallback.busName = strdup(busName);
  aboutCallback.version = version;
  aboutCallback.port = port;
  aboutCallback.objectDescriptionArg = new ajn::MsgArg(objectDescriptionArg);
  aboutCallback.aboutDataArg = new ajn::MsgArg(aboutDataArg);
  uv_async_send(&about_async);
}