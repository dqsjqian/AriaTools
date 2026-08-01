# ============================================================================
#  wb_infra — stable service layer (low-level app infrastructure), standalone static library.
#  Zero platform UI dependencies. All modules reuse capabilities through this library + wb_module_api.
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

# core root is one level up: include like "infra/i18n/II18nService.h"
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

# Apple platforms use system CommonCrypto + Security, no third-party dynamic dependency.
# Mac/iOS share a real crypto implementation; other platforms fall back to the same-interface stub for now.
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

# HTTP backend: off by default (uses StubHttpClient). Once vendored libcurl+mbedTLS is integrated,
# the upper layer enables WB_HAVE_CURL_HTTP=1 here, compiles in http/CurlHttpClient.cpp, and links curl.
if(NOT DEFINED WB_HAVE_CURL_HTTP)
    target_compile_definitions(wb_infra PUBLIC WB_HAVE_CURL_HTTP=0)
endif()
