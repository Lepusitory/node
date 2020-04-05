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

#ifndef SRC_NODE_H_
#define SRC_NODE_H_

#ifdef _WIN32
# ifndef BUILDING_NODE_EXTENSION
#  define NODE_EXTERN __declspec(dllexport)
# else
#  define NODE_EXTERN __declspec(dllimport)
# endif
#else
# define NODE_EXTERN __attribute__((visibility("default")))
#endif

#ifdef BUILDING_NODE_EXTENSION
# undef BUILDING_V8_SHARED
# undef BUILDING_UV_SHARED
# define USING_V8_SHARED 1
# define USING_UV_SHARED 1
#endif

// This should be defined in make system.
// See issue https://github.com/nodejs/node-v0.x-archive/issues/1236
#if defined(__MINGW32__) || defined(_MSC_VER)
#ifndef _WIN32_WINNT
# define _WIN32_WINNT 0x0600  // Windows Server 2008
#endif

#ifndef NOMINMAX
# define NOMINMAX
#endif

#endif

#if defined(_MSC_VER)
#define PATH_MAX MAX_PATH
#endif

#ifdef _WIN32
# define SIGKILL 9
#endif

#if (__GNUC__ >= 8) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
#endif
#include "v8.h"  // NOLINT(build/include_order)
#if (__GNUC__ >= 8) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

#include "v8-platform.h"  // NOLINT(build/include_order)
#include "node_version.h"  // NODE_MODULE_VERSION

#include <memory>
#include <functional>

// We cannot use __POSIX__ in this header because that's only defined when
// building Node.js.
#ifndef _WIN32
#include <signal.h>
#endif  // _WIN32

#define NODE_MAKE_VERSION(major, minor, patch)                                \
  ((major) * 0x1000 + (minor) * 0x100 + (patch))

#ifdef __clang__
# define NODE_CLANG_AT_LEAST(major, minor, patch)                             \
  (NODE_MAKE_VERSION(major, minor, patch) <=                                  \
      NODE_MAKE_VERSION(__clang_major__, __clang_minor__, __clang_patchlevel__))
#else
# define NODE_CLANG_AT_LEAST(major, minor, patch) (0)
#endif

#ifdef __GNUC__
# define NODE_GNUC_AT_LEAST(major, minor, patch)                              \
  (NODE_MAKE_VERSION(major, minor, patch) <=                                  \
      NODE_MAKE_VERSION(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__))
#else
# define NODE_GNUC_AT_LEAST(major, minor, patch) (0)
#endif

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS
# define NODE_DEPRECATED(message, declarator) declarator
#else  // NODE_WANT_INTERNALS
# if NODE_CLANG_AT_LEAST(2, 9, 0) || NODE_GNUC_AT_LEAST(4, 5, 0)
#  define NODE_DEPRECATED(message, declarator)                                 \
    __attribute__((deprecated(message))) declarator
# elif defined(_MSC_VER)
#  define NODE_DEPRECATED(message, declarator)                                 \
    __declspec(deprecated) declarator
# else
#  define NODE_DEPRECATED(message, declarator) declarator
# endif
#endif

// Forward-declare libuv loop
struct uv_loop_s;

// Forward-declare these functions now to stop MSVS from becoming
// terminally confused when it's done in node_internals.h
namespace node {

namespace tracing {

class TracingController;

}

NODE_EXTERN v8::Local<v8::Value> ErrnoException(v8::Isolate* isolate,
                                                int errorno,
                                                const char* syscall = nullptr,
                                                const char* message = nullptr,
                                                const char* path = nullptr);
NODE_EXTERN v8::Local<v8::Value> UVException(v8::Isolate* isolate,
                                             int errorno,
                                             const char* syscall = nullptr,
                                             const char* message = nullptr,
                                             const char* path = nullptr,
                                             const char* dest = nullptr);

NODE_DEPRECATED("Use ErrnoException(isolate, ...)",
                inline v8::Local<v8::Value> ErrnoException(
      int errorno,
      const char* syscall = nullptr,
      const char* message = nullptr,
      const char* path = nullptr) {
  return ErrnoException(v8::Isolate::GetCurrent(),
                        errorno,
                        syscall,
                        message,
                        path);
})

NODE_DEPRECATED("Use UVException(isolate, ...)",
                inline v8::Local<v8::Value> UVException(int errorno,
                                        const char* syscall = nullptr,
                                        const char* message = nullptr,
                                        const char* path = nullptr) {
  return UVException(v8::Isolate::GetCurrent(),
                     errorno,
                     syscall,
                     message,
                     path);
})

/*
 * These methods need to be called in a HandleScope.
 *
 * It is preferred that you use the `MakeCallback` overloads taking
 * `async_context` arguments.
 */

NODE_DEPRECATED("Use MakeCallback(..., async_context)",
                NODE_EXTERN v8::Local<v8::Value> MakeCallback(
                    v8::Isolate* isolate,
                    v8::Local<v8::Object> recv,
                    const char* method,
                    int argc,
                    v8::Local<v8::Value>* argv));
NODE_DEPRECATED("Use MakeCallback(..., async_context)",
                NODE_EXTERN v8::Local<v8::Value> MakeCallback(
                    v8::Isolate* isolate,
                    v8::Local<v8::Object> recv,
                    v8::Local<v8::String> symbol,
                    int argc,
                    v8::Local<v8::Value>* argv));
NODE_DEPRECATED("Use MakeCallback(..., async_context)",
                NODE_EXTERN v8::Local<v8::Value> MakeCallback(
                    v8::Isolate* isolate,
                    v8::Local<v8::Object> recv,
                    v8::Local<v8::Function> callback,
                    int argc,
                    v8::Local<v8::Value>* argv));

}  // namespace node

