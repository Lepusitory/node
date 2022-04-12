// Copyright 2021 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_OBJECTS_JS_SHADOW_REALMS_INL_H_
#define V8_OBJECTS_JS_SHADOW_REALMS_INL_H_

#include "src/api/api-inl.h"
#include "src/heap/heap-write-barrier-inl.h"
#include "src/objects/js-shadow-realms.h"
#include "src/objects/smi-inl.h"

// Has to be the last include (doesn't have include guards):
#include "src/objects/object-macros.h"

namespace v8 {
namespace internal {

#include "torque-generated/src/objects/js-shadow-realms-tq-inl.inc"

TQ_OBJECT_CONSTRUCTORS_IMPL(JSShadowRealm)

}  // namespace internal
}  // namespace v8

#include "src/objects/object-macros-undef.h"

#endif  // V8_OBJECTS_JS_SHADOW_REALMS_INL_H_
