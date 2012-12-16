#include <node.h>

#ifndef _WIN32
#  include <sys/statvfs.h>
#  include <string.h>
#  include <stdlib.h>
#  define __STDC_FORMAT_MACROS
#  include <inttypes.h>
#else
#  include <windows.h>
#endif

#define FS_SPACE_TOTAL 0
#define FS_SPACE_FREE 1
#define FS_SPACE_USED 2

using namespace node;
using namespace v8;

struct SpaceBaton {
  uv_work_t request;
  Persistent<Function> callback;

  const char* path;
  int which;
  uint64_t result;
};

uint64_t FS_GetFreeSpace(const char* path) {
#ifndef _WIN32
  struct statvfs st;
  int r = statvfs(path, &st);
  if (r < 0)
    return r;
  else
    return ((uint64_t)st.f_bavail) * ((uint64_t)st.f_frsize);
#else
  uint64_t free;
  BOOL r = GetDiskFreeSpaceEx(path, NULL, NULL, (PULARGE_INTEGER)&free);
  if (r)
    return free;
  else
    return -1;
#endif
}
uint64_t FS_GetTotalSpace(const char* path) {
#ifndef _WIN32
  struct statvfs st;
  int r = statvfs(path, &st);
  if (r < 0)
    return r;
  else
    return ((uint64_t)st.f_blocks) * ((uint64_t)st.f_frsize);
#else
  uint64_t total;
  BOOL r = GetDiskFreeSpaceEx(path, NULL, (PULARGE_INTEGER)&total, NULL);
  if (r)
    return total;
  else
    return -1;
#endif
}
uint64_t FS_GetUsedSpace(const char* path) {
#ifndef _WIN32
  struct statvfs st;
  int r = statvfs(path, &st);
  if (r < 0)
    return r;
  else
    return (((uint64_t)st.f_blocks) - ((uint64_t)st.f_bfree)) * ((uint64_t)st.f_frsize);
#else
  uint64_t total, free;
  BOOL r = GetDiskFreeSpaceEx(path, NULL, (PULARGE_INTEGER)&total, (PULARGE_INTEGER)&free);
  if (r)
    return (total - free);
  else
    return -1;
#endif
}
void NSpaceWork(uv_work_t* req) {
  SpaceBaton* baton = static_cast<SpaceBaton*>(req->data);

  if (baton->which == FS_SPACE_TOTAL)
    baton->result = FS_GetTotalSpace(baton->path);
  else if (baton->which == FS_SPACE_FREE)
    baton->result = FS_GetFreeSpace(baton->path);
  else
    baton->result = FS_GetUsedSpace(baton->path);
}
void NSpaceAfter(uv_work_t* req) {
  HandleScope scope;
  SpaceBaton* baton = static_cast<SpaceBaton*>(req->data);

  if (baton->result < 0) {
    Local<Value> err = Exception::Error(
      String::New(uv_strerror(uv_last_error(uv_default_loop())))
    );

    const unsigned argc = 1;
    Local<Value> argv[argc] = { err };

    TryCatch try_catch;
    baton->callback->Call(Context::GetCurrent()->Global(), argc, argv);
    if (try_catch.HasCaught())
      FatalException(try_catch);
  } else {
    char nStr[128];
    int cw;
#ifndef _WIN32
    cw = sprintf(nStr, "%" PRIu64, baton->result);
#else
    cw = sprintf(nStr, "%I64u", baton->result);
#endif
    const unsigned argc = 2;
    Local<Value> argv[argc] = {
      Local<Value>::New(Undefined()),
      Local<Value>::New(String::New((const char*)nStr, cw))
    };

    TryCatch try_catch;
    baton->callback->Call(Context::GetCurrent()->Global(), argc, argv);
    if (try_catch.HasCaught())
      FatalException(try_catch);
  }

  free((void*)baton->path);
  baton->callback.Dispose();
  delete baton;
}
Handle<Value> NGetSpace(const Arguments& args, int which) {
  HandleScope scope;
  bool async = (args.Length() > 1 && args[args.Length() - 1]->IsFunction());
  const char* path = NULL;
  String::Utf8Value pathStr(args[0]->ToString());

  if (async)
    path = strdup((const char*)(*pathStr));
  else
    path = (const char*)(*pathStr);

  if (async) {
    SpaceBaton* baton = new SpaceBaton();
    baton->request.data = baton;
    baton->path = path;
    baton->which = which;
    baton->callback = Persistent<Function>::New(
      Local<Function>::Cast(args[args.Length() - 1])
    );
    int status = uv_queue_work(uv_default_loop(),
                               &baton->request,
                               NSpaceWork,
                               (uv_after_work_cb)NSpaceAfter);
    assert(status == 0);
    return Undefined();
  } else {
    uint64_t r;
    if (which == FS_SPACE_TOTAL)
      r = FS_GetTotalSpace(path);
    else if (which == FS_SPACE_FREE)
      r = FS_GetFreeSpace(path);
    else
      r = FS_GetUsedSpace(path);
    if (r < 0) {
      return ThrowException(Exception::Error(
        String::New(uv_strerror(uv_last_error(uv_default_loop())))
      ));
    }
    char nStr[128];
    int cw;
#ifndef _WIN32
    cw = sprintf(nStr, "%" PRIu64, r);
#else
    cw = sprintf(nStr, "%I64u", r);
#endif
    Local<String> valStr = String::New((const char*)nStr, cw);
    return scope.Close(valStr);
  }
}
Handle<Value> NGetTotalSpace(const Arguments& args) {
  return NGetSpace(args, FS_SPACE_TOTAL);
}
Handle<Value> NGetUsedSpace(const Arguments& args) {
  return NGetSpace(args, FS_SPACE_USED);
}
Handle<Value> NGetFreeSpace(const Arguments& args) {
  return NGetSpace(args, FS_SPACE_FREE);
}

extern "C" {
  void init(Handle<Object> target) {
    HandleScope scope;
    target->Set(String::NewSymbol("getFree"),
                FunctionTemplate::New(NGetFreeSpace)->GetFunction());
    target->Set(String::NewSymbol("getTotal"),
                FunctionTemplate::New(NGetTotalSpace)->GetFunction());
    target->Set(String::NewSymbol("getUsed"),
                FunctionTemplate::New(NGetUsedSpace)->GetFunction());
  }

  NODE_MODULE(fs, init);
}
