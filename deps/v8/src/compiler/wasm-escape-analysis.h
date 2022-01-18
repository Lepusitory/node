// Copyright 2021 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if !V8_ENABLE_WEBASSEMBLY
#error This header should only be included if WebAssembly is enabled.
#endif  // !V8_ENABLE_WEBASSEMBLY

#ifndef V8_COMPILER_WASM_ESCAPE_ANALYSIS_H_
#define V8_COMPILER_WASM_ESCAPE_ANALYSIS_H_

#include "src/compiler/graph-reducer.h"

namespace v8 {
namespace internal {
namespace compiler {

class MachineGraph;

// Eliminate allocated objects which are only assigned to.
// Current restrictions: Does not work for arrays (until they are also allocated
// with AllocateRaw). Does not work if the allocated object is passed to a phi.
class WasmEscapeAnalysis final : public AdvancedReducer {
 public:
  WasmEscapeAnalysis(Editor* editor, MachineGraph* mcgraph)
      : AdvancedReducer(editor), mcgraph_(mcgraph) {}

  const char* reducer_name() const override { return "WasmEscapeAnalysis"; }

  Reduction Reduce(Node* node) final;

 private:
  Reduction ReduceAllocateRaw(Node* call);
  MachineGraph* const mcgraph_;
};

}  // namespace compiler
}  // namespace internal
}  // namespace v8

#endif  // V8_COMPILER_WASM_ESCAPE_ANALYSIS_H_
