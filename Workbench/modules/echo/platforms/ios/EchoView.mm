#include "EchoView.h"
#include "support/IosUi.h"
#include "viewmodels/EchoVm.h"

namespace wb::echo::iosview {

EchoView::EchoView(EchoVm& vm, aria::binding::BindingEngine& be)
    : vc_(nil) {
    UILabel* title = wb::ios::ui::make_title(@"");
    UILabel* message = wb::ios::ui::make_label(@"");
    vc_ = wb::ios::ui::make_stack_vc(@[title, message]);

    be.bind_text_oneway(vm.title, wb::ios::ui::view_for(title));
    be.bind_text_oneway(vm.message, wb::ios::ui::view_for(message));
}

}  // namespace wb::echo::iosview
