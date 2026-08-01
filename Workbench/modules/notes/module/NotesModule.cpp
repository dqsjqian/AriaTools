#include "module/NotesModule.h"

#include "models/NotesModel.h"
#include "services/MarkdownNotesService.h"
#include "viewmodels/NotesVm.h"

namespace wb::notes {

void NotesModule::ensure_initialized_(wb::module_api::ModuleContext& ctx) {
    if (model_) return;
    service_ = std::make_shared<MarkdownNotesService>(ctx.services().storage());
    model_ = std::make_shared<NotesModel>(service_, ctx.bus());
}

std::shared_ptr<aria::binding::ViewModel>
NotesModule::create_view_model(wb::module_api::ModuleContext& ctx) {
    ensure_initialized_(ctx);
    return std::make_shared<NotesVm>(model_);
}

std::shared_ptr<wb::module_api::IModule> make_notes_module() {
    return std::make_shared<NotesModule>();
}

}  // namespace wb::notes
