// Copyright 2018 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/torque/type-oracle.h"

namespace v8 {
namespace internal {
namespace torque {

DEFINE_CONTEXTUAL_VARIABLE(TypeOracle)

// static
const std::vector<std::unique_ptr<AggregateType>>*
TypeOracle::GetAggregateTypes() {
  return &Get().aggregate_types_;
}

// static
void TypeOracle::FinalizeAggregateTypes() {
  for (const std::unique_ptr<AggregateType>& p : Get().aggregate_types_) {
    p->Finalize();
  }
}

}  // namespace torque
}  // namespace internal
}  // namespace v8
