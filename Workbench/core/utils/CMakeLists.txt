# ============================================================================
#  wb_utils — 纯函数工具库。全层可依赖。只放无状态纯函数、只依赖标准库。
# ============================================================================
add_library(wb_utils STATIC
    Base64.cpp
)

target_include_directories(wb_utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..   # include 形如 "utils/Base64.h"
)

target_compile_features(wb_utils PUBLIC cxx_std_20)
