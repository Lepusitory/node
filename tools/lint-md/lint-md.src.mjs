import fs from 'fs';

import { unified } from 'unified';
import remarkParse from 'remark-parse';
import remarkStringify from 'remark-stringify';
import presetLintNode from 'remark-preset-lint-node';
import { read } from 'to-vfile';
import { reporter } from 'vfile-reporter';

const paths = process.argv.slice(2);

if (!paths.length) {
  console.error('Usage: lint-md.mjs <path> [<path> ...]');
  process.exit(1);
}

let format = false;

if (paths[0] === '--format') {
  paths.shift();
  format = true;
}

const linter = unified()
  .use(remarkParse)
  .use(presetLintNode)
  .use(remarkStringify);

paths.forEach(async (path) => {
  const file = await read(path);
  // We need to calculate `fileContents` before running `linter.process(files)`
  // because `linter.process(files)` mutates `file` and returns it as `result`.
  // So we won't be able to use `file` after that to see if its contents have
  // changed as they will have been altered to the changed version.
  const fileContents = file.toString();
  const result = await linter.process(file);
  const isDifferent = fileContents !== result.toString();
  if (format) {
    if (isDifferent) {
      fs.writeFileSync(path, result.toString());
    }
  } else {
    if (isDifferent) {
      process.exitCode = 1;
      const cmd = process.platform === 'win32' ? 'vcbuild' : 'make';
      console.error(`${path} is not formatted. Please run '${cmd} format-md'.`);
    }
    if (result.messages.length) {
      process.exitCode = 1;
      console.error(reporter(result));
    }
  }
});
