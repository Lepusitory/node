/**
 * @fileoverview Rule to flag references to undeclared variables.
 * @author Mark Macdonald
 */
"use strict";

//------------------------------------------------------------------------------
// Helpers
//------------------------------------------------------------------------------

/**
 * Checks if the given node is the argument of a typeof operator.
 * @param {ASTNode} node The AST node being checked.
 * @returns {boolean} Whether or not the node is the argument of a typeof operator.
 */
function hasTypeOfOperator(node) {
    let parent = node.parent;

    return parent.type === "UnaryExpression" && parent.operator === "typeof";
}

//------------------------------------------------------------------------------
// Rule Definition
//------------------------------------------------------------------------------

module.exports = {
    meta: {
        docs: {
            description: "disallow the use of undeclared variables unless mentioned in `/*global */` comments",
            category: "Variables",
            recommended: true
        },

        schema: [
            {
                type: "object",
                properties: {
                    typeof: {
                        type: "boolean"
                    }
                },
                additionalProperties: false
            }
        ]
    },

    create: function(context) {
        let options = context.options[0];
        let considerTypeOf = options && options.typeof === true || false;

        return {
            "Program:exit": function(/* node */) {
                let globalScope = context.getScope();

                globalScope.through.forEach(function(ref) {
                    let identifier = ref.identifier;

                    if (!considerTypeOf && hasTypeOfOperator(identifier)) {
                        return;
                    }

                    context.report({
                        node: identifier,
                        message: "'{{name}}' is not defined.",
                        data: identifier
                    });
                });
            }
        };
    }
};
