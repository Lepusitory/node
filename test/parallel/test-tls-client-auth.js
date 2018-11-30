'use strict';

require('../common');
const fixtures = require('../common/fixtures');

const {
  assert, connect, keys
} = require(fixtures.path('tls-connect'));

// Use ec10 and agent10, they are the only identities with intermediate CAs.
const client = keys.ec10;
const server = keys.agent10;

// The certificates aren't for "localhost", so override the identity check.
function checkServerIdentity(hostname, cert) {
  assert.strictEqual(hostname, 'localhost');
  assert.strictEqual(cert.subject.CN, 'agent10.example.com');
}

// Split out the single end-entity cert and the subordinate CA for later use.
split(client.cert, client);
split(server.cert, server);

function split(file, into) {
  const certs = /([^]*END CERTIFICATE-----\r?\n)(-----BEGIN[^]*)/.exec(file);
  assert.strictEqual(certs.length, 3);
  into.single = certs[1];
  into.subca = certs[2];
}

// Typical setup, nothing special, complete cert chains sent to peer.
connect({
  client: {
    key: client.key,
    cert: client.cert,
    ca: server.ca,
    checkServerIdentity,
  },
  server: {
    key: server.key,
    cert: server.cert,
    ca: client.ca,
    requestCert: true,
  },
}, function(err, pair, cleanup) {
  assert.ifError(err);
  return cleanup();
});

// As above, but without requesting client's cert.
connect({
  client: {
    ca: server.ca,
    checkServerIdentity,
  },
  server: {
    key: server.key,
    cert: server.cert,
    ca: client.ca,
  },
}, function(err, pair, cleanup) {
  assert.ifError(err);
  return cleanup();
});

// Request cert from client that doesn't have one.
connect({
  client: {
    ca: server.ca,
    checkServerIdentity,
  },
  server: {
    key: server.key,
    cert: server.cert,
    ca: client.ca,
    requestCert: true,
  },
}, function(err, pair, cleanup) {
  assert.strictEqual(err.code, 'ECONNRESET');
  return cleanup();
});

// Typical configuration error, incomplete cert chains sent, we have to know the
// peer's subordinate CAs in order to verify the peer.
connect({
  client: {
    key: client.key,
    cert: client.single,
    ca: [server.ca, server.subca],
    checkServerIdentity,
  },
  server: {
    key: server.key,
    cert: server.single,
    ca: [client.ca, client.subca],
    requestCert: true,
  },
}, function(err, pair, cleanup) {
  assert.ifError(err);
  return cleanup();
});

// Like above, but provide root CA and subordinate CA as multi-PEM.
connect({
  client: {
    key: client.key,
    cert: client.single,
    ca: server.ca + '\n' + server.subca,
    checkServerIdentity,
  },
  server: {
    key: server.key,
    cert: server.single,
    ca: client.ca + '\n' + client.subca,
    requestCert: true,
  },
}, function(err, pair, cleanup) {
  assert.ifError(err);
  return cleanup();
});

// Like above, but provide multi-PEM in an array.
connect({
  client: {
    key: client.key,
    cert: client.single,
    ca: [server.ca + '\n' + server.subca],
    checkServerIdentity,
  },
  server: {
    key: server.key,
    cert: server.single,
    ca: [client.ca + '\n' + client.subca],
    requestCert: true,
  },
}, function(err, pair, cleanup) {
  assert.ifError(err);
  return cleanup();
});

// Fail to complete server's chain
connect({
  client: {
    ca: server.ca,
    checkServerIdentity,
  },
  server: {
    key: server.key,
    cert: server.single,
  },
}, function(err, pair, cleanup) {
  assert.strictEqual(err.code, 'UNABLE_TO_VERIFY_LEAF_SIGNATURE');
  return cleanup();
});

