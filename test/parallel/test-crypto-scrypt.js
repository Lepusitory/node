'use strict';
const common = require('../common');
if (!common.hasCrypto)
  common.skip('missing crypto');

const assert = require('assert');
const crypto = require('crypto');

if (typeof process.binding('crypto').scrypt !== 'function')
  common.skip('no scrypt support');

const good = [
  // Zero-length key is legal, functions as a parameter validation check.
  {
    pass: '',
    salt: '',
    keylen: 0,
    N: 16,
    p: 1,
    r: 1,
    expected: '',
  },
  // Test vectors from https://tools.ietf.org/html/rfc7914#page-13 that
  // should pass.  Note that the test vector with N=1048576 is omitted
  // because it takes too long to complete and uses over 1 GB of memory.
  {
    pass: '',
    salt: '',
    keylen: 64,
    N: 16,
    p: 1,
    r: 1,
    expected:
        '77d6576238657b203b19ca42c18a0497f16b4844e3074ae8dfdffa3fede21442' +
        'fcd0069ded0948f8326a753a0fc81f17e8d3e0fb2e0d3628cf35e20c38d18906',
  },
  {
    pass: 'password',
    salt: 'NaCl',
    keylen: 64,
    N: 1024,
    p: 16,
    r: 8,
    expected:
        'fdbabe1c9d3472007856e7190d01e9fe7c6ad7cbc8237830e77376634b373162' +
        '2eaf30d92e22a3886ff109279d9830dac727afb94a83ee6d8360cbdfa2cc0640',
  },
  {
    pass: 'pleaseletmein',
    salt: 'SodiumChloride',
    keylen: 64,
    N: 16384,
    p: 1,
    r: 8,
    expected:
        '7023bdcb3afd7348461c06cd81fd38ebfda8fbba904f8e3ea9b543f6545da1f2' +
        'd5432955613f0fcf62d49705242a9af9e61e85dc0d651e40dfcf017b45575887',
  },
];

// Test vectors that should fail.
const bad = [
  { N: 1, p: 1, r: 1 },        // N < 2
  { N: 3, p: 1, r: 1 },        // Not power of 2.
  { N: 2 ** 16, p: 1, r: 1 },  // N >= 2**(r*16)
  { N: 2, p: 2 ** 30, r: 1 },  // p > (2**30-1)/r
];

// Test vectors where 128*N*r exceeds maxmem.
const toobig = [
  { N: 2 ** 20, p: 1, r: 8 },
  { N: 2 ** 10, p: 1, r: 8, maxmem: 2 ** 20 },
];

const badargs = [
  {
    args: [],
    expected: { code: 'ERR_INVALID_ARG_TYPE', message: /"password"/ },
  },
  {
    args: [null],
    expected: { code: 'ERR_INVALID_ARG_TYPE', message: /"password"/ },
  },
  {
    args: [''],
    expected: { code: 'ERR_INVALID_ARG_TYPE', message: /"salt"/ },
  },
  {
    args: ['', null],
    expected: { code: 'ERR_INVALID_ARG_TYPE', message: /"salt"/ },
  },
  {
    args: ['', ''],
    expected: { code: 'ERR_INVALID_ARG_TYPE', message: /"keylen"/ },
  },
  {
    args: ['', '', null],
    expected: { code: 'ERR_INVALID_ARG_TYPE', message: /"keylen"/ },
  },
  {
    args: ['', '', .42],
    expected: { code: 'ERR_OUT_OF_RANGE', message: /"keylen"/ },
  },
  {
    args: ['', '', -42],
    expected: { code: 'ERR_OUT_OF_RANGE', message: /"keylen"/ },
  },
];

for (const options of good) {
  const { pass, salt, keylen, expected } = options;
  const actual = crypto.scryptSync(pass, salt, keylen, options);
  assert.strictEqual(actual.toString('hex'), expected);
  crypto.scrypt(pass, salt, keylen, options, common.mustCall((err, actual) => {
    assert.ifError(err);
    assert.strictEqual(actual.toString('hex'), expected);
  }));
}

for (const options of bad) {
  const expected = {
    code: 'ERR_CRYPTO_SCRYPT_INVALID_PARAMETER',
    message: 'Invalid scrypt parameter',
    type: Error,
  };
  common.expectsError(() => crypto.scrypt('pass', 'salt', 1, options, () => {}),
                      expected);
  common.expectsError(() => crypto.scryptSync('pass', 'salt', 1, options),
                      expected);
}

for (const options of toobig) {
  const expected = {
    message: /error:[^:]+:digital envelope routines:EVP_PBE_scrypt:memory limit exceeded/,
    type: Error,
  };
  common.expectsError(() => crypto.scrypt('pass', 'salt', 1, options, () => {}),
                      expected);
  common.expectsError(() => crypto.scryptSync('pass', 'salt', 1, options),
                      expected);
}

{
  const defaults = { N: 16384, p: 1, r: 8 };
  const expected = crypto.scryptSync('pass', 'salt', 1, defaults);
  const actual = crypto.scryptSync('pass', 'salt', 1);
  assert.deepStrictEqual(actual.toString('hex'), expected.toString('hex'));
  crypto.scrypt('pass', 'salt', 1, common.mustCall((err, actual) => {
    assert.ifError(err);
    assert.deepStrictEqual(actual.toString('hex'), expected.toString('hex'));
  }));
}

for (const { args, expected } of badargs) {
  common.expectsError(() => crypto.scrypt(...args), expected);
  common.expectsError(() => crypto.scryptSync(...args), expected);
}

{
  const expected = { code: 'ERR_INVALID_CALLBACK' };
  common.expectsError(() => crypto.scrypt('', '', 42, null), expected);
  common.expectsError(() => crypto.scrypt('', '', 42, {}, null), expected);
  common.expectsError(() => crypto.scrypt('', '', 42, {}), expected);
  common.expectsError(() => crypto.scrypt('', '', 42, {}, {}), expected);
}
