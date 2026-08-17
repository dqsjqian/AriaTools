#include "NotesView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/NotesVm.h"

namespace wb::notes {

void register_notes_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "notes", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<iosview::NotesView>(static_cast<NotesVm&>(vm), be);
            UIViewController* vc = view->viewController();
            wb::ios::ui::attach_owner(vc, std::move(view));
            return vc;
        });
}

}  // namespace wb::notes
