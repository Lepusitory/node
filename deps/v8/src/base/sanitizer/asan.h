// Copyright 2018 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// AddressSanitizer support.

#ifndef V8_BASE_SANITIZER_ASAN_H_
#define V8_BASE_SANITIZER_ASAN_H_

#include <type_traits>

#include "src/base/macros.h"

#ifdef V8_USE_ADDRESS_SANITIZER

#include <sanitizer/asan_interface.h>

#if !defined(ASAN_POISON_MEMORY_REGION) || !defined(ASAN_UNPOISON_MEMORY_REGION)
#error \
    "ASAN_POISON_MEMORY_REGION and ASAN_UNPOISON_MEMORY_REGION must be defined"
#endif

#define DISABLE_ASAN __attribute__((no_sanitize_address))

// Check that all bytes in a memory region are poisoned. This is different from
// `__asan_region_is_poisoned()` which only requires a single byte in the region
// to be poisoned.
#define ASAN_CHECK_MEMORY_REGION_IS_POISONED(start, size)                     \
  do {                                                                        \
    for (size_t i = 0; i < size; i++) {                                       \
      CHECK(__asan_address_is_poisoned(reinterpret_cast<const char*>(start) + \
                                       i));                                   \
    }                                                                         \
  } while (0)

#else  // !V8_USE_ADDRESS_SANITIZER

#define DISABLE_ASAN

#define ASAN_POISON_MEMORY_REGION(start, size)                      \
  static_assert(std::is_pointer<decltype(start)>::value,            \
                "static type violation");                           \
  static_assert(std::is_convertible<decltype(size), size_t>::value, \
                "static type violation");                           \
  USE(start, size)

#define ASAN_UNPOISON_MEMORY_REGION(start, size) \
  ASAN_POISON_MEMORY_REGION(start, size)

#define ASAN_CHECK_MEMORY_REGION_IS_POISONED(start, size) \
  ASAN_POISON_MEMORY_REGION(start, size)

#endif  // !V8_USE_ADDRESS_SANITIZER

#endif  // V8_BASE_SANITIZER_ASAN_H_
