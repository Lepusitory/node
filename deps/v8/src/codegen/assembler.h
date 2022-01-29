// Copyright (c) 1994-2006 Sun Microsystems Inc.
// All Rights Reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// - Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// - Redistribution in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// - Neither the name of Sun Microsystems or the names of contributors may
// be used to endorse or promote products derived from this software without
// specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
// IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// The original source code covered by the above license above has been
// modified significantly by Google Inc.
// Copyright 2012 the V8 project authors. All rights reserved.

#ifndef V8_CODEGEN_ASSEMBLER_H_
#define V8_CODEGEN_ASSEMBLER_H_

#include <forward_list>
#include <memory>
#include <unordered_map>

#include "src/base/macros.h"
#include "src/base/memory.h"
#include "src/codegen/code-comments.h"
#include "src/codegen/cpu-features.h"
#include "src/codegen/external-reference.h"
#include "src/codegen/reglist.h"
#include "src/codegen/reloc-info.h"
#include "src/common/globals.h"
#include "src/deoptimizer/deoptimize-reason.h"
#include "src/flags/flags.h"
#include "src/handles/handles.h"
#include "src/objects/objects.h"

namespace v8 {

// Forward declarations.
class ApiFunction;

namespace internal {

using base::Memory;
using base::ReadUnalignedValue;
using base::WriteUnalignedValue;

// Forward declarations.
class EmbeddedData;
class OffHeapInstructionStream;
class Isolate;
class SCTableReference;
class SourcePosition;
class StatsCounter;
class StringConstantBase;

// -----------------------------------------------------------------------------
// Optimization for far-jmp like instructions that can be replaced by shorter.

class JumpOptimizationInfo {
 public:
  bool is_collecting() const { return stage_ == kCollection; }
  bool is_optimizing() const { return stage_ == kOptimization; }
  void set_optimizing() {
    DCHECK(is_optimizable());
    stage_ = kOptimization;
  }

  bool is_optimizable() const { return optimizable_; }
  void set_optimizable() {
    DCHECK(is_collecting());
    optimizable_ = true;
  }

  // Used to verify the instruction sequence is always the same in two stages.
  size_t hash_code() const { return hash_code_; }
  void set_hash_code(size_t hash_code) { hash_code_ = hash_code; }

  std::vector<uint32_t>& farjmp_bitmap() { return farjmp_bitmap_; }

 private:
  enum { kCollection, kOptimization } stage_ = kCollection;
  bool optimizable_ = false;
  std::vector<uint32_t> farjmp_bitmap_;
  size_t hash_code_ = 0u;
};

class HeapObjectRequest {
 public:
  explicit HeapObjectRequest(double heap_number, int offset = -1);
  explicit HeapObjectRequest(const StringConstantBase* string, int offset = -1);

  enum Kind { kHeapNumber, kStringConstant };
  Kind kind() const { return kind_; }

  double heap_number() const {
    DCHECK_EQ(kind(), kHeapNumber);
    return value_.heap_number;
  }

  const StringConstantBase* string() const {
    DCHECK_EQ(kind(), kStringConstant);
    return value_.string;
  }

  // The code buffer offset at the time of the request.
  int offset() const {
    DCHECK_GE(offset_, 0);
    return offset_;
  }
  void set_offset(int offset) {
    DCHECK_LT(offset_, 0);
    offset_ = offset;
    DCHECK_GE(offset_, 0);
  }

 private:
  Kind kind_;

  union {
    double heap_number;
    const StringConstantBase* string;
  } value_;

