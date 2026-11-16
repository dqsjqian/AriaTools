#include "module/TipcalcModule.h"
#include "viewmodels/TipCalcVm.h"

namespace wb::tipcalc {

std::shared_ptr<aria::binding::ViewModel>
TipcalcModule::create_view_model(wb::module_api::ModuleContext& ctx) {
    return std::make_shared<TipCalcVm>(ctx.i18n_context());
}

std::shared_ptr<wb::module_api::IModule> make_tipcalc_module() {
    return std::make_shared<TipcalcModule>();
}

}  // namespace wb::tipcalc
