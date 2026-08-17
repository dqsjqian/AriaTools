#include "EchoView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/EchoVm.h"

namespace wb::echo {

void register_echo_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "echo", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto* view = new qtview::EchoView(static_cast<EchoVm&>(vm), be);
            return view->widget();
        });
}

}  // namespace wb::echo
