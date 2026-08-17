#include "SyncView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/SyncVm.h"

namespace wb::sync {

void register_sync_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "sync", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<iosview::SyncView>(static_cast<SyncVm&>(vm), be);
            UIViewController* vc = view->viewController();
            wb::ios::ui::attach_owner(vc, std::move(view));
            return vc;
        });
}

}  // namespace wb::sync
