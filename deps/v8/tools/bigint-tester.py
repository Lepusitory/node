#!/usr/bin/python
# Copyright 2017 the V8 project authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import argparse
import math
import multiprocessing
import os
import random
import subprocess
import sys
import tempfile

# Configuration.
kChars = "0123456789abcdef"
kBase = 16
kLineLength = 70  # A bit less than 80.
kNumInputsGenerate = 20
kNumInputsStress = 1000

# Internally used sentinels.
kNo = 0
kYes = 1
kRandom = 2

TEST_HEADER = """\
// Copyright 2017 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Generated by %s.
""" % sys.argv[0]

TEST_BODY = """
var error_count = 0;
for (var i = 0; i < data.length; i++) {
  var d = data[i];
%s
}
if (error_count !== 0) {
  print("Finished with " + error_count + " errors.")
  quit(1);
}"""

def GenRandom(length, negative=kRandom):
  if length == 0: return "0n"
  s = []
  if negative == kYes or (negative == kRandom and (random.randint(0, 1) == 0)):
    s.append("-")  # 50% chance of negative.
  s.append("0x")
  s.append(kChars[random.randint(1, kBase - 1)])  # No leading zero.
  for i in range(1, length):
    s.append(kChars[random.randint(0, kBase - 1)])
  s.append("n")
  return "".join(s)

def Parse(x):
  assert x[-1] == 'n', x
  return int(x[:-1], kBase)

def Format(x):
  original = x
  negative = False
  if x == 0: return "0n"
  if x < 0:
    negative = True
    x = -x
  s = ""
  while x > 0:
    s = kChars[x % kBase] + s
    x = x / kBase
  s = "0x" + s + "n"
  if negative:
    s = "-" + s
  assert Parse(s) == original
  return s

class TestGenerator(object):
  # Subclasses must implement these.
  # Returns a JSON snippet defining inputs and expected output for one test.
  def EmitOne(self): raise NotImplementedError
  # Returns a snippet of JavaScript that will operate on a variable "d"
  # whose content is defined by the result of a call to "EmitOne".
  def EmitTestCore(self): raise NotImplementedError

  def EmitHeader(self):
    return TEST_HEADER

  def EmitData(self, count):
    s = []
    for i in range(count):
      s.append(self.EmitOne())
    return "var data = [" + ", ".join(s) + "];"

  def EmitTestBody(self):
    return TEST_BODY % self.EmitTestCore()

  def PrintTest(self, count):
    print(self.EmitHeader())
    print(self.EmitData(count))
    print(self.EmitTestBody())

  def RunTest(self, count, binary):
    try:
      fd, path = tempfile.mkstemp(suffix=".js", prefix="bigint-test-")
      with open(path, "w") as f:
        f.write(self.EmitData(count))
        f.write(self.EmitTestBody())
      return subprocess.call("%s %s" % (binary, path),
                             shell=True)
    finally:
      os.close(fd)
      os.remove(path)

class UnaryOp(TestGenerator):
  # Subclasses must implement these two.
  def GetOpString(self): raise NotImplementedError
  def GenerateResult(self, x): raise NotImplementedError

  # Subclasses may override this:
  def GenerateInput(self):
    return GenRandom(random.randint(0, kLineLength))

  # Subclasses should not override anything below.
  def EmitOne(self):
    x_str = self.GenerateInput()
    x_num = Parse(x_str)
    result_num = self.GenerateResult(x_num)
    result_str = Format(result_num)
    return "{\n  a: %s,\n  r: %s\n}" % (x_str, result_str)

  def EmitTestCore(self):
    return """\
  var r = %(op)sd.a;
  if (d.r !== r) {
    print("Input:    " + d.a.toString(%(base)d));
    print("Result:   " + r.toString(%(base)d));
    print("Expected: " + d.r);
    error_count++;
  }""" % {"op": self.GetOpString(), "base": kBase}

class BinaryOp(TestGenerator):
  # Subclasses must implement these two.
  def GetOpString(self): raise NotImplementedError
  def GenerateResult(self, left, right): raise NotImplementedError

  # Subclasses may override these:
  def GenerateInputLengths(self):
    return random.randint(0, kLineLength), random.randint(0, kLineLength)

  def GenerateInputs(self):
    left_length, right_length = self.GenerateInputLengths()
    return GenRandom(left_length), GenRandom(right_length)

  # Subclasses should not override anything below.
  def EmitOne(self):
    left_str, right_str = self.GenerateInputs()
    left_num = Parse(left_str)
    right_num = Parse(right_str)
    result_num = self.GenerateResult(left_num, right_num)
    result_str = Format(result_num)
    return ("{\n  a: %s,\n  b: %s,\n  r: %s\n}" %
            (left_str, right_str, result_str))

  def EmitTestCore(self):
    return """\
  var r = d.a %(op)s d.b;
  if (d.r !== r) {
    print("Input A:  " + d.a.toString(%(base)d));
    print("Input B:  " + d.b.toString(%(base)d));
    print("Result:   " + r.toString(%(base)d));
    print("Expected: " + d.r);
    print("Op: %(op)s");
    error_count++;
  }""" % {"op": self.GetOpString(), "base": kBase}

class Neg(UnaryOp):
  def GetOpString(self): return "-"
  def GenerateResult(self, x): return -x

class BitNot(UnaryOp):
  def GetOpString(self): return "~"
  def GenerateResult(self, x): return ~x

class Inc(UnaryOp):
  def GetOpString(self): return "++"
  def GenerateResult(self, x): return x + 1

