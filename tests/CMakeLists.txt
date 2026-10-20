
cmake_minimum_required(VERSION 3.31.0)

#  Trivial "hello" plugin -- built through asyffi_add_plugin() (Phase 6) to
#  prove the CMake helper produces a loadable plugin end to end.
asyffi_add_plugin(hello ${CMAKE_CURRENT_SOURCE_DIR}/hello/hello.cc LIBNAME hello)

#  Round-trip test

set(ASYFFI_TEST_ASY_BINARY "/home/ai/asymptote/asy")

# Generate the .asy file at build time, substituting the actual .so path via
# the $<TARGET_FILE:hello> generator expression.
file(GENERATE
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/rt_test.asy
    CONTENT
    [[
import "$<TARGET_FILE:hello>" as m;
int r = m.doubleIt(21);
if (r != 42) abort("FFI round-trip failed: expected 42, got " + string(r));
label("ok");
size(1cm, 1cm);
    ]]
)

# The test: a CMake -P script that runs asy and checks exit code.
add_test(
    NAME hello_roundtrip
    COMMAND ${CMAKE_COMMAND}
        -DASY_BIN=${ASYFFI_TEST_ASY_BINARY}
        -DASY_FILE=${CMAKE_CURRENT_BINARY_DIR}/rt_test.asy
        -DOUT_DIR=${CMAKE_CURRENT_BINARY_DIR}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/run_roundtrip.cmake
)


#  Phase 6 tooling: build the single-file template through asyffi_add_plugin()
#  and round-trip it. Acceptance check for the CMake helper + template.
asyffi_add_plugin(
        template_plugin
        ${CMAKE_CURRENT_SOURCE_DIR}/../templates/single_fn_plugin.cc
        LIBNAME template_plugin
)

file(GENERATE
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/rt_template.asy
    CONTENT
    [[
import "$<TARGET_FILE:template_plugin>" as m;
int r = m.doubleIt(21);
if (r != 42) abort("template_plugin doubleIt failed: expected 42, got " + string(r));
label("ok");
size(1cm, 1cm);
    ]]
)

add_test(
    NAME template_plugin_roundtrip
    COMMAND ${CMAKE_COMMAND}
        -DASY_BIN=${ASYFFI_TEST_ASY_BINARY}
        -DASY_FILE=${CMAKE_CURRENT_BINARY_DIR}/rt_template.asy
        -DOUT_DIR=${CMAKE_CURRENT_BINARY_DIR}
        -DOUT_BASE=rt_template
        -DNAME=template_plugin
        -P ${CMAKE_CURRENT_SOURCE_DIR}/run_roundtrip.cmake
)


#  toItem/fromItem value-conversion round-trip test.
#
# A plugin that reads every supported argument type (int64_t, double, bool,
# std::string, std::vector<double>) and returns a std::string and a
# std::vector<double>. The generated .asy driver passes known inputs and
# aborts() if any value comes back wrong.
add_library(
        convert_roundtrip
        SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/convert/convert.cc
)

