// Copyright 2018 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Flags: --experimental-wasm-anyref --expose-gc

load("test/mjsunit/wasm/wasm-module-builder.js");

(function TestDefaultValue() {
  print(arguments.callee.name);
  let builder = new WasmModuleBuilder();
  const g_nullref = builder.addGlobal(kWasmAnyRef, true).index;
  const g_nullfunc = builder.addGlobal(kWasmAnyFunc, true).index;
  builder.addFunction("get_anyref_global", kSig_r_v)
    .addBody([kExprGetGlobal, g_nullref])
    .exportAs("get_anyref_global");
  builder.addFunction("get_anyfunc_global", kSig_a_v)
    .addBody([kExprGetGlobal, g_nullfunc])
    .exportAs("get_anyfunc_global");

  const instance = builder.instantiate();
  assertEquals(null, instance.exports.get_anyref_global());
  assertEquals(null, instance.exports.get_anyfunc_global());
})();

(function TestDefaultValueSecondGlobal() {
  print(arguments.callee.name);
  let builder = new WasmModuleBuilder();
  const g_setref = builder.addGlobal(kWasmAnyRef, true);
  const g_setfunc = builder.addGlobal(kWasmAnyFunc, true);
  const g_nullref = builder.addGlobal(kWasmAnyRef, true);
  const g_nullfunc = builder.addGlobal(kWasmAnyFunc, true);
  builder.addFunction("get_anyref_global", kSig_r_r)
    .addBody([
      kExprGetLocal, 0,
      kExprSetGlobal, g_setref.index,
      kExprGetGlobal, g_nullref.index
    ])
    .exportAs("get_anyref_global");
  builder.addFunction("get_anyfunc_global", kSig_a_a)
    .addBody([
      kExprGetLocal, 0,
      kExprSetGlobal, g_setfunc.index,
      kExprGetGlobal, g_nullfunc.index
    ])
    .exportAs("get_anyfunc_global");

  const instance = builder.instantiate();
  assertEquals(null, instance.exports.get_anyref_global({}));
  assertEquals(null, instance.exports.get_anyfunc_global(
    instance.exports.get_anyref_global));
})();

(function TestAnyRefGlobalChangeValue() {
  print(arguments.callee.name);
  let builder = new WasmModuleBuilder();
  // Dummy global for offset.
  builder.addGlobal(kWasmAnyRef, true);
  const g = builder.addGlobal(kWasmAnyRef, true);
  builder.addFunction("main", kSig_r_r)
    .addBody([
      kExprGetLocal, 0,
      kExprSetGlobal, g.index,
      kExprGetGlobal, g.index
    ])
    .exportAs("main");

  const instance = builder.instantiate();

  const test_value = { hello: 'world' };
  assertSame(test_value, instance.exports.main(test_value));
})();

(function TestAnyFuncGlobalChangeValue() {
  print(arguments.callee.name);
  let builder = new WasmModuleBuilder();
  // Dummy global for offset.
  builder.addGlobal(kWasmAnyFunc, true);
  const g = builder.addGlobal(kWasmAnyFunc, true);
  builder.addFunction("main", kSig_a_a)
    .addBody([
      kExprGetLocal, 0,
      kExprSetGlobal, g.index,
      kExprGetGlobal, g.index
    ])
    .exportAs("main");

  const instance = builder.instantiate();

  const test_value = instance.exports.main;
  assertSame(test_value, instance.exports.main(test_value));
})();

(function TestGlobalChangeValueWithGC() {
  print(arguments.callee.name);
  let builder = new WasmModuleBuilder();
  const gc_index = builder.addImport("q", "gc", kSig_v_v);
  // Dummy global for offset.
  builder.addGlobal(kWasmAnyRef, true);
  const g = builder.addGlobal(kWasmAnyRef, true);
  builder.addFunction("main", kSig_r_r)
    .addBody([
      kExprGetLocal, 0,
      kExprSetGlobal, g.index,
      kExprCallFunction, gc_index,  // call gc
      kExprGetGlobal, g.index
    ])
    .exportAs("main");

  const instance = builder.instantiate({ q: { gc: gc } });

  const test_value = { hello: 'world' };
  assertSame(test_value, instance.exports.main(test_value));
  assertSame(5, instance.exports.main(5));
  assertSame("Hello", instance.exports.main("Hello"));
})();

