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

#ifndef SRC_UTIL_INL_H_
#define SRC_UTIL_INL_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include <cstring>
#include "util.h"

#if defined(_MSC_VER)
#include <intrin.h>
#define BSWAP_2(x) _byteswap_ushort(x)
#define BSWAP_4(x) _byteswap_ulong(x)
#define BSWAP_8(x) _byteswap_uint64(x)
#else
#define BSWAP_2(x) ((x) << 8) | ((x) >> 8)
#define BSWAP_4(x)                                                            \
  (((x) & 0xFF) << 24) |                                                      \
  (((x) & 0xFF00) << 8) |                                                     \
  (((x) >> 8) & 0xFF00) |                                                     \
  (((x) >> 24) & 0xFF)
#define BSWAP_8(x)                                                            \
  (((x) & 0xFF00000000000000ull) >> 56) |                                     \
  (((x) & 0x00FF000000000000ull) >> 40) |                                     \
  (((x) & 0x0000FF0000000000ull) >> 24) |                                     \
  (((x) & 0x000000FF00000000ull) >> 8) |                                      \
  (((x) & 0x00000000FF000000ull) << 8) |                                      \
  (((x) & 0x0000000000FF0000ull) << 24) |                                     \
  (((x) & 0x000000000000FF00ull) << 40) |                                     \
  (((x) & 0x00000000000000FFull) << 56)
#endif

