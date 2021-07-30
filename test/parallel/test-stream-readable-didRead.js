'use strict';
const common = require('../common');
const assert = require('assert');
const Readable = require('stream').Readable;

function noop() {}

function check(readable, data, fn) {
  assert.strictEqual(readable.readableDidRead, false);
  if (data === -1) {
    readable.on('error', common.mustCall());
    readable.on('data', common.mustNotCall());
    readable.on('end', common.mustNotCall());
  } else {
    readable.on('error', common.mustNotCall());
    if (data === -2) {
      readable.on('end', common.mustNotCall());
    } else {
      readable.on('end', common.mustCall());
    }
    if (data > 0) {
      readable.on('data', common.mustCallAtLeast(data));
    } else {
      readable.on('data', common.mustNotCall());
    }
  }
  readable.on('close', common.mustCall());
  fn();
  setImmediate(() => {
    assert.strictEqual(readable.readableDidRead, true);
  });
}

{
  const readable = new Readable({
    read() {
      this.push(null);
    }
  });
  check(readable, 0, () => {
    readable.read();
  });
}

{
  const readable = new Readable({
    read() {
      this.push(null);
    }
  });
  check(readable, 0, () => {
    readable.resume();
  });
}

{
  const readable = new Readable({
    read() {
      this.push(null);
    }
  });
  check(readable, -2, () => {
    readable.destroy();
  });
}

{
  const readable = new Readable({
    read() {
      this.push(null);
    }
  });

  check(readable, -1, () => {
    readable.destroy(new Error());
  });
}

{
  const readable = new Readable({
    read() {
      this.push('data');
      this.push(null);
    }
  });

  check(readable, 1, () => {
    readable.on('data', noop);
  });
}

{
  const readable = new Readable({
    read() {
      this.push('data');
      this.push(null);
    }
  });

  check(readable, 1, () => {
    readable.on('data', noop);
    readable.off('data', noop);
  });
}