(function TestGlobalAsRoot() {
  print(arguments.callee.name);
  let builder = new WasmModuleBuilder();
  const g = builder.addGlobal(kWasmAnyRef, true);
  builder.addFunction("get_global", kSig_r_v)
    .addBody([
      kExprGetGlobal, g.index
    ])
    .exportAs("get_global");

  builder.addFunction("set_global", kSig_v_r)
    .addBody([
      kExprGetLocal, 0,
      kExprSetGlobal, g.index
    ])
    .exportAs("set_global");

  const instance = builder.instantiate();

  let test_value = { hello: 'world' };
  instance.exports.set_global(test_value);
  test_value = null;
  gc();

  const result = instance.exports.get_global();

  assertEquals('world', result.hello);
})();

(function TestImportedAnyRef() {
  print(arguments.callee.name);
  function Test(obj) {
    let builder = new WasmModuleBuilder();
    const g = builder.addImportedGlobal('m', 'val', kWasmAnyRef);
    builder.addFunction('main', kSig_r_v)
      .addBody([kExprGetGlobal, g])
      .exportAs('main');

    const instance = builder.instantiate({ m: { val: obj } });
    assertSame(obj, instance.exports.main());
  }
  Test(null);
  Test(undefined);
  Test(1653);
  Test("mystring");
  Test({ q: 14 });
  Test(print);
})();

function dummy_func() {
  let builder = new WasmModuleBuilder();
  builder.addFunction("dummy", kSig_i_v)
    .addBody([kExprI32Const, 12])
    .exportAs("dummy");
  return builder.instantiate().exports.dummy;
}

(function TestImportedAnyFunc() {
  print(arguments.callee.name);

  let builder = new WasmModuleBuilder();
  const g = builder.addImportedGlobal('m', 'val', kWasmAnyFunc);
  builder.addFunction('main', kSig_a_v)
    .addBody([kExprGetGlobal, g])
    .exportAs('main');

  const module = builder.toModule();
  const instance = new WebAssembly.Instance(module, { m: { val: null } });
  assertSame(null, instance.exports.main());

  const instance2 = new WebAssembly.Instance(
    module, { m: { val: instance.exports.main } });
  assertSame(instance.exports.main, instance2.exports.main());

  assertThrows(() => new WebAssembly.Instance(module, { m: { val: {} } }),
    WebAssembly.LinkError);
})();

(function TestAnyRefGlobalObjectDefaultValue() {
  print(arguments.callee.name);
  let default_init = new WebAssembly.Global({ value: 'anyref', mutable: true });
  assertSame(null, default_init.value);
  assertSame(null, default_init.valueOf());
})();

(function TestAnyFuncGlobalObjectDefaultValue() {
  print(arguments.callee.name);
  let default_init = new WebAssembly.Global({ value: 'anyfunc', mutable: true });
  assertSame(null, default_init.value);
  assertSame(null, default_init.valueOf());
})();

(function TestAnyRefGlobalObject() {
  print(arguments.callee.name);
  function TestGlobal(obj) {
    const global = new WebAssembly.Global({ value: 'anyref' }, obj);
    assertSame(obj, global.value);
    assertSame(obj, global.valueOf());
  }

  TestGlobal(null);
  TestGlobal(undefined);
  TestGlobal(1663);
  TestGlobal("testmyglobal");
  TestGlobal({ a: 11 });
  TestGlobal(print);
})();

(function TestAnyFuncGlobalObject() {
  print(arguments.callee.name);

  const dummy = dummy_func();
  const global = new WebAssembly.Global({ value: 'anyfunc' }, dummy);
  assertSame(dummy, global.value);
  assertSame(dummy, global.valueOf());

  const global_null = new WebAssembly.Global({ value: 'anyfunc' }, null);
  assertSame(null, global_null.value);
  assertSame(null, global_null.valueOf());

  assertThrows(() => new WebAssembly.Global({ value: 'anyfunc' }, {}), TypeError);
})();

