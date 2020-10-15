// Copyright 2020 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

function foo() {
  try {
    foo();
  } catch {
    print('Stack overflow');
    Worker('string', new Proxy([], {}));
  }
}
try {
  foo();
} catch {
  // expecting stack overflow, but we should not crash.
}
