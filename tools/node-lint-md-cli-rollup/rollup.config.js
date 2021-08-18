'use strict';

const { nodeResolve } = require('@rollup/plugin-node-resolve');
const commonjs = require('@rollup/plugin-commonjs');
const json = require('@rollup/plugin-json');

module.exports = {
  input: 'src/cli-entry.mjs',
  output: {
    file: 'dist/index.js',
    format: 'cjs',
    sourcemap: false,
    exports: 'none',
  },
  external: [
    'stream',
    'path',
    'module',
    'util',
    'tty',
    'os',
    'fs',
    'fsevents',
    'events',
    'assert',
  ],
  plugins: [
    {
      name: 'brute-replace',
      transform(code, id) {
        const normID = id.replace(__dirname, '').replace(/\\+/g, '/');
        if (normID === '/node_modules/concat-stream/index.js') {
          return code.replace('\'readable-stream\'', '\'stream\'');
        }
        if (normID === '/node_modules/unified-args/lib/options.js') {
          return code.replace('\'./schema\'', '\'./schema.json\'');
        }
        if (normID === '/node_modules/chokidar/lib/fsevents-handler.js') {
          return code.replace(
            'fsevents = require(\'fsevents\');', 'fsevents = undefined;'
          );
        }
        // Remove circular dependency in glob that messes up rollup
        return code.replace("var Glob = require('./glob.js').Glob", '');
      }
    },
    json({
      preferConst: true
    }),
    nodeResolve(), // tells Rollup how to find date-fns in node_modules
    commonjs(),
    {
      name: 'banner',
      renderChunk(code) {
        const banner = '// Don\'t change this file manually,\n' +
          '// it is generated from tools/node-lint-md-cli-rollup';
        return code.replace('\'use strict\';', '\'use strict\';\n\n' + banner);
      }
    },
  ]
};
