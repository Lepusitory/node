# Copyright 2017 the V8 project authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can
# be found in the LICENSE file.

# This file is automatically generated by mkgrokdump and should not
# be modified manually.

# List of known V8 instance types.
INSTANCE_TYPES = {
  0: "INTERNALIZED_STRING_TYPE",
  2: "EXTERNAL_INTERNALIZED_STRING_TYPE",
  8: "ONE_BYTE_INTERNALIZED_STRING_TYPE",
  10: "EXTERNAL_ONE_BYTE_INTERNALIZED_STRING_TYPE",
  18: "EXTERNAL_INTERNALIZED_STRING_WITH_ONE_BYTE_DATA_TYPE",
  34: "SHORT_EXTERNAL_INTERNALIZED_STRING_TYPE",
  42: "SHORT_EXTERNAL_ONE_BYTE_INTERNALIZED_STRING_TYPE",
  50: "SHORT_EXTERNAL_INTERNALIZED_STRING_WITH_ONE_BYTE_DATA_TYPE",
  64: "STRING_TYPE",
  65: "CONS_STRING_TYPE",
  66: "EXTERNAL_STRING_TYPE",
  67: "SLICED_STRING_TYPE",
  69: "THIN_STRING_TYPE",
  72: "ONE_BYTE_STRING_TYPE",
  73: "CONS_ONE_BYTE_STRING_TYPE",
  74: "EXTERNAL_ONE_BYTE_STRING_TYPE",
  75: "SLICED_ONE_BYTE_STRING_TYPE",
  77: "THIN_ONE_BYTE_STRING_TYPE",
  82: "EXTERNAL_STRING_WITH_ONE_BYTE_DATA_TYPE",
  98: "SHORT_EXTERNAL_STRING_TYPE",
  106: "SHORT_EXTERNAL_ONE_BYTE_STRING_TYPE",
  114: "SHORT_EXTERNAL_STRING_WITH_ONE_BYTE_DATA_TYPE",
  128: "SYMBOL_TYPE",
  129: "HEAP_NUMBER_TYPE",
  130: "ODDBALL_TYPE",
  131: "MAP_TYPE",
  132: "CODE_TYPE",
  133: "MUTABLE_HEAP_NUMBER_TYPE",
  134: "FOREIGN_TYPE",
  135: "BYTE_ARRAY_TYPE",
  136: "BYTECODE_ARRAY_TYPE",
  137: "FREE_SPACE_TYPE",
  138: "FIXED_INT8_ARRAY_TYPE",
  139: "FIXED_UINT8_ARRAY_TYPE",
  140: "FIXED_INT16_ARRAY_TYPE",
  141: "FIXED_UINT16_ARRAY_TYPE",
  142: "FIXED_INT32_ARRAY_TYPE",
  143: "FIXED_UINT32_ARRAY_TYPE",
  144: "FIXED_FLOAT32_ARRAY_TYPE",
  145: "FIXED_FLOAT64_ARRAY_TYPE",
  146: "FIXED_UINT8_CLAMPED_ARRAY_TYPE",
  147: "FIXED_DOUBLE_ARRAY_TYPE",
  148: "FILLER_TYPE",
  149: "ACCESSOR_INFO_TYPE",
  150: "ACCESSOR_PAIR_TYPE",
  151: "ACCESS_CHECK_INFO_TYPE",
  152: "INTERCEPTOR_INFO_TYPE",
  153: "FUNCTION_TEMPLATE_INFO_TYPE",
  154: "OBJECT_TEMPLATE_INFO_TYPE",
  155: "ALLOCATION_SITE_TYPE",
  156: "ALLOCATION_MEMENTO_TYPE",
  157: "SCRIPT_TYPE",
  158: "ALIASED_ARGUMENTS_ENTRY_TYPE",
  159: "PROMISE_RESOLVE_THENABLE_JOB_INFO_TYPE",
  160: "PROMISE_REACTION_JOB_INFO_TYPE",
  161: "DEBUG_INFO_TYPE",
  162: "STACK_FRAME_INFO_TYPE",
  163: "PROTOTYPE_INFO_TYPE",
  164: "TUPLE2_TYPE",
  165: "TUPLE3_TYPE",
  166: "CONTEXT_EXTENSION_TYPE",
  167: "MODULE_TYPE",
  168: "MODULE_INFO_ENTRY_TYPE",
  169: "ASYNC_GENERATOR_REQUEST_TYPE",
  170: "FIXED_ARRAY_TYPE",
  171: "HASH_TABLE_TYPE",
  172: "FEEDBACK_VECTOR_TYPE",
  173: "TRANSITION_ARRAY_TYPE",
  174: "PROPERTY_ARRAY_TYPE",
  175: "SHARED_FUNCTION_INFO_TYPE",
  176: "CELL_TYPE",
  177: "WEAK_CELL_TYPE",
  178: "PROPERTY_CELL_TYPE",
  179: "SMALL_ORDERED_HASH_MAP_TYPE",
  180: "SMALL_ORDERED_HASH_SET_TYPE",
  181: "JS_PROXY_TYPE",
  182: "JS_GLOBAL_OBJECT_TYPE",
  183: "JS_GLOBAL_PROXY_TYPE",
  184: "JS_MODULE_NAMESPACE_TYPE",
  185: "JS_SPECIAL_API_OBJECT_TYPE",
  186: "JS_VALUE_TYPE",
  187: "JS_MESSAGE_OBJECT_TYPE",
  188: "JS_DATE_TYPE",
  189: "JS_API_OBJECT_TYPE",
  190: "JS_OBJECT_TYPE",
  191: "JS_ARGUMENTS_TYPE",
  192: "JS_CONTEXT_EXTENSION_OBJECT_TYPE",
  193: "JS_GENERATOR_OBJECT_TYPE",
  194: "JS_ASYNC_GENERATOR_OBJECT_TYPE",
  195: "JS_ARRAY_TYPE",
  196: "JS_ARRAY_BUFFER_TYPE",
  197: "JS_TYPED_ARRAY_TYPE",
  198: "JS_DATA_VIEW_TYPE",
  199: "JS_SET_TYPE",
  200: "JS_MAP_TYPE",
  201: "JS_SET_KEY_VALUE_ITERATOR_TYPE",
  202: "JS_SET_VALUE_ITERATOR_TYPE",
  203: "JS_MAP_KEY_ITERATOR_TYPE",
  204: "JS_MAP_KEY_VALUE_ITERATOR_TYPE",
  205: "JS_MAP_VALUE_ITERATOR_TYPE",
  206: "JS_WEAK_MAP_TYPE",
  207: "JS_WEAK_SET_TYPE",
  208: "JS_PROMISE_CAPABILITY_TYPE",
  209: "JS_PROMISE_TYPE",
  210: "JS_REGEXP_TYPE",
  211: "JS_ERROR_TYPE",
  212: "JS_ASYNC_FROM_SYNC_ITERATOR_TYPE",
  213: "JS_STRING_ITERATOR_TYPE",
  214: "JS_TYPED_ARRAY_KEY_ITERATOR_TYPE",
  215: "JS_FAST_ARRAY_KEY_ITERATOR_TYPE",
  216: "JS_GENERIC_ARRAY_KEY_ITERATOR_TYPE",
  217: "JS_UINT8_ARRAY_KEY_VALUE_ITERATOR_TYPE",
  218: "JS_INT8_ARRAY_KEY_VALUE_ITERATOR_TYPE",
  219: "JS_UINT16_ARRAY_KEY_VALUE_ITERATOR_TYPE",
  220: "JS_INT16_ARRAY_KEY_VALUE_ITERATOR_TYPE",
  221: "JS_UINT32_ARRAY_KEY_VALUE_ITERATOR_TYPE",
  222: "JS_INT32_ARRAY_KEY_VALUE_ITERATOR_TYPE",
  223: "JS_FLOAT32_ARRAY_KEY_VALUE_ITERATOR_TYPE",
  224: "JS_FLOAT64_ARRAY_KEY_VALUE_ITERATOR_TYPE",
  225: "JS_UINT8_CLAMPED_ARRAY_KEY_VALUE_ITERATOR_TYPE",
  226: "JS_FAST_SMI_ARRAY_KEY_VALUE_ITERATOR_TYPE",
  227: "JS_FAST_HOLEY_SMI_ARRAY_KEY_VALUE_ITERATOR_TYPE",
  228: "JS_FAST_ARRAY_KEY_VALUE_ITERATOR_TYPE",
  229: "JS_FAST_HOLEY_ARRAY_KEY_VALUE_ITERATOR_TYPE",
  230: "JS_FAST_DOUBLE_ARRAY_KEY_VALUE_ITERATOR_TYPE",
  231: "JS_FAST_HOLEY_DOUBLE_ARRAY_KEY_VALUE_ITERATOR_TYPE",
  232: "JS_GENERIC_ARRAY_KEY_VALUE_ITERATOR_TYPE",
  233: "JS_UINT8_ARRAY_VALUE_ITERATOR_TYPE",
  234: "JS_INT8_ARRAY_VALUE_ITERATOR_TYPE",
  235: "JS_UINT16_ARRAY_VALUE_ITERATOR_TYPE",
  236: "JS_INT16_ARRAY_VALUE_ITERATOR_TYPE",
  237: "JS_UINT32_ARRAY_VALUE_ITERATOR_TYPE",
  238: "JS_INT32_ARRAY_VALUE_ITERATOR_TYPE",
  239: "JS_FLOAT32_ARRAY_VALUE_ITERATOR_TYPE",
  240: "JS_FLOAT64_ARRAY_VALUE_ITERATOR_TYPE",
  241: "JS_UINT8_CLAMPED_ARRAY_VALUE_ITERATOR_TYPE",
  242: "JS_FAST_SMI_ARRAY_VALUE_ITERATOR_TYPE",
  243: "JS_FAST_HOLEY_SMI_ARRAY_VALUE_ITERATOR_TYPE",
  244: "JS_FAST_ARRAY_VALUE_ITERATOR_TYPE",
  245: "JS_FAST_HOLEY_ARRAY_VALUE_ITERATOR_TYPE",
  246: "JS_FAST_DOUBLE_ARRAY_VALUE_ITERATOR_TYPE",
  247: "JS_FAST_HOLEY_DOUBLE_ARRAY_VALUE_ITERATOR_TYPE",
  248: "JS_GENERIC_ARRAY_VALUE_ITERATOR_TYPE",
  249: "WASM_INSTANCE_TYPE",
  250: "WASM_MEMORY_TYPE",
  251: "WASM_MODULE_TYPE",
  252: "WASM_TABLE_TYPE",
  253: "JS_BOUND_FUNCTION_TYPE",
  254: "JS_FUNCTION_TYPE",
}

