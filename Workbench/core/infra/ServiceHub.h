#pragma once
//
// ServiceHub — Stable service layer aggregate (app-level low-level infrastructure).
//
// Responsibilities:
//   * Hold and assemble all "stable services" (i18n/storage/secret/crypto/settings/sync);
//   * Use aria::runtime::Container for dependency injection (register_instance);
//   * Hold the single aria::runtime::EventBus for cross-module communication.
//
// Modules obtain a ServiceHub& only via ModuleContext, fetching interfaces
// through service<I>(); they never new services themselves and don't know
// concrete implementation types.
//
#include "infra/i18n/II18nService.h"
#include "infra/storage/IStorageService.h"
#include "infra/secret/ISecretStore.h"
#include "infra/crypto/ICryptoService.h"
#include "infra/settings/ISettingsService.h"
#include "infra/sync/ISyncService.h"
#include "infra/http/IHttpClient.h"

#include "aria/async/executor.hpp"
#include "aria/runtime/container.hpp"
#include "aria/runtime/event_bus.hpp"
#include "aria/scheduler.hpp"

#include <memory>
#include <string>

namespace wb::infra {

class ServiceHub {
public:
    /// i18nBaseDir: i18n resource root directory (per-module <mod>/ subdirectories).
    /// initialLang: startup language.
    explicit ServiceHub(std::string i18nBaseDir, std::string initialLang = "zh-CN");
    ~ServiceHub();

    ServiceHub(const ServiceHub&) = delete;
    ServiceHub& operator=(const ServiceHub&) = delete;

    /// Fetch a stable service (resolved via the DI Container).
    template <typename I>
    [[nodiscard]] I& service() { return *container_.resolve<I>(); }

    [[nodiscard]] aria::runtime::EventBus& bus() { return bus_; }
    [[nodiscard]] aria::runtime::Container& container() { return container_; }

    // ── Platform executors (injected by each platform shell before the VMs
    //    are created; business VMs pull them from ModuleContext) ───────────
    /// UI-thread executor. Null → InlineExecutor (synchronous) fallback.
    void set_ui_executor(aria::async::IExecutor* e) { ui_exec_ = e; }
    /// Delayed scheduler (debounce/throttle), usually backed by the UI loop.
    /// Null → a no-op immediate scheduler fallback.
    void set_timer(aria::IDelayedScheduler* t) { timer_ = t; }

    [[nodiscard]] aria::async::IExecutor&     ui_exec();
    [[nodiscard]] aria::async::ThreadPoolExecutor& worker() { return worker_; }
    [[nodiscard]] aria::IDelayedScheduler&    timer();

    // Convenience direct access (high-frequency services).
    [[nodiscard]] services::II18nService&     i18n()     { return service<services::II18nService>(); }
    [[nodiscard]] services::IStorageService&  storage()  { return service<services::IStorageService>(); }
    [[nodiscard]] services::ISecretStore&     secret()   { return service<services::ISecretStore>(); }
    [[nodiscard]] services::ICryptoService&   crypto()   { return service<services::ICryptoService>(); }
    [[nodiscard]] services::ISettingsService& settings() { return service<services::ISettingsService>(); }
    [[nodiscard]] services::ISyncService&     sync()     { return service<services::ISyncService>(); }
    [[nodiscard]] services::IHttpClient&      http()     { return service<services::IHttpClient>(); }

private:
    aria::runtime::Container container_;
    aria::runtime::EventBus  bus_;

    aria::async::IExecutor*          ui_exec_ = nullptr;   ///< injected by shell
    aria::IDelayedScheduler*         timer_   = nullptr;   ///< injected by shell
    aria::async::ThreadPoolExecutor  worker_{2};           ///< shared worker pool
    aria::async::InlineExecutor      inline_exec_;         ///< fallback for ui_exec
};

}  // namespace wb::infra