#include <cassert>
#include <cstdint>

#ifndef NODE_STRINGIFY
# define NODE_STRINGIFY(n) NODE_STRINGIFY_HELPER(n)
# define NODE_STRINGIFY_HELPER(n) #n
#endif

#ifdef _WIN32
#if !defined(_SSIZE_T_) && !defined(_SSIZE_T_DEFINED)
typedef intptr_t ssize_t;
# define _SSIZE_T_
# define _SSIZE_T_DEFINED
#endif
#else  // !_WIN32
# include <sys/types.h>  // size_t, ssize_t
#endif  // _WIN32


namespace node {

class IsolateData;
class Environment;

// TODO(addaleax): Officially deprecate this and replace it with something
// better suited for a public embedder API.
NODE_EXTERN int Start(int argc, char* argv[]);

// Tear down Node.js while it is running (there are active handles
// in the loop and / or actively executing JavaScript code).
NODE_EXTERN int Stop(Environment* env);

// TODO(addaleax): Officially deprecate this and replace it with something
// better suited for a public embedder API.
// It is recommended to use InitializeNodeWithArgs() instead as an embedder.
// Init() calls InitializeNodeWithArgs() and exits the process with the exit
// code returned from it.
NODE_EXTERN void Init(int* argc,
                      const char** argv,
                      int* exec_argc,
                      const char*** exec_argv);
// Set up per-process state needed to run Node.js. This will consume arguments
// from argv, fill exec_argv, and possibly add errors resulting from parsing
// the arguments to `errors`. The return value is a suggested exit code for the
// program; If it is 0, then initializing Node.js succeeded.
NODE_EXTERN int InitializeNodeWithArgs(std::vector<std::string>* argv,
                                       std::vector<std::string>* exec_argv,
                                       std::vector<std::string>* errors);

enum OptionEnvvarSettings {
  kAllowedInEnvironment,
  kDisallowedInEnvironment
};

NODE_EXTERN int ProcessGlobalArgs(std::vector<std::string>* args,
                      std::vector<std::string>* exec_args,
                      std::vector<std::string>* errors,
                      OptionEnvvarSettings settings);

class NodeArrayBufferAllocator;

// An ArrayBuffer::Allocator class with some Node.js-specific tweaks. If you do
// not have to use another allocator, using this class is recommended:
// - It supports Buffer.allocUnsafe() and Buffer.allocUnsafeSlow() with
//   uninitialized memory.
// - It supports transferring, rather than copying, ArrayBuffers when using
//   MessagePorts.
class NODE_EXTERN ArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
 public:
  // If `always_debug` is true, create an ArrayBuffer::Allocator instance
  // that performs additional integrity checks (e.g. make sure that only memory
  // that was allocated by the it is also freed by it).
  // This can also be set using the --debug-arraybuffer-allocations flag.
  static std::unique_ptr<ArrayBufferAllocator> Create(
      bool always_debug = false);

 private:
  virtual NodeArrayBufferAllocator* GetImpl() = 0;

  friend class IsolateData;
};

// Legacy equivalents for ArrayBufferAllocator::Create().
NODE_EXTERN ArrayBufferAllocator* CreateArrayBufferAllocator();
NODE_EXTERN void FreeArrayBufferAllocator(ArrayBufferAllocator* allocator);

class NODE_EXTERN IsolatePlatformDelegate {
 public:
  virtual std::shared_ptr<v8::TaskRunner> GetForegroundTaskRunner() = 0;
  virtual bool IdleTasksEnabled() = 0;
};

class NODE_EXTERN MultiIsolatePlatform : public v8::Platform {
 public:
  ~MultiIsolatePlatform() override = default;
  // Returns true if work was dispatched or executed. New tasks that are
  // posted during flushing of the queue are postponed until the next
  // flushing.
  virtual bool FlushForegroundTasks(v8::Isolate* isolate) = 0;
  virtual void DrainTasks(v8::Isolate* isolate) = 0;

  // TODO(addaleax): Remove this, it is unnecessary.
  // This would currently be called before `UnregisterIsolate()` but will be
  // folded into it in the future.
  virtual void CancelPendingDelayedTasks(v8::Isolate* isolate);

