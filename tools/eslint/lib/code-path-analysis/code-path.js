/**
 * @fileoverview A class of the code path.
 * @author Toru Nagashima
 */

"use strict";

//------------------------------------------------------------------------------
// Requirements
//------------------------------------------------------------------------------

let CodePathState = require("./code-path-state");
let IdGenerator = require("./id-generator");

//------------------------------------------------------------------------------
// Public Interface
//------------------------------------------------------------------------------

/**
 * A code path.
 *
 * @constructor
 * @param {string} id - An identifier.
 * @param {CodePath|null} upper - The code path of the upper function scope.
 * @param {Function} onLooped - A callback function to notify looping.
 */
function CodePath(id, upper, onLooped) {

    /**
     * The identifier of this code path.
     * Rules use it to store additional information of each rule.
     * @type {string}
     */
    this.id = id;

    /**
     * The code path of the upper function scope.
     * @type {CodePath|null}
     */
    this.upper = upper;

    /**
     * The code paths of nested function scopes.
     * @type {CodePath[]}
     */
    this.childCodePaths = [];

    // Initializes internal state.
    Object.defineProperty(
        this,
        "internal",
        {value: new CodePathState(new IdGenerator(id + "_"), onLooped)});

    // Adds this into `childCodePaths` of `upper`.
    if (upper) {
        upper.childCodePaths.push(this);
    }
}

CodePath.prototype = {
    constructor: CodePath,

    /**
     * The initial code path segment.
     * @type {CodePathSegment}
     */
    get initialSegment() {
        return this.internal.initialSegment;
    },

    /**
     * Final code path segments.
     * This array is a mix of `returnedSegments` and `thrownSegments`.
     * @type {CodePathSegment[]}
     */
    get finalSegments() {
        return this.internal.finalSegments;
    },

    /**
     * Final code path segments which is with `return` statements.
     * This array contains the last path segment if it's reachable.
     * Since the reachable last path returns `undefined`.
     * @type {CodePathSegment[]}
     */
    get returnedSegments() {
        return this.internal.returnedForkContext;
    },

    /**
     * Final code path segments which is with `throw` statements.
     * @type {CodePathSegment[]}
     */
    get thrownSegments() {
        return this.internal.thrownForkContext;
    },

    /**
     * Current code path segments.
     * @type {CodePathSegment[]}
     */
    get currentSegments() {
        return this.internal.currentSegments;
    },

    /**
     * Traverses all segments in this code path.
     *
     *     codePath.traverseSegments(function(segment, controller) {
     *         // do something.
     *     });
     *
     * This method enumerates segments in order from the head.
     *
     * The `controller` object has two methods.
     *
     * - `controller.skip()` - Skip the following segments in this branch.
     * - `controller.break()` - Skip all following segments.
     *
     * @param {Object} [options] - Omittable.
     * @param {CodePathSegment} [options.first] - The first segment to traverse.
     * @param {CodePathSegment} [options.last] - The last segment to traverse.
     * @param {Function} callback - A callback function.
     * @returns {void}
     */
    traverseSegments: function(options, callback) {
        if (typeof options === "function") {
            callback = options;
            options = null;
        }

        options = options || {};
        let startSegment = options.first || this.internal.initialSegment;
        let lastSegment = options.last;

        let item = null;
        let index = 0;
        let end = 0;
        let segment = null;
        let visited = Object.create(null);
        let stack = [[startSegment, 0]];
        let skippedSegment = null;
        let broken = false;
        let controller = {
            skip: function() {
                if (stack.length <= 1) {
                    broken = true;
                } else {
                    skippedSegment = stack[stack.length - 2][0];
                }
            },
            break: function() {
                broken = true;
            }
        };

        /**
         * Checks a given previous segment has been visited.
         * @param {CodePathSegment} prevSegment - A previous segment to check.
         * @returns {boolean} `true` if the segment has been visited.
         */
        function isVisited(prevSegment) {
            return (
                visited[prevSegment.id] ||
                segment.isLoopedPrevSegment(prevSegment)
            );
        }

        while (stack.length > 0) {
            item = stack[stack.length - 1];
            segment = item[0];
            index = item[1];

            if (index === 0) {

                // Skip if this segment has been visited already.
                if (visited[segment.id]) {
                    stack.pop();
                    continue;
                }

                // Skip if all previous segments have not been visited.
                if (segment !== startSegment &&
                    segment.prevSegments.length > 0 &&
                    !segment.prevSegments.every(isVisited)
                ) {
                    stack.pop();
                    continue;
                }

                // Reset the flag of skipping if all branches have been skipped.
                if (skippedSegment && segment.prevSegments.indexOf(skippedSegment) !== -1) {
                    skippedSegment = null;
                }
                visited[segment.id] = true;

                // Call the callback when the first time.
                if (!skippedSegment) {
                    callback.call(this, segment, controller); // eslint-disable-line callback-return
                    if (segment === lastSegment) {
                        controller.skip();
                    }
                    if (broken) {
                        break;
                    }
                }
            }

            // Update the stack.
            end = segment.nextSegments.length - 1;
            if (index < end) {
                item[1] += 1;
                stack.push([segment.nextSegments[index], 0]);
            } else if (index === end) {
                item[0] = segment.nextSegments[index];
                item[1] = 0;
            } else {
                stack.pop();
            }
        }
    }
};

/**
 * Gets the state of a given code path.
 *
 * @param {CodePath} codePath - A code path to get.
 * @returns {CodePathState} The state of the code path.
 */
CodePath.getState = function getState(codePath) {
    return codePath.internal;
};

module.exports = CodePath;
