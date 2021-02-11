// Copyright 2020 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_HEAP_CPPGC_GLOBALS_H_
#define V8_HEAP_CPPGC_GLOBALS_H_

#include <stddef.h>
#include <stdint.h>

#include "include/cppgc/internal/gc-info.h"

namespace cppgc {
namespace internal {

using Address = uint8_t*;
using ConstAddress = const uint8_t*;

constexpr size_t kKB = 1024;
constexpr size_t kMB = kKB * 1024;
constexpr size_t kGB = kMB * 1024;

// AccessMode used for choosing between atomic and non-atomic accesses.
enum class AccessMode : uint8_t { kNonAtomic, kAtomic };

// See 6.7.6 (http://eel.is/c++draft/basic.align) for alignment restrictions. We
// do not fully support all alignment restrictions (following
// alignof(std​::​max_­align_­t)) but limit to alignof(double).
//
// This means that any scalar type with stricter alignment requirements (in
// practice: long double) cannot be used unrestricted in garbage-collected
// objects.
//
// Note: We use the same allocation granularity on 32-bit and 64-bit systems.
constexpr size_t kAllocationGranularity = 8;
constexpr size_t kAllocationMask = kAllocationGranularity - 1;

constexpr size_t kPageSizeLog2 = 17;
constexpr size_t kPageSize = 1 << kPageSizeLog2;
constexpr size_t kPageOffsetMask = kPageSize - 1;
constexpr size_t kPageBaseMask = ~kPageOffsetMask;

// Guard pages are always put into memory. Whether they are actually protected
// depends on the allocator provided to the garbage collector.
constexpr size_t kGuardPageSize = 4096;

constexpr size_t kLargeObjectSizeThreshold = kPageSize / 2;

constexpr GCInfoIndex kFreeListGCInfoIndex = 0;
constexpr size_t kFreeListEntrySize = 2 * sizeof(uintptr_t);

constexpr size_t kCagedHeapReservationSize = static_cast<size_t>(4) * kGB;
constexpr size_t kCagedHeapReservationAlignment = kCagedHeapReservationSize;

}  // namespace internal
}  // namespace cppgc

#endif  // V8_HEAP_CPPGC_GLOBALS_H_
