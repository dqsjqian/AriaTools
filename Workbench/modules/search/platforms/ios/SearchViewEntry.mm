#include "SearchView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/SearchVm.h"

namespace wb::search {

void register_search_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "search", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<iosview::SearchView>(static_cast<SearchVm&>(vm), be);
            UIViewController* vc = view->viewController();
            wb::ios::ui::attach_owner(vc, std::move(view));
            return vc;
        });
}

}  // namespace wb::search
