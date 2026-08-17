#pragma once
//
// ThemeModule — IModule implementation + factory declaration.
//
#include "module_api/IModule.h"

namespace wb::theme {

class ThemeModule final : public wb::module_api::IModule {
public:
    std::string id() const override { return "theme"; }
    std::string nav_key() const override { return "nav_theme"; }
    int order() const override { return 13; }
    std::shared_ptr<aria::binding::ViewModel>
        create_view_model(wb::module_api::ModuleContext& ctx) override;
};

}  // namespace wb::theme

namespace wb::module_api { class IModule; }
namespace wb::theme {
std::shared_ptr<wb::module_api::IModule> make_theme_module();
}