  // This needs to be called between the calls to `Isolate::Allocate()` and
  // `Isolate::Initialize()`, so that initialization can already start
  // using the platform.
  // When using `NewIsolate()`, this is taken care of by that function.
  // This function may only be called once per `Isolate`.
  virtual void RegisterIsolate(v8::Isolate* isolate,
                               struct uv_loop_s* loop) = 0;
  // This method can be used when an application handles task scheduling on its
  // own through `IsolatePlatformDelegate`. Upon registering an isolate with
  // this overload any other method in this class with the exception of
  // `UnregisterIsolate` *must not* be used on that isolate.
  virtual void RegisterIsolate(v8::Isolate* isolate,
                               IsolatePlatformDelegate* delegate) = 0;

  // This function may only be called once per `Isolate`, and discard any
  // pending delayed tasks scheduled for that isolate.
  // This needs to be called right before calling `Isolate::Dispose()`.
  virtual void UnregisterIsolate(v8::Isolate* isolate) = 0;

  // The platform should call the passed function once all state associated
  // with the given isolate has been cleaned up. This can, but does not have to,
  // happen asynchronously.
  virtual void AddIsolateFinishedCallback(v8::Isolate* isolate,
                                          void (*callback)(void*),
                                          void* data) = 0;

  static std::unique_ptr<MultiIsolatePlatform> Create(
      int thread_pool_size,
      v8::TracingController* tracing_controller = nullptr);
};

enum IsolateSettingsFlags {
  MESSAGE_LISTENER_WITH_ERROR_LEVEL = 1 << 0,
  DETAILED_SOURCE_POSITIONS_FOR_PROFILING = 1 << 1
};

struct IsolateSettings {
  uint64_t flags = MESSAGE_LISTENER_WITH_ERROR_LEVEL |
      DETAILED_SOURCE_POSITIONS_FOR_PROFILING;
  v8::MicrotasksPolicy policy = v8::MicrotasksPolicy::kExplicit;

  // Error handling callbacks
  v8::Isolate::AbortOnUncaughtExceptionCallback
      should_abort_on_uncaught_exception_callback = nullptr;
  v8::FatalErrorCallback fatal_error_callback = nullptr;
  v8::PrepareStackTraceCallback prepare_stack_trace_callback = nullptr;

  // Miscellaneous callbacks
  v8::PromiseRejectCallback promise_reject_callback = nullptr;
  v8::AllowWasmCodeGenerationCallback
      allow_wasm_code_generation_callback = nullptr;
  v8::HostCleanupFinalizationGroupCallback
      host_cleanup_finalization_group_callback = nullptr;
};

// Overriding IsolateSettings may produce unexpected behavior
// in Node.js core functionality, so proceed at your own risk.
NODE_EXTERN void SetIsolateUpForNode(v8::Isolate* isolate,
                                     const IsolateSettings& settings);

// Set a number of callbacks for the `isolate`, in particular the Node.js
// uncaught exception listener.
NODE_EXTERN void SetIsolateUpForNode(v8::Isolate* isolate);

// Creates a new isolate with Node.js-specific settings.
// This is a convenience method equivalent to using SetIsolateCreateParams(),
// Isolate::Allocate(), MultiIsolatePlatform::RegisterIsolate(),
// Isolate::Initialize(), and SetIsolateUpForNode().
NODE_EXTERN v8::Isolate* NewIsolate(ArrayBufferAllocator* allocator,
                                    struct uv_loop_s* event_loop);
NODE_EXTERN v8::Isolate* NewIsolate(ArrayBufferAllocator* allocator,
                                    struct uv_loop_s* event_loop,
                                    MultiIsolatePlatform* platform);
NODE_EXTERN v8::Isolate* NewIsolate(
    std::shared_ptr<ArrayBufferAllocator> allocator,
    struct uv_loop_s* event_loop,
    MultiIsolatePlatform* platform);

// Creates a new context with Node.js-specific tweaks.
NODE_EXTERN v8::Local<v8::Context> NewContext(
    v8::Isolate* isolate,
    v8::Local<v8::ObjectTemplate> object_template =
        v8::Local<v8::ObjectTemplate>());

// Runs Node.js-specific tweaks on an already constructed context
// Return value indicates success of operation
NODE_EXTERN bool InitializeContext(v8::Local<v8::Context> context);

// If `platform` is passed, it will be used to register new Worker instances.
// It can be `nullptr`, in which case creating new Workers inside of
// Environments that use this `IsolateData` will not work.
NODE_EXTERN IsolateData* CreateIsolateData(
    v8::Isolate* isolate,
    struct uv_loop_s* loop,
    MultiIsolatePlatform* platform = nullptr,
    ArrayBufferAllocator* allocator = nullptr);
NODE_EXTERN void FreeIsolateData(IsolateData* isolate_data);

struct ThreadId {
  uint64_t id = static_cast<uint64_t>(-1);
};
NODE_EXTERN ThreadId AllocateEnvironmentThreadId();

namespace EnvironmentFlags {
enum Flags : uint64_t {
  kNoFlags = 0,
  // Use the default behaviour for Node.js instances.
  kDefaultFlags = 1 << 0,
  // Controls whether this Environment is allowed to affect per-process state
  // (e.g. cwd, process title, uid, etc.).
  // This is set when using kDefaultFlags.
  kOwnsProcessState = 1 << 1,
  // Set if this Environment instance is associated with the global inspector
  // handling code (i.e. listening on SIGUSR1).
  // This is set when using kDefaultFlags.
  kOwnsInspector = 1 << 2
};
}  // namespace EnvironmentFlags

