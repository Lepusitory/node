#ifndef SRC_REQ_WRAP_INL_H_
#define SRC_REQ_WRAP_INL_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include "req_wrap.h"
#include "async_wrap-inl.h"
#include "env-inl.h"
#include "util-inl.h"
#include "uv.h"

namespace node {

template <typename T>
ReqWrap<T>::ReqWrap(Environment* env,
                    v8::Local<v8::Object> object,
                    AsyncWrap::ProviderType provider)
    : AsyncWrap(env, object, provider) {

  // FIXME(bnoordhuis) The fact that a reinterpret_cast is needed is
  // arguably a good indicator that there should be more than one queue.
  env->req_wrap_queue()->PushBack(reinterpret_cast<ReqWrap<uv_req_t>*>(this));
}

template <typename T>
ReqWrap<T>::~ReqWrap() {
  CHECK_EQ(req_.data, this);  // Assert that someone has called Dispatched().
  CHECK_EQ(false, persistent().IsEmpty());
}

template <typename T>
void ReqWrap<T>::Dispatched() {
  req_.data = this;
}

template <typename T>
ReqWrap<T>* ReqWrap<T>::from_req(T* req) {
  return ContainerOf(&ReqWrap<T>::req_, req);
}

template <typename T>
void ReqWrap<T>::Cancel() {
  uv_cancel(reinterpret_cast<uv_req_t*>(&req_));
}

// Below is dark template magic designed to invoke libuv functions that
// initialize uv_req_t instances in a unified fashion, to allow easier
// tracking of active/inactive requests.

// Invoke a generic libuv function that initializes uv_req_t instances.
// This is, unfortunately, necessary since they come in three different
// variants that can not all be invoked in the same way:
// - int uv_foo(uv_loop_t* loop, uv_req_t* request, ...);
// - int uv_foo(uv_req_t* request, ...);
// - void uv_foo(uv_req_t* request, ...);
template <typename ReqT, typename T>
struct CallLibuvFunction;

// Detect `int uv_foo(uv_loop_t* loop, uv_req_t* request, ...);`.
template <typename ReqT, typename... Args>
struct CallLibuvFunction<ReqT, int(*)(uv_loop_t*, ReqT*, Args...)> {
  using T = int(*)(uv_loop_t*, ReqT*, Args...);
  template <typename... PassedArgs>
  static int Call(T fn, uv_loop_t* loop, ReqT* req, PassedArgs... args) {
    return fn(loop, req, args...);
  }
};

// Detect `int uv_foo(uv_req_t* request, ...);`.
template <typename ReqT, typename... Args>
struct CallLibuvFunction<ReqT, int(*)(ReqT*, Args...)> {
  using T = int(*)(ReqT*, Args...);
  template <typename... PassedArgs>
  static int Call(T fn, uv_loop_t* loop, ReqT* req, PassedArgs... args) {
    return fn(req, args...);
  }
};

// Detect `void uv_foo(uv_req_t* request, ...);`.
template <typename ReqT, typename... Args>
struct CallLibuvFunction<ReqT, void(*)(ReqT*, Args...)> {
  using T = void(*)(ReqT*, Args...);
  template <typename... PassedArgs>
  static int Call(T fn, uv_loop_t* loop, ReqT* req, PassedArgs... args) {
    fn(req, args...);
    return 0;
  }
};

// This is slightly darker magic: This template is 'applied' to each parameter
// passed to the libuv function. If the parameter type (aka `T`) is a
// function type, it is assumed that this it is the request callback, and a
// wrapper that calls the original callback is created.
// If not, the parameter is passed through verbatim.
template <typename ReqT, typename T>
struct MakeLibuvRequestCallback {
  static T For(ReqWrap<ReqT>* req_wrap, T v) {
    static_assert(!std::is_function<T>::value,
                  "MakeLibuvRequestCallback missed a callback");
    return v;
  }
};

// Match the `void callback(uv_req_t*, ...);` signature that all libuv
// callbacks use.
template <typename ReqT, typename... Args>
struct MakeLibuvRequestCallback<ReqT, void(*)(ReqT*, Args...)> {
  using F = void(*)(ReqT* req, Args... args);

  static void Wrapper(ReqT* req, Args... args) {
    ReqWrap<ReqT>* req_wrap = ContainerOf(&ReqWrap<ReqT>::req_, req);
    F original_callback = reinterpret_cast<F>(req_wrap->original_callback_);
    original_callback(req, args...);
  }

  static F For(ReqWrap<ReqT>* req_wrap, F v) {
    CHECK_EQ(req_wrap->original_callback_, nullptr);
    req_wrap->original_callback_ =
        reinterpret_cast<typename ReqWrap<ReqT>::callback_t>(v);
    return Wrapper;
  }
};

template <typename T>
template <typename LibuvFunction, typename... Args>
int ReqWrap<T>::Dispatch(LibuvFunction fn, Args... args) {
  Dispatched();

  // This expands as:
  //
  // return fn(env()->event_loop(), req(), arg1, arg2, Wrapper, arg3, ...)
  //           ^                                       ^        ^
  //           |                                       |        |
  //           \-- Omitted if `fn` has no              |        |
  //               first `uv_loop_t*` argument         |        |
  //                                                   |        |
  //     A function callback whose first argument      |        |
  //     matches the libuv request type is replaced ---/        |
  //     by the `Wrapper` method defined above                  |
  //                                                            |
  //            Other (non-function) arguments are passed  -----/
  //            through verbatim
  return CallLibuvFunction<T, LibuvFunction>::Call(
      fn,
      env()->event_loop(),
      req(),
      MakeLibuvRequestCallback<T, Args>::For(this, args)...);
}

}  // namespace node

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#endif  // SRC_REQ_WRAP_INL_H_