# List of known V8 maps.
KNOWN_MAPS = {
  0x02201: (137, "FreeSpaceMap"),
  0x02259: (131, "MetaMap"),
  0x022b1: (130, "NullMap"),
  0x02309: (170, "FixedArrayMap"),
  0x02361: (148, "OnePointerFillerMap"),
  0x023b9: (148, "TwoPointerFillerMap"),
  0x02411: (130, "UninitializedMap"),
  0x02469: (8, "OneByteInternalizedStringMap"),
  0x024c1: (130, "UndefinedMap"),
  0x02519: (129, "HeapNumberMap"),
  0x02571: (130, "TheHoleMap"),
  0x025c9: (130, "BooleanMap"),
  0x02621: (135, "ByteArrayMap"),
  0x02679: (170, "FixedCOWArrayMap"),
  0x026d1: (171, "HashTableMap"),
  0x02729: (128, "SymbolMap"),
  0x02781: (72, "OneByteStringMap"),
  0x027d9: (170, "ScopeInfoMap"),
  0x02831: (175, "SharedFunctionInfoMap"),
  0x02889: (132, "CodeMap"),
  0x028e1: (170, "FunctionContextMap"),
  0x02939: (176, "CellMap"),
  0x02991: (177, "WeakCellMap"),
  0x029e9: (178, "GlobalPropertyCellMap"),
  0x02a41: (134, "ForeignMap"),
  0x02a99: (173, "TransitionArrayMap"),
  0x02af1: (130, "ArgumentsMarkerMap"),
  0x02b49: (130, "ExceptionMap"),
  0x02ba1: (130, "TerminationExceptionMap"),
  0x02bf9: (130, "OptimizedOutMap"),
  0x02c51: (130, "StaleRegisterMap"),
  0x02ca9: (170, "NativeContextMap"),
  0x02d01: (170, "ModuleContextMap"),
  0x02d59: (170, "EvalContextMap"),
  0x02db1: (170, "ScriptContextMap"),
  0x02e09: (170, "BlockContextMap"),
  0x02e61: (170, "CatchContextMap"),
  0x02eb9: (170, "WithContextMap"),
  0x02f11: (147, "FixedDoubleArrayMap"),
  0x02f69: (133, "MutableHeapNumberMap"),
  0x02fc1: (171, "OrderedHashTableMap"),
  0x03019: (170, "SloppyArgumentsElementsMap"),
  0x03071: (179, "SmallOrderedHashMapMap"),
  0x030c9: (180, "SmallOrderedHashSetMap"),
  0x03121: (187, "JSMessageObjectMap"),
  0x03179: (136, "BytecodeArrayMap"),
  0x031d1: (170, "ModuleInfoMap"),
  0x03229: (176, "NoClosuresCellMap"),
  0x03281: (176, "OneClosureCellMap"),
  0x032d9: (176, "ManyClosuresCellMap"),
  0x03331: (174, "PropertyArrayMap"),
  0x03389: (64, "StringMap"),
  0x033e1: (73, "ConsOneByteStringMap"),
  0x03439: (65, "ConsStringMap"),
  0x03491: (77, "ThinOneByteStringMap"),
  0x034e9: (69, "ThinStringMap"),
  0x03541: (67, "SlicedStringMap"),
  0x03599: (75, "SlicedOneByteStringMap"),
  0x035f1: (66, "ExternalStringMap"),
  0x03649: (82, "ExternalStringWithOneByteDataMap"),
  0x036a1: (74, "ExternalOneByteStringMap"),
  0x036f9: (98, "ShortExternalStringMap"),
  0x03751: (114, "ShortExternalStringWithOneByteDataMap"),
  0x037a9: (0, "InternalizedStringMap"),
  0x03801: (2, "ExternalInternalizedStringMap"),
  0x03859: (18, "ExternalInternalizedStringWithOneByteDataMap"),
  0x038b1: (10, "ExternalOneByteInternalizedStringMap"),
  0x03909: (34, "ShortExternalInternalizedStringMap"),
  0x03961: (50, "ShortExternalInternalizedStringWithOneByteDataMap"),
  0x039b9: (42, "ShortExternalOneByteInternalizedStringMap"),
  0x03a11: (106, "ShortExternalOneByteStringMap"),
  0x03a69: (139, "FixedUint8ArrayMap"),
  0x03ac1: (138, "FixedInt8ArrayMap"),
  0x03b19: (141, "FixedUint16ArrayMap"),
  0x03b71: (140, "FixedInt16ArrayMap"),
  0x03bc9: (143, "FixedUint32ArrayMap"),
  0x03c21: (142, "FixedInt32ArrayMap"),
  0x03c79: (144, "FixedFloat32ArrayMap"),
  0x03cd1: (145, "FixedFloat64ArrayMap"),
  0x03d29: (146, "FixedUint8ClampedArrayMap"),
  0x03d81: (157, "ScriptMap"),
  0x03dd9: (172, "FeedbackVectorMap"),
  0x03e31: (170, "DebugEvaluateContextMap"),
  0x03e89: (170, "ScriptContextTableMap"),
  0x03ee1: (171, "UnseededNumberDictionaryMap"),
  0x03f39: (190, "ExternalMap"),
  0x03f91: (106, "NativeSourceStringMap"),
  0x03fe9: (152, "InterceptorInfoMap"),
  0x04041: (208, "JSPromiseCapabilityMap"),
  0x04099: (149, "AccessorInfoMap"),
  0x040f1: (150, "AccessorPairMap"),
  0x04149: (151, "AccessCheckInfoMap"),
  0x041a1: (153, "FunctionTemplateInfoMap"),
  0x041f9: (154, "ObjectTemplateInfoMap"),
  0x04251: (155, "AllocationSiteMap"),
  0x042a9: (156, "AllocationMementoMap"),
  0x04301: (158, "AliasedArgumentsEntryMap"),
  0x04359: (159, "PromiseResolveThenableJobInfoMap"),
  0x043b1: (160, "PromiseReactionJobInfoMap"),
  0x04409: (161, "DebugInfoMap"),
  0x04461: (162, "StackFrameInfoMap"),
  0x044b9: (163, "PrototypeInfoMap"),
  0x04511: (164, "Tuple2Map"),
  0x04569: (165, "Tuple3Map"),
  0x045c1: (166, "ContextExtensionMap"),
  0x04619: (167, "ModuleMap"),
  0x04671: (168, "ModuleInfoEntryMap"),
  0x046c9: (169, "AsyncGeneratorRequestMap"),
}

