#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/NotesVm.h"

#include "aria/binding/binding_engine.hpp"

namespace wb::notes::iosview {
static UIViewController* build(NotesVm& vm, aria::binding::BindingEngine& be) {
    UILabel*  title  = wb::ios::ui::make_title(@"");
    UILabel*  status = wb::ios::ui::make_label(@"");
    UIButton* add    = wb::ios::ui::make_button(@"");
    UIButton* del    = wb::ios::ui::make_button(@"");
    auto* vc = wb::ios::ui::make_stack_vc(@[title, status, add, del]);
    be.bind_text_oneway(vm.title, wb::ios::ui::view_for(title));
    be.bind_text_oneway(vm.status, wb::ios::ui::view_for(status));
    be.bind_text_oneway(vm.addLabel, wb::ios::ui::view_for(add));
    be.bind_text_oneway(vm.deleteLabel, wb::ios::ui::view_for(del));
    be.bind_command(vm.addNote, wb::ios::ui::view_for(add));
    be.bind_command(vm.deleteSelected, wb::ios::ui::view_for(del));
    return vc;
}
}  // namespace wb::notes::iosview

namespace wb::notes {
void register_notes_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "notes", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            return iosview::build(static_cast<NotesVm&>(vm), be);
        });
}
}  // namespace wb::notes
