#include "infra/ServiceHub.h"
#include "infra/ServiceFactories.h"
#include "infra/i18n/I18n.h"

#include <filesystem>
#include <memory>

namespace wb::infra {

ServiceHub::ServiceHub(std::string i18nBaseDir, std::string initialLang) {
    using namespace wb::services;

    // 资源根 = i18n 目录的上一级；CA bundle 约定在 <resources>/ca/cacert.pem。
    const std::string caBundle =
        (std::filesystem::path(i18nBaseDir).parent_path() / "ca" / "cacert.pem").string();

    // i18n（文案资源目录由各模块汇聚而来）。
    std::shared_ptr<II18nService> i18n{
        make_xml_i18n_service(std::move(i18nBaseDir), std::move(initialLang))};

    // 设置服务（先建，dataDir 供 storage 使用）。
    std::shared_ptr<ISettingsService> settings{make_in_memory_settings_service()};
    settings->load();

    std::shared_ptr<IStorageService> storage{
        make_local_file_storage_service(settings->sync().dataDir)};
    std::shared_ptr<ISecretStore> secret{make_stub_secret_store()};
#if WB_HAVE_APPLE_CRYPTO
    std::shared_ptr<ICryptoService> crypto{make_apple_crypto_service()};
#else
    std::shared_ptr<ICryptoService> crypto{make_stub_crypto_service()};
#endif
    std::shared_ptr<ISyncService> sync{make_stub_sync_service()};

    // HTTP 抓取服务：curl+mbedTLS 接入后用真实实现（CA bundle 随资源打包），
    // 未接入前先用 stub 保证 DI 装配完整、app 可构建。
#if WB_HAVE_CURL_HTTP
    std::shared_ptr<IHttpClient> http{make_curl_http_client(caBundle)};
#else
    (void)caBundle;
    std::shared_ptr<IHttpClient> http{make_stub_http_client()};
#endif

    // 注册进 DI Container（单例实例）。
    container_.register_instance<II18nService>(i18n);
    container_.register_instance<ISettingsService>(settings);
    container_.register_instance<IStorageService>(storage);
    container_.register_instance<ISecretStore>(secret);
    container_.register_instance<ICryptoService>(crypto);
    container_.register_instance<ISyncService>(sync);
    container_.register_instance<IHttpClient>(http);

    // 把 i18n 后端注入全局门面，使 wb::i18n::str/bind 无需逐 VM 注入即可用。
    wb::i18n::set_backend(i18n.get());
}

ServiceHub::~ServiceHub() = default;

}  // namespace wb::infra
