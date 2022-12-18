/**
 * @fileoverview Prohibit the use of assert operators ( ===, !==, ==, != )
 */

'use strict';

const astSelector = 'ExpressionStatement[expression.type="CallExpression"]' +
                    '[expression.callee.name="assert"]' +
                    '[expression.arguments.0.type="BinaryExpression"]';

function parseError(method, op) {
  return `'assert.${method}' should be used instead of '${op}'`;
}

const preferredAssertMethod = {
  '===': 'strictEqual',
  '!==': 'notStrictEqual',
  '==': 'equal',
  '!=': 'notEqual',
};

module.exports = function(context) {
  return {
    [astSelector]: function(node) {
      const arg = node.expression.arguments[0];
      const assertMethod = preferredAssertMethod[arg.operator];
      if (assertMethod) {
        context.report({
          node,
          message: parseError(assertMethod, arg.operator),
          fix: (fixer) => {
            const sourceCode = context.getSourceCode();
            const left = sourceCode.getText(arg.left);
            const right = sourceCode.getText(arg.right);
            return fixer.replaceText(
              node,
              `assert.${assertMethod}(${left}, ${right});`,
            );
          },
        });
      }
    },
  };
};

module.exports.meta = {
  fixable: 'code',
};