  int offset_;
};

// -----------------------------------------------------------------------------
// Platform independent assembler base class.

enum class CodeObjectRequired { kNo, kYes };

struct V8_EXPORT_PRIVATE AssemblerOptions {
  // Recording reloc info for external references and off-heap targets is
  // needed whenever code is serialized, e.g. into the snapshot or as a Wasm
  // module. This flag allows this reloc info to be disabled for code that
  // will not survive process destruction.
  bool record_reloc_info_for_serialization = true;
  // Recording reloc info can be disabled wholesale. This is needed when the
  // assembler is used on existing code directly (e.g. JumpTableAssembler)
  // without any buffer to hold reloc information.
  bool disable_reloc_info_for_patching = false;
  // Enables root-relative access to arbitrary untagged addresses (usually
  // external references). Only valid if code will not survive the process.
  bool enable_root_relative_access = false;
  // Enables specific assembler sequences only used for the simulator.
  bool enable_simulator_code = false;
  // Enables use of isolate-independent constants, indirected through the
  // root array.
  // (macro assembler feature).
  bool isolate_independent_code = false;
  // Enables the use of isolate-independent builtins through an off-heap
  // trampoline. (macro assembler feature).
  bool inline_offheap_trampolines = true;
  // Enables generation of pc-relative calls to builtins if the off-heap
  // builtins are guaranteed to be within the reach of pc-relative call or jump
  // instructions. For example, when the bultins code is re-embedded into the
  // code range.
  bool short_builtin_calls = false;
  // On some platforms, all code is within a given range in the process,
  // and the start of this range is configured here.
  Address code_range_start = 0;
  // Enable pc-relative calls/jumps on platforms that support it. When setting
  // this flag, the code range must be small enough to fit all offsets into
  // the instruction immediates.
  bool use_pc_relative_calls_and_jumps = false;
  // Enables the collection of information useful for the generation of unwind
  // info. This is useful in some platform (Win64) where the unwind info depends
  // on a function prologue/epilogue.
  bool collect_win64_unwind_info = false;
  // Whether to emit code comments.
  bool emit_code_comments = FLAG_code_comments;

  static AssemblerOptions Default(Isolate* isolate);
  static AssemblerOptions DefaultForOffHeapTrampoline(Isolate* isolate);
};

class AssemblerBuffer {
 public:
  virtual ~AssemblerBuffer() = default;
  virtual byte* start() const = 0;
  virtual int size() const = 0;
  // Return a grown copy of this buffer. The contained data is uninitialized.
  // The data in {this} will still be read afterwards (until {this} is
  // destructed), but not written.
  virtual std::unique_ptr<AssemblerBuffer> Grow(int new_size)
      V8_WARN_UNUSED_RESULT = 0;
};

// Allocate an AssemblerBuffer which uses an existing buffer. This buffer cannot
// grow, so it must be large enough for all code emitted by the Assembler.
V8_EXPORT_PRIVATE
std::unique_ptr<AssemblerBuffer> ExternalAssemblerBuffer(void* buffer,
                                                         int size);

// Allocate a new growable AssemblerBuffer with a given initial size.
V8_EXPORT_PRIVATE
std::unique_ptr<AssemblerBuffer> NewAssemblerBuffer(int size);

class V8_EXPORT_PRIVATE AssemblerBase : public Malloced {
 public:
  AssemblerBase(const AssemblerOptions& options,
                std::unique_ptr<AssemblerBuffer>);
  virtual ~AssemblerBase();

  const AssemblerOptions& options() const { return options_; }

  bool predictable_code_size() const { return predictable_code_size_; }
  void set_predictable_code_size(bool value) { predictable_code_size_ = value; }

  uint64_t enabled_cpu_features() const { return enabled_cpu_features_; }
  void set_enabled_cpu_features(uint64_t features) {
    enabled_cpu_features_ = features;
  }
  // Features are usually enabled by CpuFeatureScope, which also asserts that
  // the features are supported before they are enabled.
  // IMPORTANT:  IsEnabled() should only be used by DCHECKs. For real feature
  // detection, use IsSupported().
  bool IsEnabled(CpuFeature f) {
    return (enabled_cpu_features_ & (static_cast<uint64_t>(1) << f)) != 0;
  }
  void EnableCpuFeature(CpuFeature f) {
    enabled_cpu_features_ |= (static_cast<uint64_t>(1) << f);
  }

  bool is_constant_pool_available() const {
    if (FLAG_enable_embedded_constant_pool) {
      // We need to disable constant pool here for embeded builtins
      // because the metadata section is not adjacent to instructions
      return constant_pool_available_ && !options().isolate_independent_code;
    } else {
      // Embedded constant pool not supported on this architecture.
      UNREACHABLE();
    }
  }

