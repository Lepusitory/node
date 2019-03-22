#include "node.h"

namespace node {

using v8::Function;
using v8::HandleScope;
using v8::Isolate;
using v8::Local;
using v8::MaybeLocal;
using v8::Object;
using v8::String;
using v8::Value;

AsyncResource::AsyncResource(Isolate* isolate,
                             Local<Object> resource,
                             const char* name,
                             async_id trigger_async_id)
    : isolate_(isolate),
      resource_(isolate, resource) {
  async_context_ = EmitAsyncInit(isolate, resource, name,
                                 trigger_async_id);
}

AsyncResource::~AsyncResource() {
  EmitAsyncDestroy(isolate_, async_context_);
  resource_.Reset();
}

MaybeLocal<Value> AsyncResource::MakeCallback(Local<Function> callback,
                                              int argc,
                                              Local<Value>* argv) {
  return node::MakeCallback(isolate_, get_resource(),
                            callback, argc, argv,
                            async_context_);
}

MaybeLocal<Value> AsyncResource::MakeCallback(const char* method,
                                              int argc,
                                              Local<Value>* argv) {
  return node::MakeCallback(isolate_, get_resource(),
                            method, argc, argv,
                            async_context_);
}

MaybeLocal<Value> AsyncResource::MakeCallback(Local<String> symbol,
                                              int argc,
                                              Local<Value>* argv) {
  return node::MakeCallback(isolate_, get_resource(),
                            symbol, argc, argv,
                            async_context_);
}

Local<Object> AsyncResource::get_resource() {
  return resource_.Get(isolate_);
}

async_id AsyncResource::get_async_id() const {
  return async_context_.async_id;
}

async_id AsyncResource::get_trigger_async_id() const {
  return async_context_.trigger_async_id;
}

AsyncResource::CallbackScope::CallbackScope(AsyncResource* res)
    : node::CallbackScope(res->isolate_,
                          res->resource_.Get(res->isolate_),
                          res->async_context_) {}

}  // namespace node
