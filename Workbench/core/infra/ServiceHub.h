#pragma once
//
// ServiceHub — 稳定服务层聚合（app 底层基础设施）。
//
// 职责：
//   * 持有并装配所有「稳定服务」（i18n/storage/secret/crypto/settings/sync）；
//   * 用 aria::runtime::Container 做依赖注入（register_instance）；
//   * 持有唯一的 aria::runtime::EventBus 供跨模块通信。
//
// 模块只经 ModuleContext 拿到 ServiceHub&，通过 service<I>() 取接口，
// 不自行 new 服务，也不认识具体实现类型。
//
#include "infra/i18n/II18nService.h"
#include "infra/storage/IStorageService.h"
#include "infra/secret/ISecretStore.h"
#include "infra/crypto/ICryptoService.h"
#include "infra/settings/ISettingsService.h"
#include "infra/sync/ISyncService.h"
#include "infra/http/IHttpClient.h"

#include "aria/runtime/container.hpp"
#include "aria/runtime/event_bus.hpp"

#include <memory>
#include <string>

namespace wb::infra {

class ServiceHub {
public:
    /// i18nBaseDir：i18n 资源根目录（各模块文案汇聚于此的 <mod>/ 子目录）。
    /// initialLang：启动语言。
    explicit ServiceHub(std::string i18nBaseDir, std::string initialLang = "zh-CN");
    ~ServiceHub();

    ServiceHub(const ServiceHub&) = delete;
    ServiceHub& operator=(const ServiceHub&) = delete;

    /// 取稳定服务（经 DI Container 解析）。
    template <typename I>
    [[nodiscard]] I& service() { return *container_.resolve<I>(); }

    [[nodiscard]] aria::runtime::EventBus& bus() { return bus_; }
    [[nodiscard]] aria::runtime::Container& container() { return container_; }

    // 便捷直取（高频服务）。
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
};

}  // namespace wb::infra
