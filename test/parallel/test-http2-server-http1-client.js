'use strict';

const common = require('../common');

if (!common.hasCrypto)
  common.skip('missing crypto');

const http = require('http');
const http2 = require('http2');

const server = http2.createServer();
server.on('stream', common.mustNotCall());
server.on('session', common.mustCall((session) => {
  session.on('close', common.mustCall());
  session.on('error', common.expectsError({
    code: 'ERR_HTTP2_ERROR',
    type: Error,
    message: 'Received bad client magic byte string'
  }));
}));

server.listen(0, common.mustCall(() => {
  const req = http.get(`http://localhost:${server.address().port}`);
  req.on('error', (error) => server.close());
}));
