#include "module/ThemeModule.h"
#include "viewmodels/ThemeVm.h"

namespace wb::theme {

std::shared_ptr<aria::binding::ViewModel>
ThemeModule::create_view_model(wb::module_api::ModuleContext& ctx) {
        (void)ctx; return std::make_shared<ThemeVm>();
}

std::shared_ptr<wb::module_api::IModule> make_theme_module() {
    return std::make_shared<ThemeModule>();
}

}  // namespace wb::theme