(function TestAnyRefGlobalObjectSetValue() {
  print(arguments.callee.name);
  let global = new WebAssembly.Global({ value: 'anyref', mutable: true });

  function TestGlobal(obj) {
    global.value = obj;
    assertSame(obj, global.value);
    assertSame(obj, global.valueOf());
  }

  TestGlobal(null);
  assertThrows(() => TestGlobal(undefined), TypeError);
  TestGlobal(1663);
  TestGlobal("testmyglobal");
  TestGlobal({ a: 11 });
  TestGlobal(print);
})();


(function TestAnyFuncGlobalObjectSetValue() {
  print(arguments.callee.name);
  let global = new WebAssembly.Global({ value: 'anyfunc', mutable: true });

  const dummy = dummy_func();
  global.value = dummy;
  assertSame(dummy, global.value);
  assertSame(dummy, global.valueOf());

  global.value = null;
  assertSame(null, global.value);
  assertSame(null, global.valueOf());

  assertThrows(() => global.value = {}, TypeError);
})();

(function TestExportMutableRefGlobal() {
  print(arguments.callee.name);
  let builder = new WasmModuleBuilder();
  const g1 = builder.addGlobal(kWasmAnyRef, true).exportAs("global1");
  const g2 = builder.addGlobal(kWasmAnyFunc, true).exportAs("global2");
  builder.addGlobal(kWasmI32, true); // Dummy.
  builder.addGlobal(kWasmAnyRef, true); // Dummy.
  const g3 = builder.addGlobal(kWasmAnyRef, true).exportAs("global3");
  const g4 = builder.addGlobal(kWasmAnyFunc, true).exportAs("global4");
  builder.addFunction("main",
    makeSig([kWasmAnyRef, kWasmAnyFunc, kWasmAnyRef, kWasmAnyFunc], []))
    .addBody([
      kExprGetLocal, 0,
      kExprSetGlobal, g1.index,
      kExprGetLocal, 1,
      kExprSetGlobal, g2.index,
      kExprGetLocal, 2,
      kExprSetGlobal, g3.index,
      kExprGetLocal, 3,
      kExprSetGlobal, g4.index
    ])
    .exportAs("main");

  const instance = builder.instantiate();
  const obj1 = { x: 221 };
  const func2 = instance.exports.main;
  const obj3 = print;
  const func4 = dummy_func();
  instance.exports.main(obj1, func2, obj3, func4);
  assertSame(obj1, instance.exports.global1.value);
  assertSame(func2, instance.exports.global2.value);
  assertSame(obj3, instance.exports.global3.value);
  assertSame(func4, instance.exports.global4.value);
})();

(function TestImportMutableAnyRefGlobal() {
  print(arguments.callee.name);
  function Test(obj) {
    let builder = new WasmModuleBuilder();
    const g = builder.addImportedGlobal('m', 'val', kWasmAnyRef, true);
    builder.addFunction('main', kSig_r_v)
      .addBody([kExprGetGlobal, g])
      .exportAs('main');

    const global = new WebAssembly.Global({ value: 'anyref', mutable: 'true' }, obj);
    const instance = builder.instantiate({ m: { val: global } });
    assertSame(obj, instance.exports.main());
  }
  Test(null);
  Test(undefined);
  Test(1653);
  Test("mystring");
  Test({ q: 14 });
  Test(print);
})();

(function TestImportMutableAnyFuncGlobal() {
  print(arguments.callee.name);
  function Test(obj) {
    let builder = new WasmModuleBuilder();
    const g = builder.addImportedGlobal('m', 'val', kWasmAnyFunc, true);
    builder.addFunction('main', kSig_a_v)
      .addBody([kExprGetGlobal, g])
      .exportAs('main');

    const global = new WebAssembly.Global({ value: 'anyfunc', mutable: 'true' }, obj);
    const instance = builder.instantiate({ m: { val: global } });
    assertSame(obj, instance.exports.main());
  }
  Test(dummy_func());
  Test(null);
})();

