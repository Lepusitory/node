#include "node.h"
#include "node_internals.h"
#include "env-inl.h"
#include "util-inl.h"
#include "uv.h"
#include "v8.h"

#include <limits.h>  // PATH_MAX
#include <stdio.h>

#if defined(_MSC_VER)
#include <direct.h>
#include <io.h>
#define umask _umask
typedef int mode_t;
#else
#include <pthread.h>
#include <sys/resource.h>  // getrlimit, setrlimit
#include <termios.h>  // tcgetattr, tcsetattr
#include <unistd.h>  // setuid, getuid
#endif

#if defined(__POSIX__) && !defined(__ANDROID__) && !defined(__CloudABI__)
#include <pwd.h>  // getpwnam()
#include <grp.h>  // getgrnam()
#endif

namespace node {

using v8::Array;
using v8::ArrayBuffer;
using v8::BigUint64Array;
using v8::Float64Array;
using v8::FunctionCallbackInfo;
using v8::HeapStatistics;
using v8::Integer;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::String;
using v8::Uint32;
using v8::Uint32Array;
using v8::Value;

// Microseconds in a second, as a float, used in CPUUsage() below
#define MICROS_PER_SEC 1e6
// used in Hrtime() below
#define NANOS_PER_SEC 1000000000

#ifdef _WIN32
/* MAX_PATH is in characters, not bytes. Make sure we have enough headroom. */
#define CHDIR_BUFSIZE (MAX_PATH * 4)
#else
#define CHDIR_BUFSIZE (PATH_MAX)
#endif

void Abort(const FunctionCallbackInfo<Value>& args) {
  Abort();
}

void Chdir(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  CHECK(env->is_main_thread());

  CHECK_EQ(args.Length(), 1);
  CHECK(args[0]->IsString());
  Utf8Value path(env->isolate(), args[0]);
  int err = uv_chdir(*path);
  if (err) {
    // Also include the original working directory, since that will usually
    // be helpful information when debugging a `chdir()` failure.
    char buf[CHDIR_BUFSIZE];
    size_t cwd_len = sizeof(buf);
    uv_cwd(buf, &cwd_len);
    return env->ThrowUVException(err, "chdir", nullptr, buf, *path);
  }
}

// CPUUsage use libuv's uv_getrusage() this-process resource usage accessor,
// to access ru_utime (user CPU time used) and ru_stime (system CPU time used),
// which are uv_timeval_t structs (long tv_sec, long tv_usec).
// Returns those values as Float64 microseconds in the elements of the array
// passed to the function.
void CPUUsage(const FunctionCallbackInfo<Value>& args) {
  uv_rusage_t rusage;

  // Call libuv to get the values we'll return.
  int err = uv_getrusage(&rusage);
  if (err) {
    // On error, return the strerror version of the error code.
    Local<String> errmsg = OneByteString(args.GetIsolate(), uv_strerror(err));
    return args.GetReturnValue().Set(errmsg);
  }

  // Get the double array pointer from the Float64Array argument.
  CHECK(args[0]->IsFloat64Array());
  Local<Float64Array> array = args[0].As<Float64Array>();
  CHECK_EQ(array->Length(), 2);
  Local<ArrayBuffer> ab = array->Buffer();
  double* fields = static_cast<double*>(ab->GetContents().Data());

  // Set the Float64Array elements to be user / system values in microseconds.
  fields[0] = MICROS_PER_SEC * rusage.ru_utime.tv_sec + rusage.ru_utime.tv_usec;
  fields[1] = MICROS_PER_SEC * rusage.ru_stime.tv_sec + rusage.ru_stime.tv_usec;
}

void Cwd(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  char buf[CHDIR_BUFSIZE];
  size_t cwd_len = sizeof(buf);
  int err = uv_cwd(buf, &cwd_len);
  if (err)
    return env->ThrowUVException(err, "uv_cwd");

  Local<String> cwd = String::NewFromUtf8(env->isolate(),
                                          buf,
                                          v8::NewStringType::kNormal,
                                          cwd_len).ToLocalChecked();
  args.GetReturnValue().Set(cwd);
}


// Hrtime exposes libuv's uv_hrtime() high-resolution timer.

// This is the legacy version of hrtime before BigInt was introduced in
// JavaScript.
// The value returned by uv_hrtime() is a 64-bit int representing nanoseconds,
// so this function instead fills in an Uint32Array with 3 entries,
// to avoid any integer overflow possibility.
// The first two entries contain the second part of the value
// broken into the upper/lower 32 bits to be converted back in JS,
// because there is no Uint64Array in JS.
// The third entry contains the remaining nanosecond part of the value.
void Hrtime(const FunctionCallbackInfo<Value>& args) {
  uint64_t t = uv_hrtime();

  Local<ArrayBuffer> ab = args[0].As<Uint32Array>()->Buffer();
  uint32_t* fields = static_cast<uint32_t*>(ab->GetContents().Data());

  fields[0] = (t / NANOS_PER_SEC) >> 32;
  fields[1] = (t / NANOS_PER_SEC) & 0xffffffff;
  fields[2] = t % NANOS_PER_SEC;
}

void HrtimeBigInt(const FunctionCallbackInfo<Value>& args) {
  Local<ArrayBuffer> ab = args[0].As<BigUint64Array>()->Buffer();
  uint64_t* fields = static_cast<uint64_t*>(ab->GetContents().Data());
  fields[0] = uv_hrtime();
}

void Kill(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  if (args.Length() != 2)
    return env->ThrowError("Bad argument.");

  int pid = args[0]->Int32Value();
  int sig = args[1]->Int32Value();
  int err = uv_kill(pid, sig);
  args.GetReturnValue().Set(err);
}


void MemoryUsage(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  size_t rss;
  int err = uv_resident_set_memory(&rss);
  if (err)
    return env->ThrowUVException(err, "uv_resident_set_memory");

  Isolate* isolate = env->isolate();
  // V8 memory usage
  HeapStatistics v8_heap_stats;
  isolate->GetHeapStatistics(&v8_heap_stats);

  // Get the double array pointer from the Float64Array argument.
  CHECK(args[0]->IsFloat64Array());
  Local<Float64Array> array = args[0].As<Float64Array>();
  CHECK_EQ(array->Length(), 4);
  Local<ArrayBuffer> ab = array->Buffer();
  double* fields = static_cast<double*>(ab->GetContents().Data());

  fields[0] = rss;
  fields[1] = v8_heap_stats.total_heap_size();
  fields[2] = v8_heap_stats.used_heap_size();
  fields[3] = isolate->AdjustAmountOfExternalAllocatedMemory(0);
}

// Most of the time, it's best to use `console.error` to write
// to the process.stderr stream.  However, in some cases, such as
// when debugging the stream.Writable class or the process.nextTick
// function, it is useful to bypass JavaScript entirely.
void RawDebug(const FunctionCallbackInfo<Value>& args) {
  CHECK(args.Length() == 1 && args[0]->IsString() &&
        "must be called with a single string");
  Utf8Value message(args.GetIsolate(), args[0]);
  PrintErrorString("%s\n", *message);
  fflush(stderr);
}

void StartProfilerIdleNotifier(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  env->StartProfilerIdleNotifier();
}


void StopProfilerIdleNotifier(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  env->StopProfilerIdleNotifier();
}

void Umask(const FunctionCallbackInfo<Value>& args) {
  uint32_t old;

  CHECK_EQ(args.Length(), 1);
  CHECK(args[0]->IsUndefined() || args[0]->IsUint32());

  if (args[0]->IsUndefined()) {
    old = umask(0);
    umask(static_cast<mode_t>(old));
  } else {
    int oct = args[0].As<Uint32>()->Value();
    old = umask(static_cast<mode_t>(oct));
  }

  args.GetReturnValue().Set(old);
}

void Uptime(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  double uptime;

  uv_update_time(env->event_loop());
  uptime = uv_now(env->event_loop()) - prog_start_time;

  args.GetReturnValue().Set(Number::New(env->isolate(), uptime / 1000));
}


#if defined(__POSIX__) && !defined(__ANDROID__) && !defined(__CloudABI__)

static const uid_t uid_not_found = static_cast<uid_t>(-1);
static const gid_t gid_not_found = static_cast<gid_t>(-1);


static uid_t uid_by_name(const char* name) {
  struct passwd pwd;
  struct passwd* pp;
  char buf[8192];

  errno = 0;
  pp = nullptr;

  if (getpwnam_r(name, &pwd, buf, sizeof(buf), &pp) == 0 && pp != nullptr)
    return pp->pw_uid;

  return uid_not_found;
}


static char* name_by_uid(uid_t uid) {
  struct passwd pwd;
  struct passwd* pp;
  char buf[8192];
  int rc;

  errno = 0;
  pp = nullptr;

  if ((rc = getpwuid_r(uid, &pwd, buf, sizeof(buf), &pp)) == 0 &&
      pp != nullptr) {
    return strdup(pp->pw_name);
  }

  if (rc == 0)
    errno = ENOENT;

  return nullptr;
}


static gid_t gid_by_name(const char* name) {
  struct group pwd;
  struct group* pp;
  char buf[8192];

  errno = 0;
  pp = nullptr;

  if (getgrnam_r(name, &pwd, buf, sizeof(buf), &pp) == 0 && pp != nullptr)
    return pp->gr_gid;

  return gid_not_found;
}


#if 0  // For future use.
static const char* name_by_gid(gid_t gid) {
  struct group pwd;
  struct group* pp;
  char buf[8192];
  int rc;

  errno = 0;
  pp = nullptr;

  if ((rc = getgrgid_r(gid, &pwd, buf, sizeof(buf), &pp)) == 0 &&
      pp != nullptr) {
    return strdup(pp->gr_name);
  }

  if (rc == 0)
    errno = ENOENT;

  return nullptr;
}
#endif


static uid_t uid_by_name(Isolate* isolate, Local<Value> value) {
  if (value->IsUint32()) {
    return static_cast<uid_t>(value->Uint32Value());
  } else {
    Utf8Value name(isolate, value);
    return uid_by_name(*name);
  }
}


static gid_t gid_by_name(Isolate* isolate, Local<Value> value) {
  if (value->IsUint32()) {
    return static_cast<gid_t>(value->Uint32Value());
  } else {
    Utf8Value name(isolate, value);
    return gid_by_name(*name);
  }
}

void GetUid(const FunctionCallbackInfo<Value>& args) {
  // uid_t is an uint32_t on all supported platforms.
  args.GetReturnValue().Set(static_cast<uint32_t>(getuid()));
}


void GetGid(const FunctionCallbackInfo<Value>& args) {
  // gid_t is an uint32_t on all supported platforms.
  args.GetReturnValue().Set(static_cast<uint32_t>(getgid()));
}


void GetEUid(const FunctionCallbackInfo<Value>& args) {
  // uid_t is an uint32_t on all supported platforms.
  args.GetReturnValue().Set(static_cast<uint32_t>(geteuid()));
}


void GetEGid(const FunctionCallbackInfo<Value>& args) {
  // gid_t is an uint32_t on all supported platforms.
  args.GetReturnValue().Set(static_cast<uint32_t>(getegid()));
}


void SetGid(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  CHECK(env->is_main_thread());

  CHECK_EQ(args.Length(), 1);
  CHECK(args[0]->IsUint32() || args[0]->IsString());

  gid_t gid = gid_by_name(env->isolate(), args[0]);

  if (gid == gid_not_found) {
    // Tells JS to throw ERR_INVALID_CREDENTIAL
    args.GetReturnValue().Set(1);
  } else if (setgid(gid)) {
    env->ThrowErrnoException(errno, "setgid");
  } else {
    args.GetReturnValue().Set(0);
  }
}


void SetEGid(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  CHECK(env->is_main_thread());

  CHECK_EQ(args.Length(), 1);
  CHECK(args[0]->IsUint32() || args[0]->IsString());

  gid_t gid = gid_by_name(env->isolate(), args[0]);

  if (gid == gid_not_found) {
    // Tells JS to throw ERR_INVALID_CREDENTIAL
    args.GetReturnValue().Set(1);
  } else if (setegid(gid)) {
    env->ThrowErrnoException(errno, "setegid");
  } else {
    args.GetReturnValue().Set(0);
  }
}


void SetUid(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  CHECK(env->is_main_thread());

  CHECK_EQ(args.Length(), 1);
  CHECK(args[0]->IsUint32() || args[0]->IsString());

  uid_t uid = uid_by_name(env->isolate(), args[0]);

  if (uid == uid_not_found) {
    // Tells JS to throw ERR_INVALID_CREDENTIAL
    args.GetReturnValue().Set(1);
  } else if (setuid(uid)) {
    env->ThrowErrnoException(errno, "setuid");
  } else {
    args.GetReturnValue().Set(0);
  }
}


void SetEUid(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  CHECK(env->is_main_thread());

  CHECK_EQ(args.Length(), 1);
  CHECK(args[0]->IsUint32() || args[0]->IsString());

  uid_t uid = uid_by_name(env->isolate(), args[0]);

  if (uid == uid_not_found) {
    // Tells JS to throw ERR_INVALID_CREDENTIAL
    args.GetReturnValue().Set(1);
  } else if (seteuid(uid)) {
    env->ThrowErrnoException(errno, "seteuid");
  } else {
    args.GetReturnValue().Set(0);
  }
}


void GetGroups(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  int ngroups = getgroups(0, nullptr);

  if (ngroups == -1)
    return env->ThrowErrnoException(errno, "getgroups");

  gid_t* groups = new gid_t[ngroups];

  ngroups = getgroups(ngroups, groups);

  if (ngroups == -1) {
    delete[] groups;
    return env->ThrowErrnoException(errno, "getgroups");
  }

  Local<Array> groups_list = Array::New(env->isolate(), ngroups);
  bool seen_egid = false;
  gid_t egid = getegid();

  for (int i = 0; i < ngroups; i++) {
    groups_list->Set(i, Integer::New(env->isolate(), groups[i]));
    if (groups[i] == egid)
      seen_egid = true;
  }

  delete[] groups;

  if (seen_egid == false)
    groups_list->Set(ngroups, Integer::New(env->isolate(), egid));

  args.GetReturnValue().Set(groups_list);
}


void SetGroups(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  CHECK_EQ(args.Length(), 1);
  CHECK(args[0]->IsArray());

  Local<Array> groups_list = args[0].As<Array>();
  size_t size = groups_list->Length();
  gid_t* groups = new gid_t[size];

  for (size_t i = 0; i < size; i++) {
    gid_t gid = gid_by_name(env->isolate(), groups_list->Get(i));

    if (gid == gid_not_found) {
      delete[] groups;
      // Tells JS to throw ERR_INVALID_CREDENTIAL
      args.GetReturnValue().Set(static_cast<uint32_t>(i + 1));
      return;
    }

    groups[i] = gid;
  }

  int rc = setgroups(size, groups);
  delete[] groups;

  if (rc == -1)
    return env->ThrowErrnoException(errno, "setgroups");

  args.GetReturnValue().Set(0);
}


void InitGroups(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  CHECK_EQ(args.Length(), 2);
  CHECK(args[0]->IsUint32() || args[0]->IsString());
  CHECK(args[1]->IsUint32() || args[1]->IsString());

  Utf8Value arg0(env->isolate(), args[0]);
  gid_t extra_group;
  bool must_free;
  char* user;

  if (args[0]->IsUint32()) {
    user = name_by_uid(args[0]->Uint32Value());
    must_free = true;
  } else {
    user = *arg0;
    must_free = false;
  }

  if (user == nullptr) {
    // Tells JS to throw ERR_INVALID_CREDENTIAL
    return args.GetReturnValue().Set(1);
  }

  extra_group = gid_by_name(env->isolate(), args[1]);

  if (extra_group == gid_not_found) {
    if (must_free)
      free(user);
    // Tells JS to throw ERR_INVALID_CREDENTIAL
    return args.GetReturnValue().Set(2);
  }

  int rc = initgroups(user, extra_group);

  if (must_free)
    free(user);

  if (rc)
    return env->ThrowErrnoException(errno, "initgroups");

  args.GetReturnValue().Set(0);
}

#endif  // __POSIX__ && !defined(__ANDROID__) && !defined(__CloudABI__)

}  // namespace node
