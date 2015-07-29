'use strict';
var common = require('../common');
var assert = require('assert');
var http = require('http');
var net = require('net');
var PORT = common.PORT;
var spawn = require('child_process').spawn;

if (common.isWindows) {
  console.log('1..0 # Skipped: This test is disabled on windows.');
  return;
}

switch (process.argv[2]) {
  case 'child': return child();
  case 'parent': return parent();
  default: return test();
}

// spawn the parent, and listen for it to tell us the pid of the child.
// WARNING: This is an example of listening on some arbitrary FD number
// that has already been bound elsewhere in advance.  However, binding
// server handles to stdio fd's is NOT a good or reliable way to do
// concurrency in HTTP servers!  Use the cluster module, or if you want
// a more low-level approach, use child process IPC manually.
function test() {
  var parent = spawn(process.execPath, [__filename, 'parent'], {
    stdio: [ 0, 'pipe', 2 ]
  });
  var json = '';
  parent.stdout.on('data', function(c) {
    json += c.toString();
    if (json.indexOf('\n') !== -1) next();
  });
  function next() {
    console.error('output from parent = %s', json);
    var child = JSON.parse(json);
    // now make sure that we can request to the child, then kill it.
    http.get({
      server: 'localhost',
      port: PORT,
      path: '/',
    }).on('response', function(res) {
      var s = '';
      res.on('data', function(c) {
        s += c.toString();
      });
      res.on('end', function() {
        // kill the child before we start doing asserts.
        // it's really annoying when tests leave orphans!
        process.kill(child.pid, 'SIGKILL');
        try {
          parent.kill();
        } catch (e) {}

        assert.equal(s, 'hello from child\n');
        assert.equal(res.statusCode, 200);
      });
    });
  }
}

function child() {
  // start a server on fd=3
  http.createServer(function(req, res) {
    console.error('request on child');
    console.error('%s %s', req.method, req.url, req.headers);
    res.end('hello from child\n');
  }).listen({ fd: 3 }, function() {
    console.error('child listening on fd=3');
  });
}

function parent() {
  var server = net.createServer(function(conn) {
    console.error('connection on parent');
    conn.end('hello from parent\n');
  }).listen(PORT, function() {
    console.error('server listening on %d', PORT);

    var spawn = require('child_process').spawn;
    var child = spawn(process.execPath, [__filename, 'child'], {
      stdio: [ 0, 1, 2, server._handle ]
    });

    console.log('%j\n', { pid: child.pid });

    // Now close the parent, so that the child is the only thing
    // referencing that handle.  Note that connections will still
    // be accepted, because the child has the fd open.
    server.close();

    child.on('exit', function(code) {
      console.error('child exited', code);
    });

    child.on('close', function() {
      console.error('child closed');
    });
    console.error('child spawned');
  });
}