  JumpOptimizationInfo* jump_optimization_info() {
    return jump_optimization_info_;
  }
  void set_jump_optimization_info(JumpOptimizationInfo* jump_opt) {
    jump_optimization_info_ = jump_opt;
  }

  void FinalizeJumpOptimizationInfo() {}

  // Overwrite a host NaN with a quiet target NaN.  Used by mksnapshot for
  // cross-snapshotting.
  static void QuietNaN(HeapObject nan) {}

  int pc_offset() const { return static_cast<int>(pc_ - buffer_start_); }

  int pc_offset_for_safepoint() {
#if defined(V8_TARGET_ARCH_MIPS) || defined(V8_TARGET_ARCH_MIPS64) || \
    defined(V8_TARGET_ARCH_LOONG64)
    // MIPS and LOONG need to use their own implementation to avoid trampoline's
    // influence.
    UNREACHABLE();
#else
    return pc_offset();
#endif
  }

  byte* buffer_start() const { return buffer_->start(); }
  int buffer_size() const { return buffer_->size(); }
  int instruction_size() const { return pc_offset(); }

  std::unique_ptr<AssemblerBuffer> ReleaseBuffer() {
    std::unique_ptr<AssemblerBuffer> buffer = std::move(buffer_);
    DCHECK_NULL(buffer_);
    // Reset fields to prevent accidental further modifications of the buffer.
    buffer_start_ = nullptr;
    pc_ = nullptr;
    return buffer;
  }

  // This function is called when code generation is aborted, so that
  // the assembler could clean up internal data structures.
  virtual void AbortedCodeGeneration() {}

  // Debugging
  void Print(Isolate* isolate);

  // Record an inline code comment that can be used by a disassembler.
  // Use --code-comments to enable.
  V8_INLINE void RecordComment(const char* comment) {
    // Set explicit dependency on --code-comments for dead-code elimination in
    // release builds.
    if (!FLAG_code_comments) return;
    if (options().emit_code_comments) {
      code_comments_writer_.Add(pc_offset(), std::string(comment));
    }
  }

  V8_INLINE void RecordComment(std::string comment) {
    // Set explicit dependency on --code-comments for dead-code elimination in
    // release builds.
    if (!FLAG_code_comments) return;
    if (options().emit_code_comments) {
      code_comments_writer_.Add(pc_offset(), std::move(comment));
    }
  }

#ifdef V8_CODE_COMMENTS
  class CodeComment {
   public:
    explicit CodeComment(Assembler* assembler, const std::string& comment)
        : assembler_(assembler) {
      if (FLAG_code_comments) Open(comment);
    }
    ~CodeComment() {
      if (FLAG_code_comments) Close();
    }
    static const int kIndentWidth = 2;

   private:
    int depth() const;
    void Open(const std::string& comment);
    void Close();
    Assembler* assembler_;
  };
#else  // V8_CODE_COMMENTS
  class CodeComment {
    explicit CodeComment(Assembler* assembler, std::string comment) {}
  };
#endif

  // The minimum buffer size. Should be at least two times the platform-specific
  // {Assembler::kGap}.
  static constexpr int kMinimalBufferSize = 128;

  // The default buffer size used if we do not know the final size of the
  // generated code.
  static constexpr int kDefaultBufferSize = 4 * KB;

 protected:
  // Add 'target' to the {code_targets_} vector, if necessary, and return the
  // offset at which it is stored.
  int AddCodeTarget(Handle<Code> target);
  Handle<Code> GetCodeTarget(intptr_t code_target_index) const;

  // Add 'object' to the {embedded_objects_} vector and return the index at
  // which it is stored.
  using EmbeddedObjectIndex = size_t;
  EmbeddedObjectIndex AddEmbeddedObject(Handle<HeapObject> object);
  Handle<HeapObject> GetEmbeddedObject(EmbeddedObjectIndex index) const;

  // The buffer into which code and relocation info are generated.
  std::unique_ptr<AssemblerBuffer> buffer_;
  // Cached from {buffer_->start()}, for faster access.
  byte* buffer_start_;
  std::forward_list<HeapObjectRequest> heap_object_requests_;
  // The program counter, which points into the buffer above and moves forward.
  // TODO(jkummerow): This should probably have type {Address}.
  byte* pc_;

