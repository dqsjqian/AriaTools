#include "SearchView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/SearchVm.h"

namespace wb::search {

void register_search_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "search",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<qtview::SearchView>(static_cast<SearchVm&>(vm), be);
            return wb::qt::own_view(std::move(view));
        });
}

}  // namespace wb::search
