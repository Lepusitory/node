// Copyright 2020 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Flags: --allow-natives-syntax

function foo() {
  return { ['bar']: class {} };
}
%PrepareFunctionForOptimization(foo);
assertEquals('bar', foo().bar.name);
assertEquals('bar', foo().bar.name);
%OptimizeFunctionOnNextCall(foo);
assertEquals('bar', foo().bar.name);
