#include "NotesView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/NotesVm.h"

#include "aria/binding/binding_engine.hpp"

namespace wb::notes::iosview {

NotesView::NotesView(NotesVm& vm, aria::binding::BindingEngine& be)
    : vc_(nil) {
    UILabel*     title  = wb::ios::ui::make_title(@"");
    UILabel*     hint   = wb::ios::ui::make_label(@"");
    UILabel*     status = wb::ios::ui::make_label(@"");
    UITextField* editTitle = wb::ios::ui::make_field(@"");
    UITextField* editBody  = wb::ios::ui::make_field(@"");
    UIButton*    add    = wb::ios::ui::make_button(@"");
    UIButton*    save   = wb::ios::ui::make_button(@"");
    UIButton*    del    = wb::ios::ui::make_button(@"");

    vc_ = wb::ios::ui::make_stack_vc(
        @[title, hint, status, editTitle, editBody, add, save, del]);

    be.bind_text_oneway(vm.title, wb::ios::ui::view_for(title));
    be.bind_text_oneway(vm.hint, wb::ios::ui::view_for(hint));
    be.bind_text_oneway(vm.status, wb::ios::ui::view_for(status));
    be.bind_text(vm.editTitle, wb::ios::ui::view_for(editTitle));
    be.bind_text(vm.editBody, wb::ios::ui::view_for(editBody));
    be.bind_text_oneway(vm.addLabel, wb::ios::ui::view_for(add));
    be.bind_text_oneway(vm.saveLabel, wb::ios::ui::view_for(save));
    be.bind_text_oneway(vm.deleteLabel, wb::ios::ui::view_for(del));
    be.bind_command(vm.addNote, wb::ios::ui::view_for(add));
    be.bind_command(vm.saveNote, wb::ios::ui::view_for(save));
    be.bind_command(vm.deleteSelected, wb::ios::ui::view_for(del));
}

}  // namespace wb::notes::iosview

namespace wb::notes {
void register_notes_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "notes", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto* view = new iosview::NotesView(static_cast<NotesVm&>(vm), be);
            return view->viewController();
        });
}
}  // namespace wb::notes
