#include "module/FrameworklabModule.h"
#include "viewmodels/FrameworkLabVm.h"

namespace wb::frameworklab {

std::shared_ptr<aria::binding::ViewModel>
FrameworklabModule::create_view_model(wb::module_api::ModuleContext& ctx) {
    return std::make_shared<FrameworkLabVm>(ctx.i18n_context());
}

std::shared_ptr<wb::module_api::IModule> make_frameworklab_module() {
    return std::make_shared<FrameworklabModule>();
}

}  // namespace wb::frameworklab
