#include "infra/ServiceHub.h"
#include "infra/ServiceFactories.h"

#include <memory>

namespace wb::infra {

ServiceHub::ServiceHub(std::string i18nBaseDir, std::string initialLang) {
    using namespace wb::services;

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

    // 注册进 DI Container（单例实例）。
    container_.register_instance<II18nService>(i18n);
    container_.register_instance<ISettingsService>(settings);
    container_.register_instance<IStorageService>(storage);
    container_.register_instance<ISecretStore>(secret);
    container_.register_instance<ICryptoService>(crypto);
    container_.register_instance<ISyncService>(sync);
}

ServiceHub::~ServiceHub() = default;

}  // namespace wb::infra