(function TestImportMutableAnyRefGlobalFromOtherInstance() {
  print(arguments.callee.name);

  // Create an instance which exports globals.
  let builder1 = new WasmModuleBuilder();
  const g3 = builder1.addGlobal(kWasmAnyRef, true).exportAs("e3");
  builder1.addGlobal(kWasmI32, true).exportAs("e1"); // Dummy.
  builder1.addGlobal(kWasmAnyRef, true).exportAs("e4"); // Dummy.
  const g2 = builder1.addGlobal(kWasmAnyRef, true).exportAs("e2");

  builder1.addFunction("set_globals", kSig_v_rr)
    .addBody([
      kExprGetLocal, 0,
      kExprSetGlobal, g2.index,
      kExprGetLocal, 1,
      kExprSetGlobal, g3.index,
    ])
    .exportAs("set_globals");

  builder1.addFunction('get_global2', kSig_r_v)
    .addBody([kExprGetGlobal, g2.index])
    .exportAs('get_global2');

  builder1.addFunction('get_global3', kSig_r_v)
    .addBody([kExprGetGlobal, g3.index])
    .exportAs('get_global3');

  const instance1 = builder1.instantiate();
  const obj2 = { x: 221 };
  const obj3 = print;
  instance1.exports.set_globals(obj2, obj3);

  // Create an instance which imports the globals of the other instance.
  let builder2 = new WasmModuleBuilder();
  const i1 = builder2.addImportedGlobal('exports', 'e1', kWasmI32, true);
  const i2 = builder2.addImportedGlobal('exports', 'e2', kWasmAnyRef, true);
  const i3 = builder2.addImportedGlobal('exports', 'e3', kWasmAnyRef, true);
  const i4 = builder2.addImportedGlobal('exports', 'e4', kWasmAnyRef, true);

  builder2.addExportOfKind("reexport1", kExternalGlobal, i1);
  builder2.addExportOfKind("reexport2", kExternalGlobal, i2);
  builder2.addExportOfKind("reexport3", kExternalGlobal, i3);
  builder2.addExportOfKind("reexport4", kExternalGlobal, i4);

  builder2.addFunction("set_globals", kSig_v_rr)
    .addBody([
      kExprGetLocal, 0,
      kExprSetGlobal, i2,
      kExprGetLocal, 1,
      kExprSetGlobal, i3,
    ])
    .exportAs("set_globals");

  builder2.addFunction('get_global2', kSig_r_v)
    .addBody([kExprGetGlobal, i2])
    .exportAs('get_global2');

  builder2.addFunction('get_global3', kSig_r_v)
    .addBody([kExprGetGlobal, i3])
    .exportAs('get_global3');

  const instance2 = builder2.instantiate(instance1);
  // Check if the globals were imported correctly.
  assertSame(obj2, instance2.exports.get_global2());
  assertSame(obj3, instance2.exports.get_global3());

  assertSame(obj2, instance2.exports.reexport2.value);
  assertSame(obj3, instance2.exports.reexport3.value);

  // Check if instance2 can make changes visible for instance1.
  instance2.exports.set_globals(null, undefined);
  assertEquals(null, instance1.exports.get_global2());
  assertEquals(undefined, instance1.exports.get_global3());

  assertEquals(null, instance2.exports.reexport2.value);
  assertEquals(undefined, instance2.exports.reexport3.value);

  // Check if instance1 can make changes visible for instance2.
  instance1.exports.set_globals("foo", 66343);
  assertEquals("foo", instance2.exports.get_global2());
  assertEquals(66343, instance2.exports.get_global3());

  assertEquals("foo", instance2.exports.reexport2.value);
  assertEquals(66343, instance2.exports.reexport3.value);

  const bar2 = { f: "oo" };
  const bar3 = { b: "ar" };
  instance2.exports.reexport2.value = bar2;
  instance2.exports.reexport3.value = bar3;

  assertSame(bar2, instance1.exports.get_global2());
  assertSame(bar3, instance1.exports.get_global3());
  assertSame(bar2, instance2.exports.get_global2());
  assertSame(bar3, instance2.exports.get_global3());
})();

