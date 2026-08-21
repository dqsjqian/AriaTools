cmake_minimum_required(VERSION 3.20)
project(wb_module_frameworklab_tests LANGUAGES CXX)

enable_testing()

set(WB_MOD_NAME frameworklab)
get_filename_component(WB_MOD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
get_filename_component(_WB_ROOT "${WB_MOD_DIR}/../.." ABSOLUTE)
include(${_WB_ROOT}/cmake/ModuleTestProject.cmake)

add_executable(frameworklab_tests main.cpp)
target_link_libraries(frameworklab_tests PRIVATE
    wb_module_frameworklab wb_infra wb_module_api wb_utils)
add_test(NAME frameworklab.business COMMAND frameworklab_tests)
