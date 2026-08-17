#include "SyncView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/SyncVm.h"

namespace wb::sync {

void register_sync_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "sync",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto* view = new qtview::SyncView(static_cast<SyncVm&>(vm), be);
            return view->widget();
        });
}

}  // namespace wb::sync