(function TestImportMutableAnyFuncGlobalFromOtherInstance() {
  print(arguments.callee.name);

  // Create an instance which exports globals.
  let builder1 = new WasmModuleBuilder();
  const g3 = builder1.addGlobal(kWasmAnyFunc, true).exportAs("e3");
  builder1.addGlobal(kWasmI32, true).exportAs("e1"); // Dummy.
  builder1.addGlobal(kWasmAnyFunc, true).exportAs("e4"); // Dummy.
  const g2 = builder1.addGlobal(kWasmAnyFunc, true).exportAs("e2");

  builder1.addFunction("set_globals", kSig_v_aa)
    .addBody([
      kExprGetLocal, 0,
      kExprSetGlobal, g2.index,
      kExprGetLocal, 1,
      kExprSetGlobal, g3.index,
    ])
    .exportAs("set_globals");

  builder1.addFunction('get_global2', kSig_a_v)
    .addBody([kExprGetGlobal, g2.index])
    .exportAs('get_global2');

  builder1.addFunction('get_global3', kSig_a_v)
    .addBody([kExprGetGlobal, g3.index])
    .exportAs('get_global3');

  const instance1 = builder1.instantiate();
  const obj2 = dummy_func();
  const obj3 = instance1.exports.set_globals;
  const obj4 = instance1.exports.get_global3;
  instance1.exports.set_globals(obj2, obj3);

  // Create an instance which imports the globals of the other instance.
  let builder2 = new WasmModuleBuilder();
  const i1 = builder2.addImportedGlobal('exports', 'e1', kWasmI32, true);
  const i2 = builder2.addImportedGlobal('exports', 'e2', kWasmAnyFunc, true);
  const i3 = builder2.addImportedGlobal('exports', 'e3', kWasmAnyFunc, true);
  const i4 = builder2.addImportedGlobal('exports', 'e4', kWasmAnyFunc, true);

  builder2.addExportOfKind("reexport1", kExternalGlobal, i1);
  builder2.addExportOfKind("reexport2", kExternalGlobal, i2);
  builder2.addExportOfKind("reexport3", kExternalGlobal, i3);
  builder2.addExportOfKind("reexport4", kExternalGlobal, i4);

  builder2.addFunction("set_globals", kSig_v_aa)
    .addBody([
      kExprGetLocal, 0,
      kExprSetGlobal, i2,
      kExprGetLocal, 1,
      kExprSetGlobal, i3,
    ])
    .exportAs("set_globals");

  builder2.addFunction('get_global2', kSig_a_v)
    .addBody([kExprGetGlobal, i2])
    .exportAs('get_global2');

  builder2.addFunction('get_global3', kSig_a_v)
    .addBody([kExprGetGlobal, i3])
    .exportAs('get_global3');

  const instance2 = builder2.instantiate(instance1);
  // Check if the globals were imported correctly.
  assertSame(obj2, instance2.exports.get_global2());
  assertSame(obj3, instance2.exports.get_global3());

  assertSame(obj2, instance2.exports.reexport2.value);
  assertSame(obj3, instance2.exports.reexport3.value);

  // Check if instance2 can make changes visible for instance1.
  instance2.exports.set_globals(null, obj4);
  assertEquals(null, instance1.exports.get_global2());
  assertEquals(obj4, instance1.exports.get_global3());

  assertEquals(null, instance2.exports.reexport2.value);
  assertEquals(obj4, instance2.exports.reexport3.value);

  // Check if instance1 can make changes visible for instance2.
  instance1.exports.set_globals(obj2, obj3);
  assertEquals(obj2, instance2.exports.get_global2());
  assertEquals(obj3, instance2.exports.get_global3());

  assertEquals(obj2, instance2.exports.reexport2.value);
  assertEquals(obj3, instance2.exports.reexport3.value);
})();

