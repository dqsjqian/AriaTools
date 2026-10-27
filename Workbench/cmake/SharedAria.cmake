# App policy: build one shared Aria SDK, including explicit development sources.
include_guard(GLOBAL)
if(NOT FORGEDEPS_PYTHON_EXECUTABLE AND DEFINED ENV{FORGEDEPS_PYTHON_EXECUTABLE})
    # CI installs the pinned tooling into a specific interpreter (MSYS2's, or
    # the container's) and names it here, so the build uses the same one.
    set(FORGEDEPS_PYTHON_EXECUTABLE "$ENV{FORGEDEPS_PYTHON_EXECUTABLE}" CACHE FILEPATH "ForgeDeps interpreter")
endif()
if(NOT FORGEDEPS_PYTHON_EXECUTABLE)
    find_package(Python3 3.14 REQUIRED COMPONENTS Interpreter)
    set(FORGEDEPS_PYTHON_EXECUTABLE "${Python3_EXECUTABLE}" CACHE FILEPATH "ForgeDeps interpreter")
endif()
set(ForgeDeps_PYTHON_EXECUTABLE "${FORGEDEPS_PYTHON_EXECUTABLE}")
execute_process(COMMAND "${FORGEDEPS_PYTHON_EXECUTABLE}" -c
    "from pathlib import Path; import forgerecipes; print(Path(forgerecipes.__file__).parent / 'cmake' / 'AriaSdk.cmake')"
    RESULT_VARIABLE _sdk_status OUTPUT_VARIABLE _sdk_module ERROR_VARIABLE _sdk_error
    OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NOT _sdk_status EQUAL 0)
    message(FATAL_ERROR "Install requirements-build.txt with ${FORGEDEPS_PYTHON_EXECUTABLE}: ${_sdk_error}")
endif()
include("${_sdk_module}")
if(NOT ARIA_SDK_SOURCE AND DEFINED ENV{ARIA_SDK_SOURCE})
    set(ARIA_SDK_SOURCE "$ENV{ARIA_SDK_SOURCE}")
endif()
if(NOT ARIA_SDK_PREFIX)
    foreach(_feature QT HTTP TLS JNI APPKIT UIKIT SHARED)
        if(NOT DEFINED ARIA_SDK_${_feature})
            set(ARIA_SDK_${_feature} OFF)
        endif()
    endforeach()
    set(_source_args)
    if(ARIA_SDK_SOURCE)
        list(APPEND _source_args SOURCE "${ARIA_SDK_SOURCE}")
    endif()
    forgerecipes_sdk(FILE "${ARIA_DEPENDENCIES_FILE}" ${_source_args}
        QT "${ARIA_SDK_QT}" HTTP "${ARIA_SDK_HTTP}" TLS "${ARIA_SDK_TLS}"
        JNI "${ARIA_SDK_JNI}" APPKIT "${ARIA_SDK_APPKIT}" UIKIT "${ARIA_SDK_UIKIT}"
        SHARED "${ARIA_SDK_SHARED}" EXTRA_PACKAGES json PREFIXES ARIA_SDK_PREFIXES)
    # The recipe package layout (where the CMake config lands inside a prefix)
    # is owned by the recipe layer, not duplicated here.
    forgerecipes_locate_package(PREFIXES ${ARIA_SDK_PREFIXES} PACKAGE aria
        OUT _aria_config_dir PREFIX_OUT ARIA_SDK_PREFIX)
    if(NOT ARIA_SDK_PREFIX)
        message(FATAL_ERROR "The shared environment did not install an Aria SDK")
    endif()
endif()
set(_aria_components core async runtime binding)
foreach(_feature QT HTTP JNI APPKIT UIKIT)
    if(ARIA_SDK_${_feature})
        string(TOLOWER "${_feature}" _component)
        if(_feature STREQUAL "QT")
            set(_component qt6)
        endif()
        list(APPEND _aria_components "${_component}")
    endif()
endforeach()
# A cross build (iOS, Android) re-roots every search prefix under
# CMAKE_FIND_ROOT_PATH and never looks at the SDK the environment just
# installed, so name the config directory. When no prefix is known (a Gradle
# build that passes CMAKE_PREFIX_PATH instead), keep the normal search.
# Find Qt once, before Aria: Qt's EntryPointPrivate config creates its imported
# target unconditionally, so a second find_package(Qt6) in the same configure
# aborts. Aria's package config reuses the components that already exist.
if(ARIA_SDK_QT AND NOT TARGET Qt6::Widgets)
    find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
endif()
if(ARIA_SDK_PREFIX)
    set(aria_DIR "${ARIA_SDK_PREFIX}/lib/cmake/aria" CACHE PATH "Aria package config" FORCE)
    find_package(aria CONFIG REQUIRED COMPONENTS ${_aria_components} NO_DEFAULT_PATH)
else()
    find_package(aria CONFIG REQUIRED COMPONENTS ${_aria_components})
endif()

# Business modules use JSON even when Aria HTTP is disabled. Import it at the
# project scope so sibling module targets see the same selected package. The
# recipe layer owns the layout; a plain config lookup resolves it.
forgerecipes_locate_package(PREFIXES ${ARIA_SDK_PREFIXES} PACKAGE nlohmann_json
    OUT _json_config_dir)
find_package(nlohmann_json CONFIG REQUIRED NO_DEFAULT_PATH)
