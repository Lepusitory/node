'use strict';
const common = require('../common.js');
const fs = require('fs');
const zlib = require('zlib');

const bench = common.createBenchmark(main, {
  inputLen: [1024],
  duration: [5],
  type: ['string', 'buffer']
});

function main({ inputLen, duration, type }) {
  const buffer = Buffer.alloc(inputLen, fs.readFileSync(__filename));
  const chunk = type === 'buffer' ? buffer : buffer.toString('utf8');

  const input = zlib.createGzip();
  const output = zlib.createGunzip();

  let readFromOutput = 0;
  input.pipe(output);
  if (type === 'string')
    output.setEncoding('utf8');
  output.on('data', (chunk) => readFromOutput += chunk.length);

  function write() {
    input.write(chunk, write);
  }

  bench.start();
  write();

  setTimeout(() => {
    // Give result in GBit/s, like the net benchmarks do
    bench.end(readFromOutput * 8 / (1024 ** 3));

    // Cut off writing the easy way.
    input.write = () => {};
  }, duration * 1000);
}