// Fail to complete client's chain.
connect({
  client: {
    key: client.key,
    cert: client.single,
    ca: server.ca,
    checkServerIdentity,
  },
  server: {
    key: server.key,
    cert: server.cert,
    ca: client.ca,
    requestCert: true,
  },
}, function(err, pair, cleanup) {
  assert.ifError(pair.client.error);
  assert.ifError(pair.server.error);
  assert.strictEqual(err.code, 'ECONNRESET');
  return cleanup();
});

// Fail to find CA for server.
connect({
  client: {
    checkServerIdentity,
  },
  server: {
    key: server.key,
    cert: server.cert,
  },
}, function(err, pair, cleanup) {
  assert.strictEqual(err.code, 'UNABLE_TO_GET_ISSUER_CERT_LOCALLY');
  return cleanup();
});

// Server sent their CA, but CA cannot be trusted if it is not locally known.
connect({
  client: {
    checkServerIdentity,
  },
  server: {
    key: server.key,
    cert: server.cert + '\n' + server.ca,
  },
}, function(err, pair, cleanup) {
  assert.strictEqual(err.code, 'SELF_SIGNED_CERT_IN_CHAIN');
  return cleanup();
});

// Server sent their CA, wrongly, but its OK since we know the CA locally.
connect({
  client: {
    checkServerIdentity,
    ca: server.ca,
  },
  server: {
    key: server.key,
    cert: server.cert + '\n' + server.ca,
  },
}, function(err, pair, cleanup) {
  assert.ifError(err);
  return cleanup();
});

// Fail to complete client's chain.
connect({
  client: {
    key: client.key,
    cert: client.single,
    ca: server.ca,
    checkServerIdentity,
  },
  server: {
    key: server.key,
    cert: server.cert,
    ca: client.ca,
    requestCert: true,
  },
}, function(err, pair, cleanup) {
  assert.strictEqual(err.code, 'ECONNRESET');
  return cleanup();
});

// Fail to find CA for client.
connect({
  client: {
    key: client.key,
    cert: client.cert,
    ca: server.ca,
    checkServerIdentity,
  },
  server: {
    key: server.key,
    cert: server.cert,
    requestCert: true,
  },
}, function(err, pair, cleanup) {
  assert.strictEqual(err.code, 'ECONNRESET');
  return cleanup();
});

// Confirm support for "BEGIN TRUSTED CERTIFICATE".
connect({
  client: {
    key: client.key,
    cert: client.cert,
    ca: server.ca.replace(/CERTIFICATE/g, 'TRUSTED CERTIFICATE'),
    checkServerIdentity,
  },
  server: {
    key: server.key,
    cert: server.cert,
    ca: client.ca,
    requestCert: true,
  },
}, function(err, pair, cleanup) {
  assert.ifError(err);
  return cleanup();
});

// Confirm support for "BEGIN TRUSTED CERTIFICATE".
connect({
  client: {
    key: client.key,
    cert: client.cert,
    ca: server.ca,
    checkServerIdentity,
  },
  server: {
    key: server.key,
    cert: server.cert,
    ca: client.ca.replace(/CERTIFICATE/g, 'TRUSTED CERTIFICATE'),
    requestCert: true,
  },
}, function(err, pair, cleanup) {
  assert.ifError(err);
  return cleanup();
});

// Confirm support for "BEGIN X509 CERTIFICATE".
connect({
  client: {
    key: client.key,
    cert: client.cert,
    ca: server.ca.replace(/CERTIFICATE/g, 'X509 CERTIFICATE'),
    checkServerIdentity,
  },
  server: {
    key: server.key,
    cert: server.cert,
    ca: client.ca,
    requestCert: true,
  },
}, function(err, pair, cleanup) {
  assert.ifError(err);
  return cleanup();
});

// Confirm support for "BEGIN X509 CERTIFICATE".
connect({
  client: {
    key: client.key,
    cert: client.cert,
    ca: server.ca,
    checkServerIdentity,
  },
  server: {
    key: server.key,
    cert: server.cert,
    ca: client.ca.replace(/CERTIFICATE/g, 'X509 CERTIFICATE'),
    requestCert: true,
  },
}, function(err, pair, cleanup) {
  assert.ifError(err);
  return cleanup();
});
