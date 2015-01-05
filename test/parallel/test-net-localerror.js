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

var common = require('../common');
var assert = require('assert');
var net = require('net');

  connect({
    host: 'localhost',
    port: common.PORT,
    localPort: 'foobar',
  }, 'localPort should be a number: foobar');

  connect({
    host: 'localhost',
    port: common.PORT,
    localAddress: 'foobar',
  }, 'localAddress should be a valid IP: foobar');

  connect({
    host: 'localhost',
    port: 65536
  }, 'port should be > 0 and < 65536: 65536');

  connect({
    host: 'localhost',
    port: 0
  }, 'port should be > 0 and < 65536: 0');

function connect(opts, msg) {
  assert.throws(function() {
    var client = net.connect(opts);
  }, msg);
}