class Dec(UnaryOp):
  def GetOpString(self): return "--"
  def GenerateResult(self, x): return x - 1

class Add(BinaryOp):
  def GetOpString(self): return "+"
  def GenerateResult(self, a, b): return a + b

class Sub(BinaryOp):
  def GetOpString(self): return "-"
  def GenerateResult(self, a, b): return a - b

class Mul(BinaryOp):
  def GetOpString(self): return "*"
  def GenerateResult(self, a, b): return a * b
  def GenerateInputLengths(self):
    left_length = random.randint(1, kLineLength)
    return left_length, kLineLength - left_length

class Div(BinaryOp):
  def GetOpString(self): return "/"
  def GenerateResult(self, a, b):
    result = abs(a) / abs(b)
    if (a < 0) != (b < 0): result = -result
    return result
  def GenerateInputLengths(self):
    # Let the left side be longer than the right side with high probability,
    # because that case is more interesting.
    min_left = kLineLength * 6 / 10
    max_right = kLineLength * 7 / 10
    return random.randint(min_left, kLineLength), random.randint(1, max_right)

class Mod(Div):  # Sharing GenerateInputLengths.
  def GetOpString(self): return "%"
  def GenerateResult(self, a, b):
    result = a % b
    if a < 0 and result > 0:
      result -= abs(b)
    if a > 0 and result < 0:
      result += abs(b)
    return result

class Shl(BinaryOp):
  def GetOpString(self): return "<<"
  def GenerateInputsInternal(self, small_shift_positive):
    left_length = random.randint(0, kLineLength - 1)
    left = GenRandom(left_length)
    small_shift = random.randint(0, 1) == 0
    if small_shift:
      right_length = 1 + int(math.log((kLineLength - left_length), kBase))
      neg = kNo if small_shift_positive else kYes
    else:
      right_length = random.randint(0, 3)
      neg = kYes if small_shift_positive else kNo
    right = GenRandom(right_length, negative=neg)
    return left, right

  def GenerateInputs(self): return self.GenerateInputsInternal(True)
  def GenerateResult(self, a, b):
    if b < 0: return a >> -b
    return a << b

class Sar(Shl):  # Sharing GenerateInputsInternal.
  def GetOpString(self): return ">>"
  def GenerateInputs(self):
    return self.GenerateInputsInternal(False)
  def GenerateResult(self, a, b):
    if b < 0: return a << -b
    return a >> b

class BitAnd(BinaryOp):
  def GetOpString(self): return "&"
  def GenerateResult(self, a, b): return a & b

class BitOr(BinaryOp):
  def GetOpString(self): return "|"
  def GenerateResult(self, a, b): return a | b

class BitXor(BinaryOp):
  def GetOpString(self): return "^"
  def GenerateResult(self, a, b): return a ^ b

OPS = {
  "add": Add,
  "sub": Sub,
  "mul": Mul,
  "div": Div,
  "mod": Mod,
  "inc": Inc,
  "dec": Dec,
  "neg": Neg,
  "not": BitNot,
  "shl": Shl,
  "sar": Sar,
  "and": BitAnd,
  "or": BitOr,
  "xor": BitXor
}

OPS_NAMES = ", ".join(sorted(OPS.keys()))

def RunOne(op, num_inputs, binary):
  return OPS[op]().RunTest(num_inputs, binary)
def WrapRunOne(args):
  return RunOne(*args)
def RunAll(args):
  for op in args.op:
    for r in xrange(args.runs):
      yield (op, args.num_inputs, args.binary)

def Main():
  parser = argparse.ArgumentParser(
      description="Helper for generating or running BigInt tests.")
  parser.add_argument(
      "action", help="Action to perform: 'generate' or 'stress'")
  parser.add_argument(
      "op", nargs="+",
      help="Operation(s) to test, one or more of: %s. In 'stress' mode, "
           "special op 'all' tests all ops." % OPS_NAMES)
  parser.add_argument(
      "-n", "--num-inputs", type=int, default=-1,
      help="Number of input/output sets in each generated test. Defaults to "
           "%d for 'generate' and '%d' for 'stress' mode." %
           (kNumInputsGenerate, kNumInputsStress))

  stressopts = parser.add_argument_group("'stress' mode arguments")
  stressopts.add_argument(
      "-r", "--runs", type=int, default=1000,
      help="Number of tests (with NUM_INPUTS each) to generate and run. "
           "Default: %(default)s.")
  stressopts.add_argument(
      "-b", "--binary", default="out/x64.debug/d8",
      help="The 'd8' binary to use. Default: %(default)s.")
  args = parser.parse_args()

  for op in args.op:
    if op not in OPS.keys() and op != "all":
      print("Invalid op '%s'. See --help." % op)
      return 1

  if len(args.op) == 1 and args.op[0] == "all":
    args.op = OPS.keys()

  if args.action == "generate":
    if args.num_inputs < 0: args.num_inputs = kNumInputsGenerate
    for op in args.op:
      OPS[op]().PrintTest(args.num_inputs)
  elif args.action == "stress":
    if args.num_inputs < 0: args.num_inputs = kNumInputsStress
    result = 0
    pool = multiprocessing.Pool(multiprocessing.cpu_count())
    for r in pool.imap_unordered(WrapRunOne, RunAll(args)):
      result = result or r
    return result
  else:
    print("Invalid action '%s'. See --help." % args.action)
    return 1

if __name__ == "__main__":
  sys.exit(Main())