(function TestImportImmutableAnyFuncGlobalAsAnyRef() {
  print(arguments.callee.name);
  let builder1 = new WasmModuleBuilder();
  const g3 = builder1.addGlobal(kWasmAnyFunc, true).exportAs("e3");
  builder1.addGlobal(kWasmAnyRef, false).exportAs("e1"); // Dummy.
  builder1.addGlobal(kWasmAnyFunc, false).exportAs("e2"); // Dummy.
  const instance1 = builder1.instantiate();

  let builder2 = new WasmModuleBuilder();
  const i1 = builder2.addImportedGlobal('exports', 'e1', kWasmAnyRef, false);
  const i2 = builder2.addImportedGlobal('exports', 'e2', kWasmAnyRef, false);
  builder2.instantiate(instance1);
})();

(function TestImportMutableAnyFuncGlobalAsAnyRefFails() {
  print(arguments.callee.name);
  let builder1 = new WasmModuleBuilder();
  const g3 = builder1.addGlobal(kWasmAnyFunc, true).exportAs("e3");
  builder1.addGlobal(kWasmAnyRef, true).exportAs("e1"); // Dummy.
  builder1.addGlobal(kWasmAnyFunc, true).exportAs("e2"); // Dummy.
  const instance1 = builder1.instantiate();

  let builder2 = new WasmModuleBuilder();
  const i1 = builder2.addImportedGlobal('exports', 'e1', kWasmAnyRef, true);
  const i2 = builder2.addImportedGlobal('exports', 'e2', kWasmAnyRef, true);
  assertThrows(() => builder2.instantiate(instance1), WebAssembly.LinkError);
})();

(function TestRefFuncGlobalInit() {
  print(arguments.callee.name);
  let builder = new WasmModuleBuilder();
  const g_ref = builder.addGlobal(kWasmAnyRef, true);
  const g_func = builder.addGlobal(kWasmAnyFunc, true);
  const f_ref = builder.addFunction('get_anyref_global', kSig_r_v)
                    .addBody([kExprGetGlobal, g_ref.index])
                    .exportAs('get_anyref_global');
  const f_func = builder.addFunction('get_anyfunc_global', kSig_a_v)
                     .addBody([kExprGetGlobal, g_func.index])
                     .exportAs('get_anyfunc_global');

  g_ref.function_index = f_ref.index;
  g_func.function_index = f_func.index;

  const instance = builder.instantiate();
  assertEquals(
      instance.exports.get_anyref_global, instance.exports.get_anyref_global());
  assertEquals(
      instance.exports.get_anyfunc_global,
      instance.exports.get_anyfunc_global());
})();

(function TestRefFuncGlobalInitWithImport() {
  print(arguments.callee.name);
  let builder = new WasmModuleBuilder();
  const sig_index = builder.addType(kSig_i_v);
  const import_wasm = builder.addImport('m', 'wasm', sig_index);
  const import_js = builder.addImport('m', 'js', sig_index);
  const g_wasm = builder.addGlobal(kWasmAnyFunc, true);
  const g_js = builder.addGlobal(kWasmAnyFunc, true);
  g_wasm.function_index = import_wasm;
  g_js.function_index = import_js;
  builder.addFunction('get_global_wasm', kSig_a_v)
      .addBody([kExprGetGlobal, g_wasm.index])
      .exportFunc();
  builder.addFunction('get_global_js', kSig_a_v)
      .addBody([kExprGetGlobal, g_js.index])
      .exportFunc();

  const expected_wasm = dummy_func();
  const expected_val = 27;
  // I want to test here that imported JS functions get wrapped by wasm-to-js
  // and js-to-wasm wrappers. That's why {expected_js} does not return an
  // integer directly but an object with a {valueOf} function.
  function expected_js() {
    const result = {};
    result.valueOf = () => expected_val;
    return result;
  };

  const instance =
      builder.instantiate({m: {wasm: expected_wasm, js: expected_js}});

  assertSame(expected_wasm, instance.exports.get_global_wasm());
  assertSame(expected_val, instance.exports.get_global_js()());
})();
