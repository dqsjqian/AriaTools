#pragma once
#include "module_api/IModule.h"

namespace wb::tools {

class ToolsModel;
class ToolsService;

class ToolsModule final : public wb::module_api::IModule {
public:
    std::string id() const override { return "tools"; }
    std::string nav_key() const override { return "nav_tools"; }
    int order() const override { return 3; }
    std::shared_ptr<aria::binding::ViewModel>
        create_view_model(wb::module_api::ModuleContext& ctx) override;

private:
    void ensure_initialized_(wb::module_api::ModuleContext& ctx);

    std::shared_ptr<ToolsService> service_;
    std::shared_ptr<ToolsModel> model_;
};

std::shared_ptr<wb::module_api::IModule> make_tools_module();

}  // namespace wb::tools
