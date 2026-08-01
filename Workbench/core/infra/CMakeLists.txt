# ============================================================================
#  wb_infra — 稳定服务层（app 底层基础设施），独立静态库。
#  零平台 UI 依赖。所有模块经此库 + wb_module_api 复用能力。
# ============================================================================
add_library(wb_infra STATIC
    ServiceHub.cpp
    log/Log.cpp
    i18n/XmlI18nService.cpp
    i18n/I18n.cpp
    storage/LocalFileStorageService.cpp
    secret/StubSecretStore.cpp
    crypto/StubCryptoService.cpp
    settings/InMemorySettingsService.cpp
    sync/StubSyncService.cpp
    http/HttpResponse.cpp
    http/StubHttpClient.cpp
)

# core 根在上一级：include 形如 "infra/i18n/II18nService.h"
target_include_directories(wb_infra PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(wb_infra PUBLIC
    wb_utils
    aria::binding
    aria::runtime
    aria::async
    aria::core
)

# Apple 平台使用系统 CommonCrypto + Security，无第三方动态依赖，
# Mac/iOS 统一真实加密实现；其他平台暂回退到同接口 stub。
if(APPLE)
    target_sources(wb_infra PRIVATE crypto/AppleCryptoService.cpp)
    target_link_libraries(wb_infra PRIVATE
        "-framework Security"
        "-framework CoreFoundation")
    target_compile_definitions(wb_infra PUBLIC WB_HAVE_APPLE_CRYPTO=1)
else()
    target_compile_definitions(wb_infra PUBLIC WB_HAVE_APPLE_CRYPTO=0)
endif()

target_compile_features(wb_infra PUBLIC cxx_std_20)

# HTTP 后端：默认关闭（用 StubHttpClient）。vendored libcurl+mbedTLS 接入后，
# 由上层在此打开 WB_HAVE_CURL_HTTP=1 并编入 http/CurlHttpClient.cpp、链接 curl。
if(NOT DEFINED WB_HAVE_CURL_HTTP)
    target_compile_definitions(wb_infra PUBLIC WB_HAVE_CURL_HTTP=0)
endif()
