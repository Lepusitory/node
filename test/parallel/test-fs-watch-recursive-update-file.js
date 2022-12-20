'use strict';

const common = require('../common');
const { setTimeout } = require('timers/promises');

if (common.isIBMi)
  common.skip('IBMi does not support `fs.watch()`');

// fs-watch on folders have limited capability in AIX.
// The testcase makes use of folder watching, and causes
// hang. This behavior is documented. Skip this for AIX.

if (common.isAIX)
  common.skip('folder watch capability is limited in AIX.');

const assert = require('assert');
const path = require('path');
const fs = require('fs');

const tmpdir = require('../common/tmpdir');
const testDir = tmpdir.path;
tmpdir.refresh();

(async () => {
  // Watch a folder and update an already existing file in it.

  const rootDirectory = fs.mkdtempSync(testDir + path.sep);
  const testDirectory = path.join(rootDirectory, 'test-0');
  fs.mkdirSync(testDirectory);

  const testFile = path.join(testDirectory, 'file-1.txt');
  fs.writeFileSync(testFile, 'hello');

  const watcher = fs.watch(testDirectory, { recursive: true });
  let watcherClosed = false;
  watcher.on('change', common.mustCallAtLeast(function(event, filename) {
    // Libuv inconsistenly emits a rename event for the file we are watching
    assert.ok(event === 'change' || event === 'rename');

    if (filename === path.basename(testFile)) {
      watcher.close();
      watcherClosed = true;
    }
  }));

  await setTimeout(common.platformTimeout(100));
  fs.writeFileSync(testFile, 'hello');

  process.once('exit', function() {
    assert(watcherClosed, 'watcher Object was not closed');
  });
})().then(common.mustCall());
