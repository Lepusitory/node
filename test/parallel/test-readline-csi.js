// Flags: --expose-internals
'use strict';

const common = require('../common');
const assert = require('assert');
const readline = require('readline');
const { Writable } = require('stream');
const { CSI } = require('internal/readline');

{
  assert(CSI);
  assert.strictEqual(CSI.kClearToBeginning, '\x1b[1K');
  assert.strictEqual(CSI.kClearToEnd, '\x1b[0K');
  assert.strictEqual(CSI.kClearLine, '\x1b[2K');
  assert.strictEqual(CSI.kClearScreenDown, '\x1b[0J');
  assert.strictEqual(CSI`1${2}3`, '\x1b[123');
}

class TestWritable extends Writable {
  constructor() {
    super();
    this.data = '';
  }
  _write(chunk, encoding, callback) {
    this.data += chunk.toString();
    callback();
  }
}

const writable = new TestWritable();

assert.strictEqual(readline.clearScreenDown(writable), true);
assert.deepStrictEqual(writable.data, CSI.kClearScreenDown);
assert.strictEqual(readline.clearScreenDown(writable, common.mustCall()), true);

// Verify that clearScreenDown() throws on invalid callback.
assert.throws(() => {
  readline.clearScreenDown(writable, null);
}, /ERR_INVALID_CALLBACK/);

// Verify that clearScreenDown() does not throw on null or undefined stream.
assert.strictEqual(readline.clearScreenDown(null, common.mustCall()), true);
assert.strictEqual(readline.clearScreenDown(undefined, common.mustCall()),
                   true);

writable.data = '';
assert.strictEqual(readline.clearLine(writable, -1), true);
assert.deepStrictEqual(writable.data, CSI.kClearToBeginning);

writable.data = '';
assert.strictEqual(readline.clearLine(writable, 1), true);
assert.deepStrictEqual(writable.data, CSI.kClearToEnd);

writable.data = '';
assert.strictEqual(readline.clearLine(writable, 0), true);
assert.deepStrictEqual(writable.data, CSI.kClearLine);

writable.data = '';
assert.strictEqual(readline.clearLine(writable, -1, common.mustCall()), true);
assert.deepStrictEqual(writable.data, CSI.kClearToBeginning);

// Verify that clearLine() throws on invalid callback.
assert.throws(() => {
  readline.clearLine(writable, 0, null);
}, /ERR_INVALID_CALLBACK/);

// Verify that clearLine() does not throw on null or undefined stream.
assert.strictEqual(readline.clearLine(null, 0), true);
assert.strictEqual(readline.clearLine(undefined, 0), true);
assert.strictEqual(readline.clearLine(null, 0, common.mustCall()), true);
assert.strictEqual(readline.clearLine(undefined, 0, common.mustCall()), true);

// Nothing is written when moveCursor 0, 0
[
  [0, 0, ''],
  [1, 0, '\x1b[1C'],
  [-1, 0, '\x1b[1D'],
  [0, 1, '\x1b[1B'],
  [0, -1, '\x1b[1A'],
  [1, 1, '\x1b[1C\x1b[1B'],
  [-1, 1, '\x1b[1D\x1b[1B'],
  [-1, -1, '\x1b[1D\x1b[1A'],
  [1, -1, '\x1b[1C\x1b[1A'],
].forEach((set) => {
  writable.data = '';
  assert.strictEqual(readline.moveCursor(writable, set[0], set[1]), true);
  assert.deepStrictEqual(writable.data, set[2]);
  writable.data = '';
  assert.strictEqual(
    readline.moveCursor(writable, set[0], set[1], common.mustCall()),
    true
  );
  assert.deepStrictEqual(writable.data, set[2]);
});

// Verify that moveCursor() throws on invalid callback.
assert.throws(() => {
  readline.moveCursor(writable, 1, 1, null);
}, /ERR_INVALID_CALLBACK/);

// Verify that moveCursor() does not throw on null or undefined stream.
assert.strictEqual(readline.moveCursor(null, 1, 1), true);
assert.strictEqual(readline.moveCursor(undefined, 1, 1), true);
assert.strictEqual(readline.moveCursor(null, 1, 1, common.mustCall()), true);
assert.strictEqual(readline.moveCursor(undefined, 1, 1, common.mustCall()),
                   true);

// Undefined or null as stream should not throw.
readline.cursorTo(null);
readline.cursorTo();

writable.data = '';
readline.cursorTo(writable, 'a');
assert.strictEqual(writable.data, '');

writable.data = '';
readline.cursorTo(writable, 'a', 'b');
assert.strictEqual(writable.data, '');

writable.data = '';
common.expectsError(
  () => readline.cursorTo(writable, 'a', 1),
  {
    type: TypeError,
    code: 'ERR_INVALID_CURSOR_POS',
    message: 'Cannot set cursor row without setting its column'
  });
assert.strictEqual(writable.data, '');

writable.data = '';
readline.cursorTo(writable, 1, 'a');
assert.strictEqual(writable.data, '\x1b[2G');

writable.data = '';
readline.cursorTo(writable, 1, 2);
assert.strictEqual(writable.data, '\x1b[3;2H');
