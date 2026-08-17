#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "infra/i18n/I18n.h"
#include "viewmodels/SearchVm.h"
#include "viewmodels/SearchVmHostVm.h"

#include "aria/binding/binding_engine.hpp"

namespace wb::search::iosview {

static UIViewController* build(SearchVm& vm, SearchVmHostVm& host,
                                aria::binding::BindingEngine& be) {
    UILabel*     title   = wb::ios::ui::make_title(@"");
    UILabel*     desc    = wb::ios::ui::make_label(@"");
    UITextField* query   = wb::ios::ui::make_field(@"");
    UILabel*     debLbl   = wb::ios::ui::make_label(@"");
    UILabel*     disLbl   = wb::ios::ui::make_label(@"");

    auto* vc = wb::ios::ui::make_stack_vc(@[title, desc, query, debLbl, disLbl]);

    be.bind_text_oneway(host.title, wb::ios::ui::view_for(title));
    be.bind_text_oneway(host.desc,  wb::ios::ui::view_for(desc));

    query.placeholder = [NSString stringWithUTF8String:wb::i18n::str_in("search","placeholder").c_str()];
    be.bind_text(vm.query, wb::ios::ui::view_for(query));

    // Debounced + distinct values (read-only labels).
    be.bind_text_oneway(*vm.debounced, wb::ios::ui::view_for(debLbl));
    be.bind_text_oneway(*vm.distinct,  wb::ios::ui::view_for(disLbl));
    return vc;
}

}  // namespace wb::search::iosview

namespace wb::search {
void register_search_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "search", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto& host = static_cast<SearchVmHostVm&>(vm);
            return iosview::build(host.inner(), host, be);
        });
}
}  // namespace wb::search