struct InspectorParentHandle {
  virtual ~InspectorParentHandle();
};

// TODO(addaleax): Maybe move per-Environment options parsing here.
// Returns nullptr when the Environment cannot be created e.g. there are
// pending JavaScript exceptions.
// It is recommended to use the second variant taking a flags argument.
NODE_EXTERN Environment* CreateEnvironment(IsolateData* isolate_data,
                                           v8::Local<v8::Context> context,
                                           int argc,
                                           const char* const* argv,
                                           int exec_argc,
                                           const char* const* exec_argv);
NODE_EXTERN Environment* CreateEnvironment(
    IsolateData* isolate_data,
    v8::Local<v8::Context> context,
    const std::vector<std::string>& args,
    const std::vector<std::string>& exec_args,
    EnvironmentFlags::Flags flags = EnvironmentFlags::kDefaultFlags,
    ThreadId thread_id = {} /* allocates a thread id automatically */,
    std::unique_ptr<InspectorParentHandle> inspector_parent_handle = {});

// Returns a handle that can be passed to `LoadEnvironment()`, making the
// child Environment accessible to the inspector as if it were a Node.js Worker.
// `child_thread_id` can be created using `AllocateEnvironmentThreadId()`
// and then later passed on to `CreateEnvironment()` to create the child
// Environment, together with the inspector handle.
// This method should not be called while the parent Environment is active
// on another thread.
NODE_EXTERN std::unique_ptr<InspectorParentHandle> GetInspectorParentHandle(
    Environment* parent_env,
    ThreadId child_thread_id,
    const char* child_url);

struct StartExecutionCallbackInfo {
  v8::Local<v8::Object> process_object;
  v8::Local<v8::Function> native_require;
};

using StartExecutionCallback =
    std::function<v8::MaybeLocal<v8::Value>(const StartExecutionCallbackInfo&)>;

// TODO(addaleax): Deprecate this in favour of the MaybeLocal<> overload.
NODE_EXTERN void LoadEnvironment(Environment* env);
// The `InspectorParentHandle` arguments here are ignored and not used.
// For passing `InspectorParentHandle`, use `CreateEnvironment()`.
NODE_EXTERN v8::MaybeLocal<v8::Value> LoadEnvironment(
    Environment* env,
    StartExecutionCallback cb,
    std::unique_ptr<InspectorParentHandle> ignored_donotuse_removeme = {});
NODE_EXTERN v8::MaybeLocal<v8::Value> LoadEnvironment(
    Environment* env,
    const char* main_script_source_utf8,
    std::unique_ptr<InspectorParentHandle> ignored_donotuse_removeme = {});
NODE_EXTERN void FreeEnvironment(Environment* env);

// Set a callback that is called when process.exit() is called from JS,
// overriding the default handler.
// It receives the Environment* instance and the exit code as arguments.
// This could e.g. call Stop(env); in order to terminate execution and stop
// the event loop.
// The default handler disposes of the global V8 platform instance, if one is
// being used, and calls exit().
NODE_EXTERN void SetProcessExitHandler(
    Environment* env,
    std::function<void(Environment*, int)>&& handler);
NODE_EXTERN void DefaultProcessExitHandler(Environment* env, int exit_code);

// This may return nullptr if context is not associated with a Node instance.
NODE_EXTERN Environment* GetCurrentEnvironment(v8::Local<v8::Context> context);

// This returns the MultiIsolatePlatform used in the main thread of Node.js.
// If NODE_USE_V8_PLATFORM has not been defined when Node.js was built,
// it returns nullptr.
// TODO(addaleax): Deprecate in favour of GetMultiIsolatePlatform().
NODE_EXTERN MultiIsolatePlatform* GetMainThreadMultiIsolatePlatform();
// This returns the MultiIsolatePlatform used for an Environment or IsolateData
// instance, if one exists.
NODE_EXTERN MultiIsolatePlatform* GetMultiIsolatePlatform(Environment* env);
NODE_EXTERN MultiIsolatePlatform* GetMultiIsolatePlatform(IsolateData* env);

// Legacy variants of MultiIsolatePlatform::Create().
// TODO(addaleax): Deprecate in favour of the v8::TracingController variant.
NODE_EXTERN MultiIsolatePlatform* CreatePlatform(
    int thread_pool_size,
    node::tracing::TracingController* tracing_controller);
NODE_EXTERN MultiIsolatePlatform* CreatePlatform(
    int thread_pool_size,
    v8::TracingController* tracing_controller);
NODE_EXTERN void FreePlatform(MultiIsolatePlatform* platform);

NODE_EXTERN void EmitBeforeExit(Environment* env);
NODE_EXTERN int EmitExit(Environment* env);
NODE_EXTERN void RunAtExit(Environment* env);

// This may return nullptr if the current v8::Context is not associated
// with a Node instance.
NODE_EXTERN struct uv_loop_s* GetCurrentEventLoop(v8::Isolate* isolate);

