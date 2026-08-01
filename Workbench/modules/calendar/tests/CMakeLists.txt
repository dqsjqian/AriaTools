# calendar module standalone test project.
#   cmake -S Workbench/modules/calendar/tests -B build/mac/modules/calendar
#   cmake --build build/mac/modules/calendar
#   ctest --test-dir build/mac/modules/calendar --output-on-failure
cmake_minimum_required(VERSION 3.20)
project(wb_module_calendar_tests LANGUAGES CXX)

enable_testing()

set(WB_MOD_NAME calendar)
get_filename_component(WB_MOD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

get_filename_component(_WB_ROOT "${WB_MOD_DIR}/../.." ABSOLUTE)
include(${_WB_ROOT}/cmake/ModuleTestProject.cmake)

add_executable(calendar_tests main.cpp)
target_link_libraries(calendar_tests PRIVATE
    wb_module_calendar wb_infra wb_module_api wb_utils)
add_test(NAME calendar.business COMMAND calendar_tests)
