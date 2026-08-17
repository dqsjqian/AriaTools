#include "module/UnitConvertModule.h"
#include "viewmodels/UnitConvertVm.h"

namespace wb::unitconvert {

std::shared_ptr<aria::binding::ViewModel>
UnitConvertModule::create_view_model(wb::module_api::ModuleContext& ctx) {
        (void)ctx; return std::make_shared<UnitConvertVm>();
}

std::shared_ptr<wb::module_api::IModule> make_unitconvert_module() {
    return std::make_shared<UnitConvertModule>();
}

}  // namespace wb::unitconvert
