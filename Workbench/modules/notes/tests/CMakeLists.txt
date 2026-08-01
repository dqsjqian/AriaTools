# notes module standalone test project.
#   cmake -S Workbench/modules/notes/tests -B build/mac/modules/notes
#   cmake --build build/mac/modules/notes
#   ctest --test-dir build/mac/modules/notes --output-on-failure
cmake_minimum_required(VERSION 3.20)
project(wb_module_notes_tests LANGUAGES CXX)

enable_testing()

set(WB_MOD_NAME notes)
get_filename_component(WB_MOD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

get_filename_component(_WB_ROOT "${WB_MOD_DIR}/../.." ABSOLUTE)
include(${_WB_ROOT}/cmake/ModuleTestProject.cmake)

add_executable(notes_tests main.cpp)
target_link_libraries(notes_tests PRIVATE
    wb_module_notes wb_infra wb_module_api wb_utils)
add_test(NAME notes.business COMMAND notes_tests)
