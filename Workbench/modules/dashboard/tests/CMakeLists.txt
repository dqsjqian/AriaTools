# dashboard module standalone test project (navigation / VM-layer routing).
#   cmake -S Workbench/modules/dashboard/tests -B build/mac/modules/dashboard
#   cmake --build build/mac/modules/dashboard
#   ctest --test-dir build/mac/modules/dashboard --output-on-failure
cmake_minimum_required(VERSION 3.20)
project(wb_module_dashboard_tests LANGUAGES CXX)

enable_testing()

set(WB_MOD_NAME dashboard)
get_filename_component(WB_MOD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

get_filename_component(_WB_ROOT "${WB_MOD_DIR}/../.." ABSOLUTE)
include(${_WB_ROOT}/cmake/ModuleTestProject.cmake)

# Cross-module navigation test pushes a fresh cart VM; bring the cart module
# into this standalone test project (core sources only, no platform UI).
add_subdirectory(${_WB_ROOT}/modules/cart ${CMAKE_BINARY_DIR}/wb_module_cart)

add_executable(dashboard_tests main.cpp)
target_link_libraries(dashboard_tests PRIVATE
    wb_module_dashboard wb_module_cart
    wb_module_api wb_infra wb_utils)
add_test(NAME dashboard.navigation COMMAND dashboard_tests)
