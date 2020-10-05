#include "crypto/crypto_hmac.h"
#include "crypto/crypto_keys.h"
#include "crypto/crypto_sig.h"
#include "crypto/crypto_util.h"
#include "allocated_buffer-inl.h"
#include "async_wrap-inl.h"
#include "base_object-inl.h"
#include "env-inl.h"
#include "memory_tracker-inl.h"
#include "node_buffer.h"
#include "string_bytes.h"
#include "threadpoolwork-inl.h"
#include "v8.h"

namespace node {

using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::Just;
using v8::Local;
using v8::Maybe;
using v8::MaybeLocal;
using v8::Nothing;
using v8::Object;
using v8::Uint32;
using v8::Value;

namespace crypto {
Hmac::Hmac(Environment* env, Local<Object> wrap)
    : BaseObject(env, wrap),
      ctx_(nullptr) {
  MakeWeak();
}

void Hmac::MemoryInfo(MemoryTracker* tracker) const {
  tracker->TrackFieldWithSize("context", ctx_ ? kSizeOf_HMAC_CTX : 0);
}

void Hmac::Initialize(Environment* env, Local<Object> target) {
  Local<FunctionTemplate> t = env->NewFunctionTemplate(New);

  t->InstanceTemplate()->SetInternalFieldCount(
      Hmac::kInternalFieldCount);
  t->Inherit(BaseObject::GetConstructorTemplate(env));

  env->SetProtoMethod(t, "init", HmacInit);
  env->SetProtoMethod(t, "update", HmacUpdate);
  env->SetProtoMethod(t, "digest", HmacDigest);

  target->Set(env->context(),
              FIXED_ONE_BYTE_STRING(env->isolate(), "Hmac"),
              t->GetFunction(env->context()).ToLocalChecked()).Check();

  HmacJob::Initialize(env, target);
}

void Hmac::New(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  new Hmac(env, args.This());
}

void Hmac::HmacInit(const char* hash_type, const char* key, int key_len) {
  HandleScope scope(env()->isolate());

  const EVP_MD* md = EVP_get_digestbyname(hash_type);
  if (md == nullptr)
    return THROW_ERR_CRYPTO_INVALID_DIGEST(env());
  if (key_len == 0) {
    key = "";
  }
  ctx_.reset(HMAC_CTX_new());
  if (!ctx_ || !HMAC_Init_ex(ctx_.get(), key, key_len, md, nullptr)) {
    ctx_.reset();
    return ThrowCryptoError(env(), ERR_get_error());
  }
}

void Hmac::HmacInit(const FunctionCallbackInfo<Value>& args) {
  Hmac* hmac;
  ASSIGN_OR_RETURN_UNWRAP(&hmac, args.Holder());
  Environment* env = hmac->env();

  const node::Utf8Value hash_type(env->isolate(), args[0]);
  ByteSource key = ByteSource::FromSecretKeyBytes(env, args[1]);
  hmac->HmacInit(*hash_type, key.get(), key.size());
}

bool Hmac::HmacUpdate(const char* data, size_t len) {
  return ctx_ && HMAC_Update(ctx_.get(),
                             reinterpret_cast<const unsigned char*>(data),
                             len) == 1;
}

void Hmac::HmacUpdate(const FunctionCallbackInfo<Value>& args) {
  Decode<Hmac>(args, [](Hmac* hmac, const FunctionCallbackInfo<Value>& args,
                        const char* data, size_t size) {
    Environment* env = Environment::GetCurrent(args);
    if (UNLIKELY(size > INT_MAX))
      return THROW_ERR_OUT_OF_RANGE(env, "data is too long");
    bool r = hmac->HmacUpdate(data, size);
    args.GetReturnValue().Set(r);
  });
}

void Hmac::HmacDigest(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  Hmac* hmac;
  ASSIGN_OR_RETURN_UNWRAP(&hmac, args.Holder());

  enum encoding encoding = BUFFER;
  if (args.Length() >= 1) {
    encoding = ParseEncoding(env->isolate(), args[0], BUFFER);
  }

  unsigned char md_value[EVP_MAX_MD_SIZE];
  unsigned int md_len = 0;

  if (hmac->ctx_) {
    HMAC_Final(hmac->ctx_.get(), md_value, &md_len);
    hmac->ctx_.reset();
  }

  Local<Value> error;
  MaybeLocal<Value> rc =
      StringBytes::Encode(env->isolate(),
                          reinterpret_cast<const char*>(md_value),
                          md_len,
                          encoding,
                          &error);
  if (rc.IsEmpty()) {
    CHECK(!error.IsEmpty());
    env->isolate()->ThrowException(error);
    return;
  }
  args.GetReturnValue().Set(rc.FromMaybe(Local<Value>()));
}

HmacConfig::HmacConfig(HmacConfig&& other) noexcept
    : job_mode(other.job_mode),
      mode(other.mode),
      key(std::move(other.key)),
      data(std::move(other.data)),
      signature(std::move(other.signature)),
      digest(other.digest) {}

HmacConfig& HmacConfig::operator=(HmacConfig&& other) noexcept {
  if (&other == this) return *this;
  this->~HmacConfig();
  return *new (this) HmacConfig(std::move(other));
}

void HmacConfig::MemoryInfo(MemoryTracker* tracker) const {
  tracker->TrackField("key", key.get());
  // If the job is sync, then the HmacConfig does not own the data
  if (job_mode == kCryptoJobAsync) {
    tracker->TrackFieldWithSize("data", data.size());
    tracker->TrackFieldWithSize("signature", signature.size());
  }
}

Maybe<bool> HmacTraits::AdditionalConfig(
    CryptoJobMode mode,
    const FunctionCallbackInfo<Value>& args,
    unsigned int offset,
    HmacConfig* params) {
  Environment* env = Environment::GetCurrent(args);

  params->job_mode = mode;

  CHECK(args[offset]->IsUint32());  // SignConfiguration::Mode
  params->mode =
    static_cast<SignConfiguration::Mode>(args[offset].As<Uint32>()->Value());

  CHECK(args[offset + 1]->IsString());  // Hash
  CHECK(args[offset + 2]->IsObject());  // Key

  Utf8Value digest(env->isolate(), args[offset + 1]);
  params->digest = EVP_get_digestbyname(*digest);
  if (params->digest == nullptr) {
    THROW_ERR_CRYPTO_INVALID_DIGEST(env);
    return Nothing<bool>();
  }

  KeyObjectHandle* key;
  ASSIGN_OR_RETURN_UNWRAP(&key, args[offset + 2], Nothing<bool>());
  params->key = key->Data();

  ArrayBufferOrViewContents<char> data(args[offset + 3]);
  if (UNLIKELY(!data.CheckSizeInt32())) {
    THROW_ERR_OUT_OF_RANGE(env, "data is too big");
    return Nothing<bool>();
  }
  params->data = mode == kCryptoJobAsync
      ? data.ToCopy()
      : data.ToByteSource();

  if (!args[offset + 4]->IsUndefined()) {
    ArrayBufferOrViewContents<char> signature(args[offset + 4]);
    if (UNLIKELY(!signature.CheckSizeInt32())) {
      THROW_ERR_OUT_OF_RANGE(env, "signature is too big");
      return Nothing<bool>();
    }
    params->signature = mode == kCryptoJobAsync
        ? signature.ToCopy()
        : signature.ToByteSource();
  }

  return Just(true);
}

bool HmacTraits::DeriveBits(
    Environment* env,
    const HmacConfig& params,
    ByteSource* out) {
  HMACCtxPointer ctx(HMAC_CTX_new());

  if (!ctx ||
      !HMAC_Init_ex(
          ctx.get(),
          params.key->GetSymmetricKey(),
          params.key->GetSymmetricKeySize(),
          params.digest,
          nullptr)) {
    return false;
  }

  if (!HMAC_Update(
          ctx.get(),
          params.data.data<unsigned char>(),
          params.data.size())) {
    return false;
  }

  char* data = MallocOpenSSL<char>(EVP_MAX_MD_SIZE);
  ByteSource buf = ByteSource::Allocated(data, EVP_MAX_MD_SIZE);
  unsigned char* ptr = reinterpret_cast<unsigned char*>(data);
  unsigned int len;

  if (!HMAC_Final(ctx.get(), ptr, &len)) {
    return false;
  }

  buf.Resize(len);
  *out = std::move(buf);

  return true;
}

Maybe<bool> HmacTraits::EncodeOutput(
    Environment* env,
    const HmacConfig& params,
    ByteSource* out,
    Local<Value>* result) {
  switch (params.mode) {
    case SignConfiguration::kSign:
      *result = out->ToArrayBuffer(env);
      break;
    case SignConfiguration::kVerify:
      *result =
          out->size() > 0 &&
          out->size() == params.signature.size() &&
          memcmp(out->get(), params.signature.get(), out->size()) == 0
              ? v8::True(env->isolate())
              : v8::False(env->isolate());
      break;
    default:
      UNREACHABLE();
  }
  return Just(!result->IsEmpty());
}

}  // namespace crypto
}  // namespace node
