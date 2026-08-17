# CMakeLists — Workbench Android JNI bridge (aria_jni.so).
#
# Links the NDK cross-built core static libs from scripts/gen-android.sh
# (stage 1). All include dirs are the same set the core build itself uses
# (see build/platforms/android/compile_commands.json).
#
# Required: -DWB_NATIVE_ROOT=<AriaTools repo root> (set by app/build.gradle.kts).
cmake_minimum_required(VERSION 3.22.1)

project(wb_aria_jni)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT WB_NATIVE_ROOT)
    message(FATAL_ERROR "WB_NATIVE_ROOT not set (AriaTools repo root)")
endif()

set(WB_ROOT  "${WB_NATIVE_ROOT}/Workbench")
set(ARIA_ROOT "${WB_NATIVE_ROOT}/third_party/aria")
set(WB_LIB   "${WB_NATIVE_ROOT}/build/platforms/android/lib")

# ── JNI side-channel bridge ─────────────────────────────────────────────
# C++ AppCore / VMs → Property on_changed → JNI callback →
# Kotlin MutableStateFlow → Compose recomposition (same as Aria demo5).
add_library(aria_jni SHARED
    "${WB_ROOT}/platform/android/jni/jni_bridge.cpp"
    "${WB_ROOT}/platform/android/jni/AndroidShell.cpp"
)

# ── Include directories (mirror the core build's set) ────────────────────
target_include_directories(aria_jni PRIVATE
    ${WB_ROOT}/core
    ${WB_ROOT}/modules/dashboard
    ${WB_ROOT}/modules/notes
    ${WB_ROOT}/modules/calendar
    ${WB_ROOT}/modules/tools
    ${WB_ROOT}/modules/settings
    ${WB_ROOT}/modules/sync
    ${ARIA_ROOT}/third_party/nlohmann_json/include
    ${ARIA_ROOT}/modules/abi/include
    ${ARIA_ROOT}/modules/adapters/jni/include
    ${ARIA_ROOT}/modules/async/include
    ${ARIA_ROOT}/modules/binding/include
    ${ARIA_ROOT}/modules/core/include
    ${ARIA_ROOT}/modules/runtime/include
)

# ── Link Workbench + Aria static archives ─────────────────────────────────
# Order matters: dependent libs first (AppCore → modules → infra → api →
# utils → jni adapter → binding → runtime → abi).
target_link_libraries(aria_jni
    ${WB_LIB}/libwb_core_app.a
    ${WB_LIB}/libwb_module_dashboard.a
    ${WB_LIB}/libwb_module_notes.a
    ${WB_LIB}/libwb_module_calendar.a
    ${WB_LIB}/libwb_module_tools.a
    ${WB_LIB}/libwb_module_settings.a
    ${WB_LIB}/libwb_module_sync.a
    ${WB_LIB}/libwb_module_tipcalc.a
    ${WB_LIB}/libwb_module_unitconvert.a
    ${WB_LIB}/libwb_module_cart.a
    ${WB_LIB}/libwb_module_signup.a
    ${WB_LIB}/libwb_module_search.a
    ${WB_LIB}/libwb_module_login.a
    ${WB_LIB}/libwb_module_chat.a
    ${WB_LIB}/libwb_module_theme.a
    ${WB_LIB}/libwb_module_wizard.a
    ${WB_LIB}/libwb_infra.a
    ${WB_LIB}/libwb_module_api.a
    ${WB_LIB}/libwb_utils.a
    ${WB_LIB}/libaria_adapters_jni.a
    ${WB_LIB}/libaria_binding.a
    ${WB_LIB}/libaria_runtime.a
    ${WB_LIB}/libaria_abi.a
    android
    log
)

# ── Flags ────────────────────────────────────────────────────────────────
target_compile_options(aria_jni PRIVATE
    -Wall -Wextra
    -fvisibility=hidden
)

target_compile_definitions(aria_jni PRIVATE
    ARIA_BINDING_STATIC
    ARIA_JNI_STATIC
)