  void set_constant_pool_available(bool available) {
    if (FLAG_enable_embedded_constant_pool) {
      constant_pool_available_ = available;
    } else {
      // Embedded constant pool not supported on this architecture.
      UNREACHABLE();
    }
  }

  // {RequestHeapObject} records the need for a future heap number allocation,
  // code stub generation or string allocation. After code assembly, each
  // platform's {Assembler::AllocateAndInstallRequestedHeapObjects} will
  // allocate these objects and place them where they are expected (determined
  // by the pc offset associated with each request).
  void RequestHeapObject(HeapObjectRequest request);

  bool ShouldRecordRelocInfo(RelocInfo::Mode rmode) const {
    DCHECK(!RelocInfo::IsNoInfo(rmode));
    if (options().disable_reloc_info_for_patching) return false;
    if (RelocInfo::IsOnlyForSerializer(rmode) &&
        !options().record_reloc_info_for_serialization && !FLAG_debug_code) {
      return false;
    }
#ifndef ENABLE_DISASSEMBLER
    if (RelocInfo::IsLiteralConstant(rmode)) return false;
#endif
    return true;
  }

  CodeCommentsWriter code_comments_writer_;

 private:
  // Before we copy code into the code space, we sometimes cannot encode
  // call/jump code targets as we normally would, as the difference between the
  // instruction's location in the temporary buffer and the call target is not
  // guaranteed to fit in the instruction's offset field. We keep track of the
  // code handles we encounter in calls in this vector, and encode the index of
  // the code handle in the vector instead.
  std::vector<Handle<Code>> code_targets_;

  // If an assembler needs a small number to refer to a heap object handle
  // (for example, because there are only 32bit available on a 64bit arch), the
  // assembler adds the object into this vector using AddEmbeddedObject, and
  // may then refer to the heap object using the handle's index in this vector.
  std::vector<Handle<HeapObject>> embedded_objects_;

  // Embedded objects are deduplicated based on handle location. This is a
  // compromise that is almost as effective as deduplication based on actual
  // heap object addresses maintains GC safety.
  std::unordered_map<Handle<HeapObject>, EmbeddedObjectIndex,
                     Handle<HeapObject>::hash, Handle<HeapObject>::equal_to>
      embedded_objects_map_;

  const AssemblerOptions options_;
  uint64_t enabled_cpu_features_;
  bool predictable_code_size_;

  // Indicates whether the constant pool can be accessed, which is only possible
  // if the pp register points to the current code object's constant pool.
  bool constant_pool_available_;

  JumpOptimizationInfo* jump_optimization_info_;

#ifdef V8_CODE_COMMENTS
  int comment_depth_ = 0;
#endif

  // Constant pool.
  friend class FrameAndConstantPoolScope;
  friend class ConstantPoolUnavailableScope;
};

// Enable a specified feature within a scope.
class V8_EXPORT_PRIVATE V8_NODISCARD CpuFeatureScope {
 public:
  enum CheckPolicy {
    kCheckSupported,
    kDontCheckSupported,
  };

#ifdef DEBUG
  CpuFeatureScope(AssemblerBase* assembler, CpuFeature f,
                  CheckPolicy check = kCheckSupported);
  ~CpuFeatureScope();

 private:
  AssemblerBase* assembler_;
  uint64_t old_enabled_;
#else
  CpuFeatureScope(AssemblerBase* assembler, CpuFeature f,
                  CheckPolicy check = kCheckSupported) {}
  ~CpuFeatureScope() {
    // Define a destructor to avoid unused variable warnings.
  }
#endif
};

#ifdef V8_CODE_COMMENTS
#define ASM_CODE_COMMENT(asm) ASM_CODE_COMMENT_STRING(asm, __func__)
#define ASM_CODE_COMMENT_STRING(asm, comment) \
  AssemblerBase::CodeComment UNIQUE_IDENTIFIER(asm_code_comment)(asm, comment)
#else
#define ASM_CODE_COMMENT(asm)
#define ASM_CODE_COMMENT_STRING(asm, ...)
#endif

}  // namespace internal
}  // namespace v8
#endif  // V8_CODEGEN_ASSEMBLER_H_
