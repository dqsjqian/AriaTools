#pragma once
#include "module_api/IModule.h"

namespace wb::notes {

class NotesModel;
class MarkdownNotesService;

class NotesModule final : public wb::module_api::IModule {
public:
    std::string id() const override { return "notes"; }
    std::string nav_key() const override { return "nav_notes"; }
    int order() const override { return 1; }
    std::shared_ptr<aria::binding::ViewModel>
        create_view_model(wb::module_api::ModuleContext& ctx) override;

private:
    void ensure_initialized_(wb::module_api::ModuleContext& ctx);

    std::shared_ptr<MarkdownNotesService> service_;
    std::shared_ptr<NotesModel> model_;
};

std::shared_ptr<wb::module_api::IModule> make_notes_module();

}  // namespace wb::notes