# List of known V8 objects.
KNOWN_OBJECTS = {
  ("OLD_SPACE", 0x02201): "NullValue",
  ("OLD_SPACE", 0x02231): "EmptyDescriptorArray",
  ("OLD_SPACE", 0x02241): "EmptyFixedArray",
  ("OLD_SPACE", 0x02251): "UninitializedValue",
  ("OLD_SPACE", 0x022d1): "UndefinedValue",
  ("OLD_SPACE", 0x02301): "NanValue",
  ("OLD_SPACE", 0x02311): "TheHoleValue",
  ("OLD_SPACE", 0x02361): "HoleNanValue",
  ("OLD_SPACE", 0x02371): "TrueValue",
  ("OLD_SPACE", 0x023e1): "FalseValue",
  ("OLD_SPACE", 0x02431): "empty_string",
  ("OLD_SPACE", 0x02449): "EmptyScopeInfo",
  ("OLD_SPACE", 0x02459): "ArgumentsMarker",
  ("OLD_SPACE", 0x024b1): "Exception",
  ("OLD_SPACE", 0x02509): "TerminationException",
  ("OLD_SPACE", 0x02569): "OptimizedOut",
  ("OLD_SPACE", 0x025c1): "StaleRegister",
  ("OLD_SPACE", 0x02619): "EmptyByteArray",
  ("OLD_SPACE", 0x02629): "EmptyFixedUint8Array",
  ("OLD_SPACE", 0x02649): "EmptyFixedInt8Array",
  ("OLD_SPACE", 0x02669): "EmptyFixedUint16Array",
  ("OLD_SPACE", 0x02689): "EmptyFixedInt16Array",
  ("OLD_SPACE", 0x026a9): "EmptyFixedUint32Array",
  ("OLD_SPACE", 0x026c9): "EmptyFixedInt32Array",
  ("OLD_SPACE", 0x026e9): "EmptyFixedFloat32Array",
  ("OLD_SPACE", 0x02719): "EmptyFixedFloat64Array",
  ("OLD_SPACE", 0x02739): "EmptyFixedUint8ClampedArray",
  ("OLD_SPACE", 0x02759): "EmptyScript",
  ("OLD_SPACE", 0x027e1): "UndefinedCell",
  ("OLD_SPACE", 0x027f1): "EmptySloppyArgumentsElements",
  ("OLD_SPACE", 0x02811): "EmptySlowElementDictionary",
  ("OLD_SPACE", 0x02859): "EmptyPropertyCell",
  ("OLD_SPACE", 0x02881): "EmptyWeakCell",
  ("OLD_SPACE", 0x02891): "ArrayProtector",
  ("OLD_SPACE", 0x028b9): "IsConcatSpreadableProtector",
  ("OLD_SPACE", 0x028c9): "SpeciesProtector",
  ("OLD_SPACE", 0x028f1): "StringLengthProtector",
  ("OLD_SPACE", 0x02901): "FastArrayIterationProtector",
  ("OLD_SPACE", 0x02911): "ArrayIteratorProtector",
  ("OLD_SPACE", 0x02939): "ArrayBufferNeuteringProtector",
  ("OLD_SPACE", 0x02961): "InfinityValue",
  ("OLD_SPACE", 0x02971): "MinusZeroValue",
  ("OLD_SPACE", 0x02981): "MinusInfinityValue",
}

# List of known V8 Frame Markers.
FRAME_MARKERS = (
  "ENTRY",
  "CONSTRUCT_ENTRY",
  "EXIT",
  "JAVA_SCRIPT",
  "OPTIMIZED",
  "WASM_COMPILED",
  "WASM_TO_JS",
  "JS_TO_WASM",
  "WASM_INTERPRETER_ENTRY",
  "C_WASM_ENTRY",
  "INTERPRETED",
  "STUB",
  "BUILTIN_CONTINUATION",
  "JAVA_SCRIPT_BUILTIN_CONTINUATION",
  "INTERNAL",
  "CONSTRUCT",
  "ARGUMENTS_ADAPTOR",
  "BUILTIN",
  "BUILTIN_EXIT",
)

# This set of constants is generated from a shipping build.
