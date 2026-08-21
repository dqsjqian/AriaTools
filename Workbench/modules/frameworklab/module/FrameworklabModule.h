#pragma once

#include "module_api/IModule.h"

namespace wb::frameworklab {

class FrameworklabModule final : public wb::module_api::IModule {
public:
    std::string id() const override { return "frameworklab"; }
    std::string nav_key() const override { return "nav_frameworklab"; }
    int order() const override { return 18; }
    std::shared_ptr<aria::binding::ViewModel>
        create_view_model(wb::module_api::ModuleContext& ctx) override;
};

std::shared_ptr<wb::module_api::IModule> make_frameworklab_module();

}  // namespace wb::frameworklab
