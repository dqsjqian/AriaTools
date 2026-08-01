# ============================================================================
#  wb_utils — pure-function utility library. Usable from any layer. Stateless pure functions only, depending solely on the standard library.
# ============================================================================
add_library(wb_utils STATIC
    Base64.cpp
)

target_include_directories(wb_utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..   # include like "utils/Base64.h"
)

target_compile_features(wb_utils PUBLIC cxx_std_20)
