# Reuse the actual assembled module libraries: tests exercise the same targets
# as the app and write only under their binary working directories.
foreach(module calendar cart dashboard frameworklab notes tools)
    add_executable(wb_${module}_tests "../modules/${module}/tests/main.cpp")
    target_link_libraries(wb_${module}_tests PRIVATE wb_module_${module})
    if(module STREQUAL "dashboard")
        target_link_libraries(wb_${module}_tests PRIVATE wb_module_cart)
    endif()
    add_test(NAME tools.${module} COMMAND wb_${module}_tests)
endforeach()
add_executable(wb_i18n_tests i18n/main.cpp)
target_link_libraries(wb_i18n_tests PRIVATE wb_module_api)
add_test(NAME tools.i18n COMMAND wb_i18n_tests "${CMAKE_CURRENT_BINARY_DIR}/i18n-fixtures")

add_executable(wb_application_tests application/main.cpp)
target_link_libraries(wb_application_tests PRIVATE wb_core_app)
list(LENGTH WB_MODULES _module_count)
target_compile_definitions(wb_application_tests PRIVATE WB_EXPECTED_MODULES=${_module_count})
add_test(NAME tools.application COMMAND wb_application_tests
    "${WB_RUNTIME_I18N_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/application-fixtures")

if(WORKBENCH_TARGET_QT)
    add_test(NAME tools.qt_probe COMMAND $<TARGET_FILE:workbench> --probe)
    set_tests_properties(tools.qt_probe PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endif()
