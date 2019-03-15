// Copyright 2018 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_INTL_SUPPORT
#error Internationalization is expected to be enabled.
#endif  // V8_INTL_SUPPORT

#ifndef V8_OBJECTS_JS_LIST_FORMAT_INL_H_
#define V8_OBJECTS_JS_LIST_FORMAT_INL_H_

#include "src/objects-inl.h"
#include "src/objects/js-list-format.h"

// Has to be the last include (doesn't have include guards):
#include "src/objects/object-macros.h"

namespace v8 {
namespace internal {

OBJECT_CONSTRUCTORS_IMPL(JSListFormat, JSObject)

// Base list format accessors.
ACCESSORS(JSListFormat, locale, String, kLocaleOffset)
ACCESSORS(JSListFormat, icu_formatter, Managed<icu::ListFormatter>,
          kICUFormatterOffset)
SMI_ACCESSORS(JSListFormat, flags, kFlagsOffset)

inline void JSListFormat::set_style(Style style) {
  DCHECK_GT(Style::COUNT, style);
  int hints = flags();
  hints = StyleBits::update(hints, style);
  set_flags(hints);
}

inline JSListFormat::Style JSListFormat::style() const {
  return StyleBits::decode(flags());
}

inline void JSListFormat::set_type(Type type) {
  DCHECK_GT(Type::COUNT, type);
  int hints = flags();
  hints = TypeBits::update(hints, type);
  set_flags(hints);
}

inline JSListFormat::Type JSListFormat::type() const {
  return TypeBits::decode(flags());
}

CAST_ACCESSOR(JSListFormat)

}  // namespace internal
}  // namespace v8

#include "src/objects/object-macros-undef.h"

#endif  // V8_OBJECTS_JS_LIST_FORMAT_INL_H_
