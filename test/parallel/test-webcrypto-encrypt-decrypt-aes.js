'use strict';

const common = require('../common');

if (!common.hasCrypto)
  common.skip('missing crypto');

const assert = require('assert');
const { subtle } = require('crypto').webcrypto;

async function testEncrypt({ keyBuffer, algorithm, plaintext, result }) {
  const key = await subtle.importKey(
    'raw',
    keyBuffer,
    { name: algorithm.name },
    false,
    ['encrypt', 'decrypt']);

  const output = await subtle.encrypt(algorithm, key, plaintext);
  plaintext[0] = 255 - plaintext[0];

  assert.strictEqual(
    Buffer.from(output).toString('hex'),
    Buffer.from(result).toString('hex'));

  const check = await subtle.decrypt(algorithm, key, output);
  output[0] = 255 - output[0];

  assert.strictEqual(
    Buffer.from(check).toString('hex'),
    Buffer.from(plaintext).toString('hex'));
}

async function testEncryptNoEncrypt({ keyBuffer, algorithm, plaintext }) {
  const key = await subtle.importKey(
    'raw',
    keyBuffer,
    { name: algorithm.name },
    false,
    ['decrypt']);

  return assert.rejects(subtle.encrypt(algorithm, key, plaintext), {
    message: /The requested operation is not valid for the provided key/
  });
}

async function testEncryptNoDecrypt({ keyBuffer, algorithm, plaintext }) {
  const key = await subtle.importKey(
    'raw',
    keyBuffer,
    { name: algorithm.name },
    false,
    ['encrypt']);

  const output = await subtle.encrypt(algorithm, key, plaintext);

  return assert.rejects(subtle.decrypt(algorithm, key, output), {
    message: /The requested operation is not valid for the provided key/
  });
}

async function testEncryptWrongAlg({ keyBuffer, algorithm, plaintext }, alg) {
  assert.notStrictEqual(algorithm.name, alg);
  const key = await subtle.importKey(
    'raw',
    keyBuffer,
    { name: alg },
    false,
    ['encrypt']);

  return assert.rejects(subtle.encrypt(algorithm, key, plaintext), {
    message: /The requested operation is not valid for the provided key/
  });
}

async function testDecrypt({ keyBuffer, algorithm, result }) {
  const key = await subtle.importKey(
    'raw',
    keyBuffer,
    { name: algorithm.name },
    false,
    ['encrypt', 'decrypt']);

  await subtle.decrypt(algorithm, key, result);
}

// Test aes-cbc vectors
{
  const {
    passing,
    failing,
    decryptionFailing
  } = require('../fixtures/crypto/aes_cbc')();

  (async function() {
    const variations = [];

    passing.forEach((vector) => {
      variations.push(testEncrypt(vector));
      variations.push(testEncryptNoEncrypt(vector));
      variations.push(testEncryptNoDecrypt(vector));
      variations.push(testEncryptWrongAlg(vector, 'AES-CTR'));
    });

    failing.forEach((vector) => {
      variations.push(assert.rejects(testEncrypt(vector), {
        message: /algorithm\.iv must contain exactly 16 bytes/
      }));
      variations.push(assert.rejects(testDecrypt(vector), {
        message: /algorithm\.iv must contain exactly 16 bytes/
      }));
    });

    decryptionFailing.forEach((vector) => {
      variations.push(assert.rejects(testDecrypt(vector), {
        message: /bad decrypt/
      }));
    });

    await Promise.all(variations);
  })().then(common.mustCall());
}

// Test aes-ctr vectors
{
  const {
    passing,
    failing,
    decryptionFailing
  } = require('../fixtures/crypto/aes_ctr')();

  (async function() {
    const variations = [];

    passing.forEach((vector) => {
      variations.push(testEncrypt(vector));
      variations.push(testEncryptNoEncrypt(vector));
      variations.push(testEncryptNoDecrypt(vector));
      variations.push(testEncryptWrongAlg(vector, 'AES-CBC'));
    });

    // TODO(@jasnell): These fail for different reasons. Need to
    // make them consistent
    failing.forEach((vector) => {
      variations.push(assert.rejects(testEncrypt(vector), {
        message: /.*/
      }));
      variations.push(assert.rejects(testDecrypt(vector), {
        message: /.*/
      }));
    });

    decryptionFailing.forEach((vector) => {
      variations.push(assert.rejects(testDecrypt(vector), {
        message: /bad decrypt/
      }));
    });

    await Promise.all(variations);
  })().then(common.mustCall());
}

// Test aes-gcm vectors
{
  const {
    passing,
    failing,
    decryptionFailing
  } = require('../fixtures/crypto/aes_gcm')();

  (async function() {
    const variations = [];

    passing.forEach((vector) => {
      variations.push(testEncrypt(vector));
      variations.push(testEncryptNoEncrypt(vector));
      variations.push(testEncryptNoDecrypt(vector));
      variations.push(testEncryptWrongAlg(vector, 'AES-CBC'));
    });

    failing.forEach((vector) => {
      variations.push(assert.rejects(testEncrypt(vector), {
        message: /is not a valid AES-GCM tag length/
      }));
      variations.push(assert.rejects(testDecrypt(vector), {
        message: /is not a valid AES-GCM tag length/
      }));
    });

    decryptionFailing.forEach((vector) => {
      variations.push(assert.rejects(testDecrypt(vector), {
        message: /bad decrypt/
      }));
    });

    await Promise.all(variations);
  })().then(common.mustCall());
}
