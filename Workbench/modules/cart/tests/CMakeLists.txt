# cart module standalone test project.
cmake_minimum_required(VERSION 3.20)
project(wb_module_cart_tests LANGUAGES CXX)

enable_testing()

set(WB_MOD_NAME cart)
get_filename_component(WB_MOD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
get_filename_component(_WB_ROOT "${WB_MOD_DIR}/../.." ABSOLUTE)
include(${_WB_ROOT}/cmake/ModuleTestProject.cmake)

add_executable(cart_tests main.cpp)
target_link_libraries(cart_tests PRIVATE
    wb_module_cart wb_infra wb_module_api wb_utils)
add_test(NAME cart.business COMMAND cart_tests)
