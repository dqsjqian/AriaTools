#include "infra/ServiceHub.h"
#include "infra/ServiceFactories.h"
#include "infra/i18n/I18n.h"

#include <filesystem>
#include <functional>
#include <memory>

namespace wb::infra {

namespace {

/// No-op fallback timer: fires immediately, ignoring the delay. Used only
/// until a platform shell injects a real UI-loop-backed scheduler.
class InlineTimer final : public aria::IDelayedScheduler {
public:
    void post_after(std::chrono::milliseconds, std::function<void()> fn) override {
        fn();
    }
};

}  // namespace

aria::async::IExecutor& ServiceHub::ui_exec() {
    return ui_exec_ ? *ui_exec_ : inline_exec_;
}

aria::IDelayedScheduler& ServiceHub::timer() {
    static InlineTimer fallback;
    return timer_ ? *timer_ : fallback;
}

ServiceHub::ServiceHub(std::string i18nBaseDir, std::string initialLang) {
    using namespace wb::services;

    // Resource root = parent of the i18n directory; CA bundle is expected at <resources>/ca/cacert.pem.
    const std::string caBundle =
        (std::filesystem::path(i18nBaseDir).parent_path() / "ca" / "cacert.pem").string();

    // i18n (text resource directory aggregates per-module subdirectories).
    std::shared_ptr<II18nService> i18n{
        make_xml_i18n_service(std::move(i18nBaseDir), std::move(initialLang))};

    // Settings service (create first; dataDir is used by storage).
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

    // HTTP fetch service: use real implementation once curl+mbedTLS is integrated
    // (CA bundle bundled with resources); until then use a stub to keep DI
    // wiring complete and the app buildable.
#if WB_HAVE_CURL_HTTP
    std::shared_ptr<IHttpClient> http{make_curl_http_client(caBundle)};
#else
    (void)caBundle;
    std::shared_ptr<IHttpClient> http{make_stub_http_client()};
#endif

    // Register into the DI Container (singleton instances).
    container_.register_instance<II18nService>(i18n);
    container_.register_instance<ISettingsService>(settings);
    container_.register_instance<IStorageService>(storage);
    container_.register_instance<ISecretStore>(secret);
    container_.register_instance<ICryptoService>(crypto);
    container_.register_instance<ISyncService>(sync);
    container_.register_instance<IHttpClient>(http);

    // Inject the i18n backend into the global facade so wb::i18n::str/bind
    // work without per-VM injection.
    wb::i18n::set_backend(i18n.get());
}

ServiceHub::~ServiceHub() = default;

}  // namespace wb::infra