namespace node {

template <typename T>
ListNode<T>::ListNode() : prev_(this), next_(this) {}

template <typename T>
ListNode<T>::~ListNode() {
  Remove();
}

template <typename T>
void ListNode<T>::Remove() {
  prev_->next_ = next_;
  next_->prev_ = prev_;
  prev_ = this;
  next_ = this;
}

template <typename T>
bool ListNode<T>::IsEmpty() const {
  return prev_ == this;
}

template <typename T, ListNode<T> (T::*M)>
ListHead<T, M>::Iterator::Iterator(ListNode<T>* node) : node_(node) {}

template <typename T, ListNode<T> (T::*M)>
T* ListHead<T, M>::Iterator::operator*() const {
  return ContainerOf(M, node_);
}

template <typename T, ListNode<T> (T::*M)>
const typename ListHead<T, M>::Iterator&
ListHead<T, M>::Iterator::operator++() {
  node_ = node_->next_;
  return *this;
}

template <typename T, ListNode<T> (T::*M)>
bool ListHead<T, M>::Iterator::operator!=(const Iterator& that) const {
  return node_ != that.node_;
}

template <typename T, ListNode<T> (T::*M)>
ListHead<T, M>::~ListHead() {
  while (IsEmpty() == false)
    head_.next_->Remove();
}

template <typename T, ListNode<T> (T::*M)>
void ListHead<T, M>::PushBack(T* element) {
  ListNode<T>* that = &(element->*M);
  head_.prev_->next_ = that;
  that->prev_ = head_.prev_;
  that->next_ = &head_;
  head_.prev_ = that;
}

template <typename T, ListNode<T> (T::*M)>
void ListHead<T, M>::PushFront(T* element) {
  ListNode<T>* that = &(element->*M);
  head_.next_->prev_ = that;
  that->prev_ = &head_;
  that->next_ = head_.next_;
  head_.next_ = that;
}

template <typename T, ListNode<T> (T::*M)>
bool ListHead<T, M>::IsEmpty() const {
  return head_.IsEmpty();
}

template <typename T, ListNode<T> (T::*M)>
T* ListHead<T, M>::PopFront() {
  if (IsEmpty())
    return nullptr;
  ListNode<T>* node = head_.next_;
  node->Remove();
  return ContainerOf(M, node);
}

template <typename T, ListNode<T> (T::*M)>
typename ListHead<T, M>::Iterator ListHead<T, M>::begin() const {
  return Iterator(head_.next_);
}

template <typename T, ListNode<T> (T::*M)>
typename ListHead<T, M>::Iterator ListHead<T, M>::end() const {
  return Iterator(const_cast<ListNode<T>*>(&head_));
}

template <typename Inner, typename Outer>
constexpr uintptr_t OffsetOf(Inner Outer::*field) {
  return reinterpret_cast<uintptr_t>(&(static_cast<Outer*>(nullptr)->*field));
}

template <typename Inner, typename Outer>
ContainerOfHelper<Inner, Outer>::ContainerOfHelper(Inner Outer::*field,
                                                   Inner* pointer)
    : pointer_(
        reinterpret_cast<Outer*>(
            reinterpret_cast<uintptr_t>(pointer) - OffsetOf(field))) {}

template <typename Inner, typename Outer>
template <typename TypeName>
ContainerOfHelper<Inner, Outer>::operator TypeName*() const {
  return static_cast<TypeName*>(pointer_);
}

template <typename Inner, typename Outer>
constexpr ContainerOfHelper<Inner, Outer> ContainerOf(Inner Outer::*field,
                                                      Inner* pointer) {
  return ContainerOfHelper<Inner, Outer>(field, pointer);
}

inline v8::Local<v8::String> OneByteString(v8::Isolate* isolate,
                                           const char* data,
                                           int length) {
  return v8::String::NewFromOneByte(isolate,
                                    reinterpret_cast<const uint8_t*>(data),
                                    v8::NewStringType::kNormal,
                                    length).ToLocalChecked();
}

inline v8::Local<v8::String> OneByteString(v8::Isolate* isolate,
                                           const signed char* data,
                                           int length) {
  return v8::String::NewFromOneByte(isolate,
                                    reinterpret_cast<const uint8_t*>(data),
                                    v8::NewStringType::kNormal,
                                    length).ToLocalChecked();
}

inline v8::Local<v8::String> OneByteString(v8::Isolate* isolate,
                                           const unsigned char* data,
                                           int length) {
  return v8::String::NewFromOneByte(
             isolate, data, v8::NewStringType::kNormal, length)
      .ToLocalChecked();
}

void SwapBytes16(char* data, size_t nbytes) {
  CHECK_EQ(nbytes % 2, 0);

#if defined(_MSC_VER)
  int align = reinterpret_cast<uintptr_t>(data) % sizeof(uint16_t);
  if (align == 0) {
    // MSVC has no strict aliasing, and is able to highly optimize this case.
    uint16_t* data16 = reinterpret_cast<uint16_t*>(data);
    size_t len16 = nbytes / sizeof(*data16);
    for (size_t i = 0; i < len16; i++) {
      data16[i] = BSWAP_2(data16[i]);
    }
    return;
  }
#endif

  uint16_t temp;
  for (size_t i = 0; i < nbytes; i += sizeof(temp)) {
    memcpy(&temp, &data[i], sizeof(temp));
    temp = BSWAP_2(temp);
    memcpy(&data[i], &temp, sizeof(temp));
  }
}

void SwapBytes32(char* data, size_t nbytes) {
  CHECK_EQ(nbytes % 4, 0);

#if defined(_MSC_VER)
  int align = reinterpret_cast<uintptr_t>(data) % sizeof(uint32_t);
  // MSVC has no strict aliasing, and is able to highly optimize this case.
  if (align == 0) {
    uint32_t* data32 = reinterpret_cast<uint32_t*>(data);
    size_t len32 = nbytes / sizeof(*data32);
    for (size_t i = 0; i < len32; i++) {
      data32[i] = BSWAP_4(data32[i]);
    }
    return;
  }
#endif

  uint32_t temp;
  for (size_t i = 0; i < nbytes; i += sizeof(temp)) {
    memcpy(&temp, &data[i], sizeof(temp));
    temp = BSWAP_4(temp);
    memcpy(&data[i], &temp, sizeof(temp));
  }
}

void SwapBytes64(char* data, size_t nbytes) {
  CHECK_EQ(nbytes % 8, 0);

#if defined(_MSC_VER)
  int align = reinterpret_cast<uintptr_t>(data) % sizeof(uint64_t);
  if (align == 0) {
    // MSVC has no strict aliasing, and is able to highly optimize this case.
    uint64_t* data64 = reinterpret_cast<uint64_t*>(data);
    size_t len64 = nbytes / sizeof(*data64);
    for (size_t i = 0; i < len64; i++) {
      data64[i] = BSWAP_8(data64[i]);
    }
    return;
  }
#endif

  uint64_t temp;
  for (size_t i = 0; i < nbytes; i += sizeof(temp)) {
    memcpy(&temp, &data[i], sizeof(temp));
    temp = BSWAP_8(temp);
    memcpy(&data[i], &temp, sizeof(temp));
  }
}

char ToLower(char c) {
  return c >= 'A' && c <= 'Z' ? c + ('a' - 'A') : c;
}

std::string ToLower(const std::string& in) {
  std::string out(in.size(), 0);
  for (size_t i = 0; i < in.size(); ++i)
    out[i] = ToLower(in[i]);
  return out;
}

bool StringEqualNoCase(const char* a, const char* b) {
  do {
    if (*a == '\0')
      return *b == '\0';
    if (*b == '\0')
      return *a == '\0';
  } while (ToLower(*a++) == ToLower(*b++));
  return false;
}

bool StringEqualNoCaseN(const char* a, const char* b, size_t length) {
  for (size_t i = 0; i < length; i++) {
    if (ToLower(a[i]) != ToLower(b[i]))
      return false;
    if (a[i] == '\0')
      return true;
  }
  return true;
}

template <typename T>
inline T MultiplyWithOverflowCheck(T a, T b) {
  auto ret = a * b;
  if (a != 0)
    CHECK_EQ(b, ret / a);

  return ret;
}

// These should be used in our code as opposed to the native
// versions as they abstract out some platform and or
// compiler version specific functionality.
// malloc(0) and realloc(ptr, 0) have implementation-defined behavior in
// that the standard allows them to either return a unique pointer or a
// nullptr for zero-sized allocation requests.  Normalize by always using
// a nullptr.
template <typename T>
T* UncheckedRealloc(T* pointer, size_t n) {
  size_t full_size = MultiplyWithOverflowCheck(sizeof(T), n);

  if (full_size == 0) {
    free(pointer);
    return nullptr;
  }

  void* allocated = realloc(pointer, full_size);

  if (UNLIKELY(allocated == nullptr)) {
    // Tell V8 that memory is low and retry.
    LowMemoryNotification();
    allocated = realloc(pointer, full_size);
  }

  return static_cast<T*>(allocated);
}

// As per spec realloc behaves like malloc if passed nullptr.
template <typename T>
inline T* UncheckedMalloc(size_t n) {
  if (n == 0) n = 1;
  return UncheckedRealloc<T>(nullptr, n);
}

template <typename T>
inline T* UncheckedCalloc(size_t n) {
  if (n == 0) n = 1;
  MultiplyWithOverflowCheck(sizeof(T), n);
  return static_cast<T*>(calloc(n, sizeof(T)));
}

template <typename T>
inline T* Realloc(T* pointer, size_t n) {
  T* ret = UncheckedRealloc(pointer, n);
  CHECK_IMPLIES(n > 0, ret != nullptr);
  return ret;
}

template <typename T>
inline T* Malloc(size_t n) {
  T* ret = UncheckedMalloc<T>(n);
  CHECK_IMPLIES(n > 0, ret != nullptr);
  return ret;
}

template <typename T>
inline T* Calloc(size_t n) {
  T* ret = UncheckedCalloc<T>(n);
  CHECK_IMPLIES(n > 0, ret != nullptr);
  return ret;
}

// Shortcuts for char*.
inline char* Malloc(size_t n) { return Malloc<char>(n); }
inline char* Calloc(size_t n) { return Calloc<char>(n); }
inline char* UncheckedMalloc(size_t n) { return UncheckedMalloc<char>(n); }
inline char* UncheckedCalloc(size_t n) { return UncheckedCalloc<char>(n); }

// This is a helper in the .cc file so including util-inl.h doesn't include more
// headers than we really need to.
void ThrowErrStringTooLong(v8::Isolate* isolate);

v8::MaybeLocal<v8::Value> ToV8Value(v8::Local<v8::Context> context,
                                    const std::string& str,
                                    v8::Isolate* isolate) {
  if (isolate == nullptr) isolate = context->GetIsolate();
  if (UNLIKELY(str.size() >= static_cast<size_t>(v8::String::kMaxLength))) {
    // V8 only has a TODO comment about adding an exception when the maximum
    // string size is exceeded.
    ThrowErrStringTooLong(isolate);
    return v8::MaybeLocal<v8::Value>();
  }

  return v8::String::NewFromUtf8(
             isolate, str.data(), v8::NewStringType::kNormal, str.size())
      .FromMaybe(v8::Local<v8::String>());
}

template <typename T>
v8::MaybeLocal<v8::Value> ToV8Value(v8::Local<v8::Context> context,
                                    const std::vector<T>& vec,
                                    v8::Isolate* isolate) {
  if (isolate == nullptr) isolate = context->GetIsolate();
  v8::EscapableHandleScope handle_scope(isolate);

  MaybeStackBuffer<v8::Local<v8::Value>, 128> arr(vec.size());
  arr.SetLength(vec.size());
  for (size_t i = 0; i < vec.size(); ++i) {
    if (!ToV8Value(context, vec[i], isolate).ToLocal(&arr[i]))
      return v8::MaybeLocal<v8::Value>();
  }

  return handle_scope.Escape(v8::Array::New(isolate, arr.out(), arr.length()));
}

template <typename T, typename U>
v8::MaybeLocal<v8::Value> ToV8Value(v8::Local<v8::Context> context,
                                    const std::unordered_map<T, U>& map,
                                    v8::Isolate* isolate) {
  if (isolate == nullptr) isolate = context->GetIsolate();
  v8::EscapableHandleScope handle_scope(isolate);

  v8::Local<v8::Map> ret = v8::Map::New(isolate);
  for (const auto& item : map) {
    v8::Local<v8::Value> first, second;
    if (!ToV8Value(context, item.first, isolate).ToLocal(&first) ||
        !ToV8Value(context, item.second, isolate).ToLocal(&second) ||
        ret->Set(context, first, second).IsEmpty()) {
      return v8::MaybeLocal<v8::Value>();
    }
  }

  return handle_scope.Escape(ret);
}

template <typename T, typename >
v8::MaybeLocal<v8::Value> ToV8Value(v8::Local<v8::Context> context,
                                    const T& number,
                                    v8::Isolate* isolate) {
  if (isolate == nullptr) isolate = context->GetIsolate();

  using Limits = std::numeric_limits<T>;
  // Choose Uint32, Int32, or Double depending on range checks.
  // These checks should all collapse at compile time.
  if (static_cast<uint32_t>(Limits::max()) <=
          std::numeric_limits<uint32_t>::max() &&
      static_cast<uint32_t>(Limits::min()) >=
          std::numeric_limits<uint32_t>::min() && Limits::is_exact) {
    return v8::Integer::NewFromUnsigned(isolate, static_cast<uint32_t>(number));
  }

  if (static_cast<int32_t>(Limits::max()) <=
          std::numeric_limits<int32_t>::max() &&
      static_cast<int32_t>(Limits::min()) >=
          std::numeric_limits<int32_t>::min() && Limits::is_exact) {
    return v8::Integer::New(isolate, static_cast<int32_t>(number));
  }

  return v8::Number::New(isolate, static_cast<double>(number));
}

SlicedArguments::SlicedArguments(
    const v8::FunctionCallbackInfo<v8::Value>& args, size_t start) {
  const size_t length = static_cast<size_t>(args.Length());
  if (start >= length) return;
  const size_t size = length - start;

  AllocateSufficientStorage(size);
  for (size_t i = 0; i < size; ++i)
    (*this)[i] = args[i + start];
}

template <typename T, size_t S>
ArrayBufferViewContents<T, S>::ArrayBufferViewContents(
    v8::Local<v8::Value> value) {
  CHECK(value->IsArrayBufferView());
  Read(value.As<v8::ArrayBufferView>());
}

template <typename T, size_t S>
ArrayBufferViewContents<T, S>::ArrayBufferViewContents(
    v8::Local<v8::ArrayBufferView> abv) {
  Read(abv);
}

template <typename T, size_t S>
void ArrayBufferViewContents<T, S>::Read(v8::Local<v8::ArrayBufferView> abv) {
  static_assert(sizeof(T) == 1, "Only supports one-byte data at the moment");
  length_ = abv->ByteLength();
  if (length_ > sizeof(stack_storage_) || abv->HasBuffer()) {
    data_ = static_cast<T*>(abv->Buffer()->GetContents().Data()) +
        abv->ByteOffset();
  } else {
    abv->CopyContents(stack_storage_, sizeof(stack_storage_));
    data_ = stack_storage_;
  }
}

}  // namespace node

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#endif  // SRC_UTIL_INL_H_
