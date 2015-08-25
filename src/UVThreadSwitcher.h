#ifndef LD_UVTHREADSWITCHER_H
#define LD_UVTHREADSWITCHER_H

typedef std::function<void(void *)> UV_THREAD_CALLBACK;

class UVThreadSwitcher{
    uv_loop_t *loop;
    uv_async_t async;
    uv_rwlock_t calllock;

    void* userData;

    UV_THREAD_CALLBACK uvThreadCallback;

  public:
    UVThreadSwitcher(UV_THREAD_CALLBACK callback){
      async.data = this;
      this->uvThreadCallback = std::move(callback);

      loop = uv_default_loop();
      uv_async_init(loop, &async, (uv_async_cb) uvCallback);
      uv_rwlock_init(&calllock);
    }

    virtual ~UVThreadSwitcher(){
      uv_rwlock_destroy(&calllock);
    }

    void setUserData(void* userData){
      this->userData = userData;
    }

    void* getUserData(){
      return this->userData;
    }

    void execute(void* userData){
      this->userData = userData;
      uv_async_send(&async);
    }

    static void uvCallback(uv_async_t *handle, int status){
      UVThreadSwitcher* that = (UVThreadSwitcher*) handle->data;
      that->uvThreadCallback(that->getUserData());
    }
};


#endif