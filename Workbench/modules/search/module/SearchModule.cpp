#include "module/SearchModule.h"
#include "viewmodels/SearchVm.h"
#include "viewmodels/SearchVmHostVm.h"

namespace wb::search {

std::shared_ptr<aria::binding::ViewModel>
SearchModule::create_view_model(wb::module_api::ModuleContext& ctx) {
        return std::make_shared<SearchVmHostVm>(ctx.timer());
}

std::shared_ptr<wb::module_api::IModule> make_search_module() {
    return std::make_shared<SearchModule>();
}

}  // namespace wb::search
