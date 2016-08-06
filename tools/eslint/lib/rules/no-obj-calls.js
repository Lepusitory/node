/**
 * @fileoverview Rule to flag use of an object property of the global object (Math and JSON) as a function
 * @author James Allardice
 */

"use strict";

//------------------------------------------------------------------------------
// Rule Definition
//------------------------------------------------------------------------------

module.exports = {
    meta: {
        docs: {
            description: "disallow calling global object properties as functions",
            category: "Possible Errors",
            recommended: true
        },

        schema: []
    },

    create: function(context) {

        return {
            CallExpression: function(node) {

                if (node.callee.type === "Identifier") {
                    let name = node.callee.name;

                    if (name === "Math" || name === "JSON") {
                        context.report(node, "'{{name}}' is not a function.", { name: name });
                    }
                }
            }
        };

    }
};