/* Converts a unixtime to V8 Date */
NODE_DEPRECATED("Use v8::Date::New() directly",
                inline v8::Local<v8::Value> NODE_UNIXTIME_V8(double time) {
                  return v8::Date::New(
                             v8::Isolate::GetCurrent()->GetCurrentContext(),
                             1000 * time)
                      .ToLocalChecked();
                })
#define NODE_UNIXTIME_V8 node::NODE_UNIXTIME_V8
NODE_DEPRECATED("Use v8::Date::ValueOf() directly",
                inline double NODE_V8_UNIXTIME(v8::Local<v8::Date> date) {
  return date->ValueOf() / 1000;
})
#define NODE_V8_UNIXTIME node::NODE_V8_UNIXTIME

#define NODE_DEFINE_CONSTANT(target, constant)                                \
  do {                                                                        \
    v8::Isolate* isolate = target->GetIsolate();                              \
    v8::Local<v8::Context> context = isolate->GetCurrentContext();            \
    v8::Local<v8::String> constant_name =                                     \
        v8::String::NewFromUtf8(isolate, #constant,                           \
            v8::NewStringType::kInternalized).ToLocalChecked();               \
    v8::Local<v8::Number> constant_value =                                    \
        v8::Number::New(isolate, static_cast<double>(constant));              \
    v8::PropertyAttribute constant_attributes =                               \
        static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete);    \
    (target)->DefineOwnProperty(context,                                      \
                                constant_name,                                \
                                constant_value,                               \
                                constant_attributes).Check();                 \
  }                                                                           \
  while (0)

#define NODE_DEFINE_HIDDEN_CONSTANT(target, constant)                         \
  do {                                                                        \
    v8::Isolate* isolate = target->GetIsolate();                              \
    v8::Local<v8::Context> context = isolate->GetCurrentContext();            \
    v8::Local<v8::String> constant_name =                                     \
        v8::String::NewFromUtf8(isolate, #constant,                           \
                                v8::NewStringType::kInternalized)             \
                                  .ToLocalChecked();                          \
    v8::Local<v8::Number> constant_value =                                    \
        v8::Number::New(isolate, static_cast<double>(constant));              \
    v8::PropertyAttribute constant_attributes =                               \
        static_cast<v8::PropertyAttribute>(v8::ReadOnly |                     \
                                           v8::DontDelete |                   \
                                           v8::DontEnum);                     \
    (target)->DefineOwnProperty(context,                                      \
                                constant_name,                                \
                                constant_value,                               \
                                constant_attributes).Check();                 \
  }                                                                           \
  while (0)

// Used to be a macro, hence the uppercase name.
inline void NODE_SET_METHOD(v8::Local<v8::Template> recv,
                            const char* name,
                            v8::FunctionCallback callback) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(isolate,
                                                                callback);
  v8::Local<v8::String> fn_name = v8::String::NewFromUtf8(isolate, name,
      v8::NewStringType::kInternalized).ToLocalChecked();
  t->SetClassName(fn_name);
  recv->Set(fn_name, t);
}

// Used to be a macro, hence the uppercase name.
inline void NODE_SET_METHOD(v8::Local<v8::Object> recv,
                            const char* name,
                            v8::FunctionCallback callback) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(isolate,
                                                                callback);
  v8::Local<v8::Function> fn = t->GetFunction(context).ToLocalChecked();
  v8::Local<v8::String> fn_name = v8::String::NewFromUtf8(isolate, name,
      v8::NewStringType::kInternalized).ToLocalChecked();
  fn->SetName(fn_name);
  recv->Set(context, fn_name, fn).Check();
}
#define NODE_SET_METHOD node::NODE_SET_METHOD

// Used to be a macro, hence the uppercase name.
// Not a template because it only makes sense for FunctionTemplates.
inline void NODE_SET_PROTOTYPE_METHOD(v8::Local<v8::FunctionTemplate> recv,
                                      const char* name,
                                      v8::FunctionCallback callback) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Signature> s = v8::Signature::New(isolate, recv);
  v8::Local<v8::FunctionTemplate> t =
      v8::FunctionTemplate::New(isolate, callback, v8::Local<v8::Value>(), s);
  v8::Local<v8::String> fn_name = v8::String::NewFromUtf8(isolate, name,
      v8::NewStringType::kInternalized).ToLocalChecked();
  t->SetClassName(fn_name);
  recv->PrototypeTemplate()->Set(fn_name, t);
}
#define NODE_SET_PROTOTYPE_METHOD node::NODE_SET_PROTOTYPE_METHOD

// BINARY is a deprecated alias of LATIN1.
enum encoding {ASCII, UTF8, BASE64, UCS2, BINARY, HEX, BUFFER, LATIN1 = BINARY};

NODE_EXTERN enum encoding ParseEncoding(
    v8::Isolate* isolate,
    v8::Local<v8::Value> encoding_v,
    enum encoding default_encoding = LATIN1);

NODE_EXTERN void FatalException(v8::Isolate* isolate,
                                const v8::TryCatch& try_catch);

NODE_EXTERN v8::Local<v8::Value> Encode(v8::Isolate* isolate,
                                        const char* buf,
                                        size_t len,
                                        enum encoding encoding = LATIN1);

