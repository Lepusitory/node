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

if (!process.versions.openssl) {
  console.error('Skipping because node compiled without OpenSSL.');
  process.exit(0);
}

var common = require('../common');
var assert = require('assert');
var tls = require('tls');
var fs = require('fs');

var options = {
  key: [
    fs.readFileSync(common.fixturesDir + '/keys/agent1-key.pem'),
    fs.readFileSync(common.fixturesDir + '/keys/ec-key.pem')
  ],
  cert: [
    fs.readFileSync(common.fixturesDir + '/keys/agent1-cert.pem'),
    fs.readFileSync(common.fixturesDir + '/keys/ec-cert.pem')
  ]
};

var ciphers = [];

var server = tls.createServer(options, function(conn) {
  conn.end('ok');
}).listen(common.PORT, function() {
  var ecdsa = tls.connect(common.PORT, {
    ciphers: 'ECDHE-ECDSA-AES256-GCM-SHA384',
    rejectUnauthorized: false
  }, function() {
    var rsa = tls.connect(common.PORT, {
      ciphers: 'ECDHE-RSA-AES256-GCM-SHA384',
      rejectUnauthorized: false
    }, function() {
      ecdsa.destroy();
      rsa.destroy();

      ciphers.push(ecdsa.getCipher());
      ciphers.push(rsa.getCipher());
      server.close();
    });
  });
});

process.on('exit', function() {
  assert.deepEqual(ciphers, [{
    name: 'ECDHE-ECDSA-AES256-GCM-SHA384',
    version: 'TLSv1/SSLv3'
  }, {
    name: 'ECDHE-RSA-AES256-GCM-SHA384',
    version: 'TLSv1/SSLv3'
  }]);
});
