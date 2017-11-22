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

'use strict';
const common = require('../common');
const assert = require('assert');

// Requiring the domain module here changes the function that is used by node to
// call process.nextTick's callbacks to a variant that specifically handles
// domains. We want to test this specific variant in this test, and so even if
// the domain module is not used, this require call is needed and must not be
// removed.
require('domain');

common.crashOnUnhandledRejection();

function enqueueMicrotask(fn) {
  Promise.resolve().then(fn);
}

let done = 0;

process.on('exit', function() {
  assert.strictEqual(done, 2);
});

// no nextTick, microtask
setTimeout(function() {
  enqueueMicrotask(function() {
    done++;
  });
}, 0);


// no nextTick, microtask with nextTick
setTimeout(function() {
  let called = false;

  enqueueMicrotask(function() {
    process.nextTick(function() {
      called = true;
    });
  });

  setTimeout(function() {
    if (called)
      done++;
  }, 0);

}, 0);
