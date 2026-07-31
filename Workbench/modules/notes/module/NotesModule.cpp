#include "module/NotesModule.h"
#include "viewmodels/NotesVm.h"

namespace wb::notes {

std::shared_ptr<aria::binding::ViewModel>
NotesModule::create_view_model(wb::module_api::ModuleContext& ctx) {
    return std::make_shared<NotesVm>(ctx.services().i18n());
}

std::shared_ptr<wb::module_api::IModule> make_notes_module() {
    return std::make_shared<NotesModule>();
}

}  // namespace wb::notes
