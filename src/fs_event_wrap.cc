// Copyright Joyent, Inc. and other Node contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "async_wrap-inl.h"
#include "env-inl.h"
#include "node.h"
#include "handle_wrap.h"
#include "string_bytes.h"


namespace node {

using v8::Context;
using v8::DontDelete;
using v8::DontEnum;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::Integer;
using v8::Local;
using v8::MaybeLocal;
using v8::Object;
using v8::PropertyAttribute;
using v8::ReadOnly;
using v8::Signature;
using v8::String;
using v8::Value;

namespace {

class FSEventWrap: public HandleWrap {
 public:
  static void Initialize(Local<Object> target,
                         Local<Value> unused,
                         Local<Context> context,
                         void* priv);
  static void New(const FunctionCallbackInfo<Value>& args);
  static void Start(const FunctionCallbackInfo<Value>& args);
  static void GetInitialized(const FunctionCallbackInfo<Value>& args);

  SET_NO_MEMORY_INFO()
  SET_MEMORY_INFO_NAME(FSEventWrap)
  SET_SELF_SIZE(FSEventWrap)

 private:
  static const encoding kDefaultEncoding = UTF8;

  FSEventWrap(Environment* env, Local<Object> object);
  ~FSEventWrap() = default;

  static void OnEvent(uv_fs_event_t* handle, const char* filename, int events,
    int status);

  uv_fs_event_t handle_;
  enum encoding encoding_ = kDefaultEncoding;
};


FSEventWrap::FSEventWrap(Environment* env, Local<Object> object)
    : HandleWrap(env,
                 object,
                 reinterpret_cast<uv_handle_t*>(&handle_),
                 AsyncWrap::PROVIDER_FSEVENTWRAP) {
  MarkAsUninitialized();
}


void FSEventWrap::GetInitialized(const FunctionCallbackInfo<Value>& args) {
  FSEventWrap* wrap = Unwrap<FSEventWrap>(args.This());
  CHECK_NOT_NULL(wrap);
  args.GetReturnValue().Set(!wrap->IsHandleClosing());
}

void FSEventWrap::Initialize(Local<Object> target,
                             Local<Value> unused,
                             Local<Context> context,
                             void* priv) {
  Environment* env = Environment::GetCurrent(context);

  auto fsevent_string = FIXED_ONE_BYTE_STRING(env->isolate(), "FSEvent");
  Local<FunctionTemplate> t = env->NewFunctionTemplate(New);
  t->InstanceTemplate()->SetInternalFieldCount(1);
  t->SetClassName(fsevent_string);

  t->Inherit(AsyncWrap::GetConstructorTemplate(env));
  env->SetProtoMethod(t, "start", Start);
  env->SetProtoMethod(t, "close", Close);

  Local<FunctionTemplate> get_initialized_templ =
      FunctionTemplate::New(env->isolate(),
                            GetInitialized,
                            env->as_callback_data(),
                            Signature::New(env->isolate(), t));

  t->PrototypeTemplate()->SetAccessorProperty(
      FIXED_ONE_BYTE_STRING(env->isolate(), "initialized"),
      get_initialized_templ,
      Local<FunctionTemplate>(),
      static_cast<PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

  target->Set(env->context(),
              fsevent_string,
              t->GetFunction(context).ToLocalChecked()).Check();
}


void FSEventWrap::New(const FunctionCallbackInfo<Value>& args) {
  CHECK(args.IsConstructCall());
  Environment* env = Environment::GetCurrent(args);
  new FSEventWrap(env, args.This());
}

// wrap.start(filename, persistent, recursive, encoding)
void FSEventWrap::Start(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  FSEventWrap* wrap = Unwrap<FSEventWrap>(args.This());
  CHECK_NOT_NULL(wrap);
  CHECK(wrap->IsHandleClosing());  // Check that Start() has not been called.

  const int argc = args.Length();
  CHECK_GE(argc, 4);

  BufferValue path(env->isolate(), args[0]);
  CHECK_NOT_NULL(*path);

  unsigned int flags = 0;
  if (args[2]->IsTrue())
    flags |= UV_FS_EVENT_RECURSIVE;

  wrap->encoding_ = ParseEncoding(env->isolate(), args[3], kDefaultEncoding);

  int err = uv_fs_event_init(wrap->env()->event_loop(), &wrap->handle_);
  if (err != 0) {
    return args.GetReturnValue().Set(err);
  }

  err = uv_fs_event_start(&wrap->handle_, OnEvent, *path, flags);
  wrap->MarkAsInitialized();

  if (err != 0) {
    FSEventWrap::Close(args);
    return args.GetReturnValue().Set(err);
  }

  // Check for persistent argument
  if (!args[1]->IsTrue()) {
    uv_unref(reinterpret_cast<uv_handle_t*>(&wrap->handle_));
  }

  args.GetReturnValue().Set(err);
}


void FSEventWrap::OnEvent(uv_fs_event_t* handle, const char* filename,
    int events, int status) {
  FSEventWrap* wrap = static_cast<FSEventWrap*>(handle->data);
  Environment* env = wrap->env();

  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(env->context());

  CHECK_EQ(wrap->persistent().IsEmpty(), false);

  // We're in a bind here. libuv can set both UV_RENAME and UV_CHANGE but
  // the Node API only lets us pass a single event to JS land.
  //
  // The obvious solution is to run the callback twice, once for each event.
  // However, since the second event is not allowed to fire if the handle is
  // closed after the first event, and since there is no good way to detect
  // closed handles, that option is out.
  //
  // For now, ignore the UV_CHANGE event if UV_RENAME is also set. Make the
  // assumption that a rename implicitly means an attribute change. Not too
  // unreasonable, right? Still, we should revisit this before v1.0.
  Local<String> event_string;
  if (status) {
    event_string = String::Empty(env->isolate());
  } else if (events & UV_RENAME) {
    event_string = env->rename_string();
  } else if (events & UV_CHANGE) {
    event_string = env->change_string();
  } else {
    CHECK(0 && "bad fs events flag");
  }

  Local<Value> argv[] = {
    Integer::New(env->isolate(), status),
    event_string,
    Null(env->isolate())
  };

  if (filename != nullptr) {
    Local<Value> error;
    MaybeLocal<Value> fn = StringBytes::Encode(env->isolate(),
                                               filename,
                                               wrap->encoding_,
                                               &error);
    if (fn.IsEmpty()) {
      argv[0] = Integer::New(env->isolate(), UV_EINVAL);
      argv[2] = StringBytes::Encode(env->isolate(),
                                    filename,
                                    strlen(filename),
                                    BUFFER,
                                    &error).ToLocalChecked();
    } else {
      argv[2] = fn.ToLocalChecked();
    }
  }

  wrap->MakeCallback(env->onchange_string(), arraysize(argv), argv);
}

}  // anonymous namespace
}  // namespace node

NODE_MODULE_CONTEXT_AWARE_INTERNAL(fs_event_wrap, node::FSEventWrap::Initialize)
