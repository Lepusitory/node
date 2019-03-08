// Flags: --experimental-report
'use strict';

// Test producing a report via API call, using the no-hooks/no-signal interface.
const common = require('../common');
common.skipIfReportDisabled();
const assert = require('assert');
const { spawnSync } = require('child_process');
const fs = require('fs');
const path = require('path');
const helper = require('../common/report');
const tmpdir = require('../common/tmpdir');

common.expectWarning('ExperimentalWarning',
                     'report is an experimental feature. This feature could ' +
                     'change at any time');
tmpdir.refresh();
process.report.directory = tmpdir.path;

function validate() {
  const reports = helper.findReports(process.pid, tmpdir.path);
  assert.strictEqual(reports.length, 1);
  helper.validate(reports[0]);
  fs.unlinkSync(reports[0]);
  return reports[0];
}

{
  // Test with no arguments.
  process.report.triggerReport();
  validate();
}

{
  // Test with an error argument.
  process.report.triggerReport(new Error('test error'));
  validate();
}

{
  // Test with a file argument.
  const file = process.report.triggerReport('custom-name-1.json');
  const absolutePath = path.join(tmpdir.path, file);
  assert.strictEqual(helper.findReports(process.pid, tmpdir.path).length, 0);
  assert.strictEqual(file, 'custom-name-1.json');
  helper.validate(absolutePath);
  fs.unlinkSync(absolutePath);
}

{
  // Test with file and error arguments.
  const file = process.report.triggerReport('custom-name-2.json',
                                            new Error('test error'));
  const absolutePath = path.join(tmpdir.path, file);
  assert.strictEqual(helper.findReports(process.pid, tmpdir.path).length, 0);
  assert.strictEqual(file, 'custom-name-2.json');
  helper.validate(absolutePath);
  fs.unlinkSync(absolutePath);
}

{
  // Test with a filename option.
  process.report.filename = 'custom-name-3.json';
  const file = process.report.triggerReport();
  assert.strictEqual(helper.findReports(process.pid, tmpdir.path).length, 0);
  const filename = path.join(process.report.directory, 'custom-name-3.json');
  assert.strictEqual(file, process.report.filename);
  helper.validate(filename);
  fs.unlinkSync(filename);
}

// Test with an invalid file argument.
[null, 1, Symbol(), function() {}].forEach((file) => {
  common.expectsError(() => {
    process.report.triggerReport(file);
  }, { code: 'ERR_INVALID_ARG_TYPE' });
});

// Test with an invalid error argument.
[null, 1, Symbol(), function() {}, 'foo'].forEach((error) => {
  common.expectsError(() => {
    process.report.triggerReport('file', error);
  }, { code: 'ERR_INVALID_ARG_TYPE' });
});

{
  // Test the special "stdout" filename.
  const args = ['--experimental-report', '-e',
                'process.report.triggerReport("stdout")'];
  const child = spawnSync(process.execPath, args, { cwd: tmpdir.path });
  assert.strictEqual(child.status, 0);
  assert.strictEqual(child.signal, null);
  assert.strictEqual(helper.findReports(child.pid, tmpdir.path).length, 0);
  helper.validateContent(child.stdout.toString());
}

{
  // Test the special "stderr" filename.
  const args = ['--experimental-report', '-e',
                'process.report.triggerReport("stderr")'];
  const child = spawnSync(process.execPath, args, { cwd: tmpdir.path });
  assert.strictEqual(child.status, 0);
  assert.strictEqual(child.signal, null);
  assert.strictEqual(child.stdout.toString().trim(), '');
  assert.strictEqual(helper.findReports(child.pid, tmpdir.path).length, 0);
  const report = child.stderr.toString().split('Node.js report completed')[0];
  helper.validateContent(report);
}
