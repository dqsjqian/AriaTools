#include "SearchView.h"
#include "support/IosUi.h"
#include "infra/i18n/I18n.h"
#include "viewmodels/SearchVm.h"

#include "aria/binding/binding_engine.hpp"

namespace wb::search::iosview {

SearchView::SearchView(SearchVm& host, aria::binding::BindingEngine& be)
    : vc_(nil) {
    auto& vm = host;

    UILabel*     title   = wb::ios::ui::make_title(@"");
    UILabel*     desc    = wb::ios::ui::make_label(@"");
    UITextField* query   = wb::ios::ui::make_field(@"");
    UILabel*     debLbl   = wb::ios::ui::make_label(@"");
    UILabel*     disLbl   = wb::ios::ui::make_label(@"");

    vc_ = wb::ios::ui::make_stack_vc(@[title, desc, query, debLbl, disLbl]);

    be.bind_text_oneway(host.title, wb::ios::ui::view_for(title));
    be.bind_text_oneway(host.desc,  wb::ios::ui::view_for(desc));

    query.placeholder = [NSString stringWithUTF8String:wb::i18n::str_in("search","placeholder").c_str()];
    be.bind_text(vm.query, wb::ios::ui::view_for(query));

    // Debounced + distinct values (read-only labels).
    be.bind_text_oneway(*vm.debounced, wb::ios::ui::view_for(debLbl));
    be.bind_text_oneway(*vm.distinct,  wb::ios::ui::view_for(disLbl));
}

}  // namespace wb::search::iosview
