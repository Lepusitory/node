'use strict';

const common = require('../common');
const assert = require('assert');

const a = Buffer.from([1, 2, 3, 4, 5, 6, 7, 8, 9, 0]);
const b = Buffer.from([5, 6, 7, 8, 9, 0, 1, 2, 3, 4]);

assert.strictEqual(-1, a.compare(b));

// Equivalent to a.compare(b).
assert.strictEqual(-1, a.compare(b, 0));
assert.strictEqual(-1, a.compare(b, '0'));
assert.strictEqual(-1, a.compare(b, undefined));

// Equivalent to a.compare(b).
assert.strictEqual(-1, a.compare(b, 0, undefined, 0));

// Zero-length target, return 1
assert.strictEqual(1, a.compare(b, 0, 0, 0));
assert.strictEqual(1, a.compare(b, '0', '0', '0'));

// Equivalent to Buffer.compare(a, b.slice(6, 10))
assert.strictEqual(1, a.compare(b, 6, 10));

// Zero-length source, return -1
assert.strictEqual(-1, a.compare(b, 6, 10, 0, 0));

// Zero-length source and target, return 0
assert.strictEqual(0, a.compare(b, 0, 0, 0, 0));
assert.strictEqual(0, a.compare(b, 1, 1, 2, 2));

// Equivalent to Buffer.compare(a.slice(4), b.slice(0, 5))
assert.strictEqual(1, a.compare(b, 0, 5, 4));

// Equivalent to Buffer.compare(a.slice(1), b.slice(5))
assert.strictEqual(1, a.compare(b, 5, undefined, 1));

// Equivalent to Buffer.compare(a.slice(2), b.slice(2, 4))
assert.strictEqual(-1, a.compare(b, 2, 4, 2));

// Equivalent to Buffer.compare(a.slice(4), b.slice(0, 7))
assert.strictEqual(-1, a.compare(b, 0, 7, 4));

// Equivalent to Buffer.compare(a.slice(4, 6), b.slice(0, 7));
assert.strictEqual(-1, a.compare(b, 0, 7, 4, 6));

// zero length target
assert.strictEqual(1, a.compare(b, 0, null));

// coerces to targetEnd == 5
assert.strictEqual(-1, a.compare(b, 0, {valueOf: () => 5}));

// zero length target
assert.strictEqual(1, a.compare(b, Infinity, -Infinity));

// zero length target because default for targetEnd <= targetSource
assert.strictEqual(1, a.compare(b, '0xff'));

const oor = common.expectsError({code: 'ERR_INDEX_OUT_OF_RANGE'}, 7);

assert.throws(() => a.compare(b, 0, 100, 0), oor);
assert.throws(() => a.compare(b, 0, 1, 0, 100), oor);
assert.throws(() => a.compare(b, -1), oor);
assert.throws(() => a.compare(b, 0, '0xff'), oor);
assert.throws(() => a.compare(b, 0, Infinity), oor);
assert.throws(() => a.compare(b, 0, 1, -1), oor);
assert.throws(() => a.compare(b, -Infinity, Infinity), oor);
assert.throws(() => a.compare(), common.expectsError({
    code: 'ERR_INVALID_ARG_TYPE',
    type: TypeError,
    message: 'The "target" argument must be one of type buffer or uint8Array'
}));
