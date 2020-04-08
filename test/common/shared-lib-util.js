'use strict';
const common = require('../common');
const path = require('path');

const kNodeShared = Boolean(process.config.variables.node_shared);
const kShlibSuffix = process.config.variables.shlib_suffix;
const kExecPath = path.dirname(process.execPath);

// If node executable is linked to shared lib, need to take care about the
// shared lib path.
function addLibraryPath(env) {
  if (!kNodeShared) {
    return;
  }

  env = env || process.env;

  env.LD_LIBRARY_PATH =
    (env.LD_LIBRARY_PATH ? env.LD_LIBRARY_PATH + path.delimiter : '') +
    path.join(kExecPath, 'lib.target');
  // For AIX.
  env.LIBPATH =
    (env.LIBPATH ? env.LIBPATH + path.delimiter : '') +
    path.join(kExecPath, 'lib.target');
  // For Mac OSX.
  env.DYLD_LIBRARY_PATH =
    (env.DYLD_LIBRARY_PATH ? env.DYLD_LIBRARY_PATH + path.delimiter : '') +
    kExecPath;
  // For Windows.
  env.PATH = (env.PATH ? env.PATH + path.delimiter : '') + kExecPath;
}

// Get the full path of shared lib.
function getSharedLibPath() {
  if (common.isWindows) {
    return path.join(kExecPath, 'node.dll');
  } else if (common.isOSX) {
    return path.join(kExecPath, `libnode.${kShlibSuffix}`);
  }
  return path.join(kExecPath, 'lib.target', `libnode.${kShlibSuffix}`);
}

// Get the binary path of stack frames.
function getBinaryPath() {
  return kNodeShared ? getSharedLibPath() : process.execPath;
}

module.exports = {
  addLibraryPath,
  getBinaryPath,
  getSharedLibPath
};
