#pragma once
//
// DashboardModule — dashboard 模块的 IModule 实现 + 工厂声明。
//
#include "module_api/IModule.h"

namespace wb::dashboard {

class DashboardModule final : public wb::module_api::IModule {
public:
    std::string id() const override { return "dashboard"; }
    std::string nav_key() const override { return "nav_dashboard"; }
    int order() const override { return 0; }
    std::shared_ptr<aria::binding::ViewModel>
        create_view_model(wb::module_api::ModuleContext& ctx) override;
};

}  // namespace wb::dashboard

// 工厂：app 层 ModulesManifest 显式调用（避免静态库全局注册被裁剪）。
namespace wb::module_api { class IModule; }
namespace wb::dashboard {
std::shared_ptr<wb::module_api::IModule> make_dashboard_module();
}