// Warning: This reverses endianness on Big Endian platforms, even though the
// signature using uint16_t implies that it should not.
NODE_EXTERN v8::Local<v8::Value> Encode(v8::Isolate* isolate,
                                        const uint16_t* buf,
                                        size_t len);

// Returns -1 if the handle was not valid for decoding
NODE_EXTERN ssize_t DecodeBytes(v8::Isolate* isolate,
                                v8::Local<v8::Value>,
                                enum encoding encoding = LATIN1);
// returns bytes written.
NODE_EXTERN ssize_t DecodeWrite(v8::Isolate* isolate,
                                char* buf,
                                size_t buflen,
                                v8::Local<v8::Value>,
                                enum encoding encoding = LATIN1);
#ifdef _WIN32
NODE_EXTERN v8::Local<v8::Value> WinapiErrnoException(
    v8::Isolate* isolate,
    int errorno,
    const char* syscall = nullptr,
    const char* msg = "",
    const char* path = nullptr);
#endif

const char* signo_string(int errorno);


typedef void (*addon_register_func)(
    v8::Local<v8::Object> exports,
    v8::Local<v8::Value> module,
    void* priv);

typedef void (*addon_context_register_func)(
    v8::Local<v8::Object> exports,
    v8::Local<v8::Value> module,
    v8::Local<v8::Context> context,
    void* priv);

enum ModuleFlags {
  kLinked = 0x02
};

struct node_module {
  int nm_version;
  unsigned int nm_flags;
  void* nm_dso_handle;
  const char* nm_filename;
  node::addon_register_func nm_register_func;
  node::addon_context_register_func nm_context_register_func;
  const char* nm_modname;
  void* nm_priv;
  struct node_module* nm_link;
};

extern "C" NODE_EXTERN void node_module_register(void* mod);

#ifdef _WIN32
# define NODE_MODULE_EXPORT __declspec(dllexport)
#else
# define NODE_MODULE_EXPORT __attribute__((visibility("default")))
#endif

#ifdef NODE_SHARED_MODE
# define NODE_CTOR_PREFIX
#else
# define NODE_CTOR_PREFIX static
#endif