set_target_properties(convert_roundtrip PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

# The conversion helpers (toItem/fromItem) and the non-inline scalar/string
# ItemFunctions live in the helper static lib.
target_link_libraries(convert_roundtrip PRIVATE asyffi_helper)

file(GENERATE
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/rt_convert.asy
    CONTENT
    [[
import "$<TARGET_FILE:convert_roundtrip>" as m;
string lbl = m.summary(7, 1.5, true, "hi", new real[]{2, 3, 4});
if (lbl != "i=7 b=1 dfinite=1 s=hi n=3")
    abort("convert_roundtrip summary failed: got \"" + lbl + "\"");
real[] got = m.transformVec(7, 1.5, true, "hi", new real[]{2, 3, 4});
real[] want = new real[]{4, 6, 8, 1.5, 7, 1, 2};
if (got.length != 7)
    abort("convert_roundtrip vec length failed: got " + string(got.length));
for (int k = 0; k < 7; ++k)
    if (got[k] != want[k])
        abort("convert_roundtrip vec[" + string(k) + "] failed: got " + string(got[k]) + ", want " + string(want[k]));
label("ok");
size(1cm, 1cm);
    ]]
)

add_test(
    NAME convert_roundtrip
    COMMAND ${CMAKE_COMMAND}
        -DASY_BIN=${ASYFFI_TEST_ASY_BINARY}
        -DASY_FILE=${CMAKE_CURRENT_BINARY_DIR}/rt_convert.asy
        -DOUT_DIR=${CMAKE_CURRENT_BINARY_DIR}
        -DOUT_BASE=rt_convert
        -DNAME=convert_roundtrip
        -P ${CMAKE_CURRENT_SOURCE_DIR}/run_roundtrip.cmake
)


#  TypeInfoOf<T> type-mapping trait test (compile-time static_asserts + runtime)
add_executable(
        typeinfo_test
        ${CMAKE_CURRENT_SOURCE_DIR}/typeinfo/typeinfo_test.cc
)

set_target_properties(typeinfo_test PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

# The trait is header-only; linking the helper lib (whose include dirs are
# PUBLIC) provides both the asyffi header and the asyffihelpers/ headers.
target_link_libraries(typeinfo_test PRIVATE asyffi_helper)

add_test(NAME typeinfo_test COMMAND typeinfo_test)


#  Typed value proxy round-trip test (pair, path).
#
# A plugin that:
#   - midpoint(pair a, pair b) -> pair: driver checks components
#   - makeCircle() -> path: driver checks length > 0
add_library(
        values_roundtrip
        SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/values/values.cc
)

set_target_properties(values_roundtrip PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

target_link_libraries(values_roundtrip PRIVATE asyffi_helper)

file(GENERATE
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/rt_values.asy
    CONTENT
    [[
import "$<TARGET_FILE:values_roundtrip>" as m;
pair p = m.midpoint((1, 2), (3, 4));
if (p.x != 2 || p.y != 3)
    abort("values_roundtrip pair failed: got (" + string(p.x) + ", " + string(p.y) + "), want (2, 3)");
path c = m.makeCircle();
if (length(c) <= 0)
    abort("values_roundtrip path failed: length <= 0");
label("ok");
size(1cm, 1cm);
    ]]
)

add_test(
    NAME values_roundtrip
    COMMAND ${CMAKE_COMMAND}
        -DASY_BIN=${ASYFFI_TEST_ASY_BINARY}
        -DASY_FILE=${CMAKE_CURRENT_BINARY_DIR}/rt_values.asy
        -DOUT_DIR=${CMAKE_CURRENT_BINARY_DIR}
        -DOUT_BASE=rt_values
        -DNAME=values_roundtrip
        -P ${CMAKE_CURRENT_SOURCE_DIR}/run_roundtrip.cmake
)


#  Type-inferred registerFunction round-trip test (Phase 4).
#
# A plugin that registers five functions using ONLY the new
# AsyFfi::registerFunction template (no hand-written metadata):
#   - doubleIt(int) -> int
#   - midpoint(pair, pair) -> pair
#   - addAll(real[]) -> real
#   - makeCircle() -> path
#   - logIt(int) -> void
add_library(
        register_roundtrip
        SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/register/register.cc
)

set_target_properties(register_roundtrip PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

target_link_libraries(register_roundtrip PRIVATE asyffi_helper)

file(GENERATE
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/rt_register.asy
    CONTENT
    [[
import "$<TARGET_FILE:register_roundtrip>" as m;
int r = m.doubleIt(21);
if (r != 42) abort("register_roundtrip doubleIt failed: expected 42, got " + string(r));
pair p = m.midpoint((1, 2), (3, 4));
if (p.x != 2 || p.y != 3)
    abort("register_roundtrip midpoint failed: got (" + string(p.x) + ", " + string(p.y) + "), want (2, 3)");
real s = m.addAll(new real[]{1, 2, 3, 4});
if (s != 10) abort("register_roundtrip addAll failed: expected 10, got " + string(s));
path c = m.makeCircle();
if (length(c) <= 0) abort("register_roundtrip makeCircle failed: length <= 0");
m.logIt(5);
label("ok");
size(1cm, 1cm);
    ]]
)

add_test(
    NAME register_roundtrip
    COMMAND ${CMAKE_COMMAND}
        -DASY_BIN=${ASYFFI_TEST_ASY_BINARY}
        -DASY_FILE=${CMAKE_CURRENT_BINARY_DIR}/rt_register.asy
        -DOUT_DIR=${CMAKE_CURRENT_BINARY_DIR}
        -DOUT_BASE=rt_register
        -DNAME=register_roundtrip
        -P ${CMAKE_CURRENT_SOURCE_DIR}/run_roundtrip.cmake
)


#  AsyFfi::call round-trip test (Phase 5): C++ calls back into Asymptote.
#
# A plugin that invokes three Asymptote functions through AsyFfi::call and
# returns the results; the driver verifies each value and aborts() on a
# mismatch:
#   - sqrtOf    : top-level built-in   (module == nullptr)
#   - shiftOf   : built-in returning a structural type (transform)
#   - hypotOf   : function from the imported "gsl" module (module != nullptr)
add_library(
        call_roundtrip
        SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/call/call.cc
)

set_target_properties(call_roundtrip PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

target_link_libraries(call_roundtrip PRIVATE asyffi_helper)

file(GENERATE
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/rt_call.asy
    CONTENT
    [[
import "$<TARGET_FILE:call_roundtrip>" as m;
import gsl;

// 1) Top-level C++ built-in (module == nullptr).
real s = m.sqrtOf(25);
if (s != 5) abort("call_roundtrip sqrtOf failed: expected 5, got " + string(s));

// 2) Built-in returning a structural type: shift((3,4)) is a transform; apply
//    it to the origin and check the translation landed at (3,4).
transform t = m.shiftOf(3, 4);
pair tp = t*(0, 0);
if (tp.x != 3 || tp.y != 4)
    abort("call_roundtrip shiftOf failed: expected (3,4), got (" + string(tp.x) + ", " + string(tp.y) + ")");

// 3) Function resolved from an imported module (module != nullptr).
real h = m.hypotOf(3, 4);
if (h != 5) abort("call_roundtrip hypotOf failed: expected 5, got " + string(h));

label("ok");
size(1cm, 1cm);
    ]]
)

add_test(
    NAME call_roundtrip
    COMMAND ${CMAKE_COMMAND}
        -DASY_BIN=${ASYFFI_TEST_ASY_BINARY}
        -DASY_FILE=${CMAKE_CURRENT_BINARY_DIR}/rt_call.asy
        -DOUT_DIR=${CMAKE_CURRENT_BINARY_DIR}
        -DOUT_BASE=rt_call
        -DNAME=call_roundtrip
        -P ${CMAKE_CURRENT_SOURCE_DIR}/run_roundtrip.cmake
)


#  Ergonomic round-trip test: a single plugin demonstrating both the
#  type-inferred registerFunction (Phase 4) and asy::call (Phase 5).
asyffi_add_plugin(
    fnsSample
    ${CMAKE_CURRENT_SOURCE_DIR}/fnsSample/fnsSample.cc
    LIBNAME fnsSample
)

file(GENERATE
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/rt_fnsSample.asy
    CONTENT
    [[
import "$<TARGET_FILE:fnsSample>" as m;
real s = m.addTwo(3, 4);
if (s != 7) abort("fnsSample addTwo failed: expected 7, got " + string(s));
real sp = m.sqrtPlus(9);
if (sp != 12) abort("fnsSample sqrtPlus failed: expected 12 (sqrt(9)+9), got " + string(sp));
label("ok");
size(1cm, 1cm);
    ]]
)

add_test(
    NAME fnsSample_roundtrip
    COMMAND ${CMAKE_COMMAND}
        -DASY_BIN=${ASYFFI_TEST_ASY_BINARY}
        -DASY_FILE=${CMAKE_CURRENT_BINARY_DIR}/rt_fnsSample.asy
        -DOUT_DIR=${CMAKE_CURRENT_BINARY_DIR}
        -DOUT_BASE=rt_fnsSample
        -DNAME=fnsSample
        -P ${CMAKE_CURRENT_SOURCE_DIR}/run_roundtrip.cmake
)

