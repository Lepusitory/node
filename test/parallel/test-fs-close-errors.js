'use strict';

// This tests that the errors thrown from fs.close and fs.closeSync
// include the desired properties

require('../common');
const assert = require('assert');
const fs = require('fs');

['', false, null, undefined, {}, []].forEach((input) => {
  const errObj = {
    code: 'ERR_INVALID_ARG_TYPE',
    name: 'TypeError',
    message: 'The "fd" argument must be of type number. ' +
             `Received type ${typeof input}`
  };
  assert.throws(() => fs.close(input), errObj);
  assert.throws(() => fs.closeSync(input), errObj);
});

{
  // Test error when cb is not a function
  const fd = fs.openSync(__filename, 'r');

  const errObj = {
    code: 'ERR_INVALID_CALLBACK',
    name: 'TypeError'
  };

  ['', false, null, {}, []].forEach((input) => {
    assert.throws(() => fs.close(fd, input), errObj);
  });

  fs.closeSync(fd);
}
