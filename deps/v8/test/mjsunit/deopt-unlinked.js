// Copyright 2017 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Flags: --allow-natives-syntax --opt --no-always-opt
// The deopt count is stored in the feedback vector which gets cleared when
// bytecode is flushed, which --gc-interval can cause in stress modes.
// Flags: --noflush-bytecode --nostress-flush-bytecode

function foo() {};
%PrepareFunctionForOptimization(foo);
foo();
foo();
%OptimizeFunctionOnNextCall(foo);
foo();

assertOptimized(foo);

// Unlink the function.
%DeoptimizeFunction(foo);

assertUnoptimized(foo);

foo();

assertUnoptimized(foo);