#if defined(_MSC_VER)
#pragma section(".CRT$XCU", read)
#define NODE_C_CTOR(fn)                                               \
  NODE_CTOR_PREFIX void __cdecl fn(void);                             \
  __declspec(dllexport, allocate(".CRT$XCU"))                         \
      void (__cdecl*fn ## _)(void) = fn;                              \
  NODE_CTOR_PREFIX void __cdecl fn(void)
#else
#define NODE_C_CTOR(fn)                                               \
  NODE_CTOR_PREFIX void fn(void) __attribute__((constructor));        \
  NODE_CTOR_PREFIX void fn(void)
#endif

#define NODE_MODULE_X(modname, regfunc, priv, flags)                  \
  extern "C" {                                                        \
    static node::node_module _module =                                \
    {                                                                 \
      NODE_MODULE_VERSION,                                            \
      flags,                                                          \
      NULL,  /* NOLINT (readability/null_usage) */                    \
      __FILE__,                                                       \
      (node::addon_register_func) (regfunc),                          \
      NULL,  /* NOLINT (readability/null_usage) */                    \
      NODE_STRINGIFY(modname),                                        \
      priv,                                                           \
      NULL   /* NOLINT (readability/null_usage) */                    \
    };                                                                \
    NODE_C_CTOR(_register_ ## modname) {                              \
      node_module_register(&_module);                                 \
    }                                                                 \
  }

#define NODE_MODULE_CONTEXT_AWARE_X(modname, regfunc, priv, flags)    \
  extern "C" {                                                        \
    static node::node_module _module =                                \
    {                                                                 \
      NODE_MODULE_VERSION,                                            \
      flags,                                                          \
      NULL,  /* NOLINT (readability/null_usage) */                    \
      __FILE__,                                                       \
      NULL,  /* NOLINT (readability/null_usage) */                    \
      (node::addon_context_register_func) (regfunc),                  \
      NODE_STRINGIFY(modname),                                        \
      priv,                                                           \
      NULL  /* NOLINT (readability/null_usage) */                     \
    };                                                                \
    NODE_C_CTOR(_register_ ## modname) {                              \
      node_module_register(&_module);                                 \
    }                                                                 \
  }

// Usage: `NODE_MODULE(NODE_GYP_MODULE_NAME, InitializerFunction)`
// If no NODE_MODULE is declared, Node.js looks for the well-known
// symbol `node_register_module_v${NODE_MODULE_VERSION}`.
#define NODE_MODULE(modname, regfunc)                                 \
  NODE_MODULE_X(modname, regfunc, NULL, 0)  // NOLINT (readability/null_usage)

#define NODE_MODULE_CONTEXT_AWARE(modname, regfunc)                   \
  /* NOLINTNEXTLINE (readability/null_usage) */                       \
  NODE_MODULE_CONTEXT_AWARE_X(modname, regfunc, NULL, 0)

// Embedders can use this type of binding for statically linked native bindings.
// It is used the same way addon bindings are used, except that linked bindings
// can be accessed through `process._linkedBinding(modname)`.
#define NODE_MODULE_LINKED(modname, regfunc)                               \
  /* NOLINTNEXTLINE (readability/null_usage) */                            \
  NODE_MODULE_CONTEXT_AWARE_X(modname, regfunc, NULL,                      \
                              node::ModuleFlags::kLinked)

/*
 * For backward compatibility in add-on modules.
 */
#define NODE_MODULE_DECL /* nothing */

#define NODE_MODULE_INITIALIZER_BASE node_register_module_v

#define NODE_MODULE_INITIALIZER_X(base, version)                      \
    NODE_MODULE_INITIALIZER_X_HELPER(base, version)

#define NODE_MODULE_INITIALIZER_X_HELPER(base, version) base##version

#define NODE_MODULE_INITIALIZER                                       \
  NODE_MODULE_INITIALIZER_X(NODE_MODULE_INITIALIZER_BASE,             \
      NODE_MODULE_VERSION)

#define NODE_MODULE_INIT()                                            \
  extern "C" NODE_MODULE_EXPORT void                                  \
  NODE_MODULE_INITIALIZER(v8::Local<v8::Object> exports,              \
                          v8::Local<v8::Value> module,                \
                          v8::Local<v8::Context> context);            \
  NODE_MODULE_CONTEXT_AWARE(NODE_GYP_MODULE_NAME,                     \
                            NODE_MODULE_INITIALIZER)                  \
  void NODE_MODULE_INITIALIZER(v8::Local<v8::Object> exports,         \
                               v8::Local<v8::Value> module,           \
                               v8::Local<v8::Context> context)

// Allows embedders to add a binding to the current Environment* that can be
// accessed through process._linkedBinding() in the target Environment and all
// Worker threads that it creates.
// In each variant, the registration function needs to be usable at least for
// the time during which the Environment exists.
NODE_EXTERN void AddLinkedBinding(Environment* env, const node_module& mod);
NODE_EXTERN void AddLinkedBinding(Environment* env,
                                  const char* name,
                                  addon_context_register_func fn,
                                  void* priv);

/* Called after the event loop exits but before the VM is disposed.
 * Callbacks are run in reverse order of registration, i.e. newest first.
 *
 * You should always use the three-argument variant (or, for addons,
 * AddEnvironmentCleanupHook) in order to avoid relying on global state.
 */
NODE_DEPRECATED(
    "Use the three-argument variant of AtExit() or AddEnvironmentCleanupHook()",
    NODE_EXTERN void AtExit(void (*cb)(void* arg), void* arg = nullptr));

/* Registers a callback with the passed-in Environment instance. The callback
 * is called after the event loop exits, but before the VM is disposed.
 * Callbacks are run in reverse order of registration, i.e. newest first.
 */
NODE_EXTERN void AtExit(Environment* env,
                        void (*cb)(void* arg),
                        void* arg);
NODE_DEPRECATED(
    "Use the three-argument variant of AtExit() or AddEnvironmentCleanupHook()",
    inline void AtExit(Environment* env,
                       void (*cb)(void* arg)) {
      AtExit(env, cb, nullptr);
    })

typedef double async_id;
struct async_context {
  ::node::async_id async_id;
  ::node::async_id trigger_async_id;
};

/* This is a lot like node::AtExit, except that the hooks added via this
 * function are run before the AtExit ones and will always be registered
 * for the current Environment instance.
 * These functions are safe to use in an addon supporting multiple
 * threads/isolates. */
NODE_EXTERN void AddEnvironmentCleanupHook(v8::Isolate* isolate,
                                           void (*fun)(void* arg),
                                           void* arg);

NODE_EXTERN void RemoveEnvironmentCleanupHook(v8::Isolate* isolate,
                                              void (*fun)(void* arg),
                                              void* arg);

/* Returns the id of the current execution context. If the return value is
 * zero then no execution has been set. This will happen if the user handles
 * I/O from native code. */
NODE_EXTERN async_id AsyncHooksGetExecutionAsyncId(v8::Isolate* isolate);

/* Return same value as async_hooks.triggerAsyncId(); */
NODE_EXTERN async_id AsyncHooksGetTriggerAsyncId(v8::Isolate* isolate);

/* If the native API doesn't inherit from the helper class then the callbacks
 * must be triggered manually. This triggers the init() callback. The return
 * value is the async id assigned to the resource.
 *
 * The `trigger_async_id` parameter should correspond to the resource which is
 * creating the new resource, which will usually be the return value of
 * `AsyncHooksGetTriggerAsyncId()`. */
NODE_EXTERN async_context EmitAsyncInit(v8::Isolate* isolate,
                                        v8::Local<v8::Object> resource,
                                        const char* name,
                                        async_id trigger_async_id = -1);

NODE_EXTERN async_context EmitAsyncInit(v8::Isolate* isolate,
                                        v8::Local<v8::Object> resource,
                                        v8::Local<v8::String> name,
                                        async_id trigger_async_id = -1);

/* Emit the destroy() callback. The overload taking an `Environment*` argument
 * should be used when the Isolate’s current Context is not associated with
 * a Node.js Environment, or when there is no current Context, for example
 * when calling this function during garbage collection. In that case, the
 * `Environment*` value should have been acquired previously, e.g. through
 * `GetCurrentEnvironment()`. */
NODE_EXTERN void EmitAsyncDestroy(v8::Isolate* isolate,
                                  async_context asyncContext);
NODE_EXTERN void EmitAsyncDestroy(Environment* env,
                                  async_context asyncContext);

class InternalCallbackScope;

/* This class works like `MakeCallback()` in that it sets up a specific
 * asyncContext as the current one and informs the async_hooks and domains
 * modules that this context is currently active.
 *
 * `MakeCallback()` is a wrapper around this class as well as
 * `Function::Call()`. Either one of these mechanisms needs to be used for
 * top-level calls into JavaScript (i.e. without any existing JS stack).
 *
 * This object should be stack-allocated to ensure that it is contained in a
 * valid HandleScope.
 *
 * Exceptions happening within this scope will be treated like uncaught
 * exceptions. If this behaviour is undesirable, a new `v8::TryCatch` scope
 * needs to be created inside of this scope.
 */
class NODE_EXTERN CallbackScope {
 public:
  CallbackScope(v8::Isolate* isolate,
                v8::Local<v8::Object> resource,
                async_context asyncContext);
  ~CallbackScope();

  void operator=(const CallbackScope&) = delete;
  void operator=(CallbackScope&&) = delete;
  CallbackScope(const CallbackScope&) = delete;
  CallbackScope(CallbackScope&&) = delete;

 private:
  InternalCallbackScope* private_;
  v8::TryCatch try_catch_;
};

/* An API specific to emit before/after callbacks is unnecessary because
 * MakeCallback will automatically call them for you.
 *
 * These methods may create handles on their own, so run them inside a
 * HandleScope.
 *
 * `asyncId` and `triggerAsyncId` should correspond to the values returned by
 * `EmitAsyncInit()` and `AsyncHooksGetTriggerAsyncId()`, respectively, when the
 * invoking resource was created. If these values are unknown, 0 can be passed.
 * */
NODE_EXTERN
v8::MaybeLocal<v8::Value> MakeCallback(v8::Isolate* isolate,
                                       v8::Local<v8::Object> recv,
                                       v8::Local<v8::Function> callback,
                                       int argc,
                                       v8::Local<v8::Value>* argv,
                                       async_context asyncContext);
NODE_EXTERN
v8::MaybeLocal<v8::Value> MakeCallback(v8::Isolate* isolate,
                                       v8::Local<v8::Object> recv,
                                       const char* method,
                                       int argc,
                                       v8::Local<v8::Value>* argv,
                                       async_context asyncContext);
NODE_EXTERN
v8::MaybeLocal<v8::Value> MakeCallback(v8::Isolate* isolate,
                                       v8::Local<v8::Object> recv,
                                       v8::Local<v8::String> symbol,
                                       int argc,
                                       v8::Local<v8::Value>* argv,
                                       async_context asyncContext);

/* Helper class users can optionally inherit from. If
 * `AsyncResource::MakeCallback()` is used, then all four callbacks will be
 * called automatically. */
class NODE_EXTERN AsyncResource {
 public:
  AsyncResource(v8::Isolate* isolate,
                v8::Local<v8::Object> resource,
                const char* name,
                async_id trigger_async_id = -1);

  virtual ~AsyncResource();

  AsyncResource(const AsyncResource&) = delete;
  void operator=(const AsyncResource&) = delete;

  v8::MaybeLocal<v8::Value> MakeCallback(
      v8::Local<v8::Function> callback,
      int argc,
      v8::Local<v8::Value>* argv);

  v8::MaybeLocal<v8::Value> MakeCallback(
      const char* method,
      int argc,
      v8::Local<v8::Value>* argv);

  v8::MaybeLocal<v8::Value> MakeCallback(
      v8::Local<v8::String> symbol,
      int argc,
      v8::Local<v8::Value>* argv);

  v8::Local<v8::Object> get_resource();
  async_id get_async_id() const;
  async_id get_trigger_async_id() const;

 protected:
  class NODE_EXTERN CallbackScope : public node::CallbackScope {
   public:
    explicit CallbackScope(AsyncResource* res);
  };

 private:
  Environment* env_;
  v8::Global<v8::Object> resource_;
  async_context async_context_;
};

#ifndef _WIN32
// Register a signal handler without interrupting any handlers that node
// itself needs. This does override handlers registered through
// process.on('SIG...', function() { ... }). The `reset_handler` flag indicates
// whether the signal handler for the given signal should be reset to its
// default value before executing the handler (i.e. it works like SA_RESETHAND).
// The `reset_handler` flag is invalid when `signal` is SIGSEGV.
NODE_EXTERN
void RegisterSignalHandler(int signal,
                           void (*handler)(int signal,
                                           siginfo_t* info,
                                           void* ucontext),
                           bool reset_handler = false);
#endif  // _WIN32

}  // namespace node

#endif  // SRC_NODE_H_
