#include "NotesView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/NotesVm.h"

namespace wb::notes {

void register_notes_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "notes",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto* view = new qtview::NotesView(static_cast<NotesVm&>(vm), be);
            return view->widget();
        });
}

}  // namespace wb::notes
