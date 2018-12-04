// Copyright 2018 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Flags: --allow-natives-syntax --opt

// Test that NumberMin properly passes the kIdentifyZeros truncation.
(function() {
  function foo(x) {
    if (Math.min(x * -2, -1) == -2) return 0;
    return 1;
  }

  assertEquals(0, foo(1));
  assertEquals(1, foo(2));
  %OptimizeFunctionOnNextCall(foo);
  assertEquals(0, foo(1));
  assertEquals(1, foo(2));
  assertOptimized(foo);
  // Now `foo` should stay optimized even if `x * -2` would produce `-0`.
  assertEquals(1, foo(0));
  assertOptimized(foo);
})();
