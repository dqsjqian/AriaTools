#include "EchoView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/EchoVm.h"
#include <QLabel>
#include <QVBoxLayout>
namespace wb::echo::qtview {
EchoView::EchoView(EchoVm& vm, aria::binding::BindingEngine& be)
    : root_(new QWidget) {
    auto* lay = new QVBoxLayout(root_);
    auto* title = wb::ui::make_title("");
    auto* msg = wb::ui::make_info("");
    lay->addWidget(title);
    lay->addWidget(msg);
    lay->addStretch();
    be.bind_text_oneway(vm.title, wb::ui::view_for(title));
    be.bind_text_oneway(vm.message, wb::ui::view_for(msg));
}
}  // namespace wb::echo::qtview
namespace wb::echo {
void register_echo_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "echo", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto* view = new qtview::EchoView(static_cast<EchoVm&>(vm), be);
            return view->widget();
        });
}
}  // namespace wb::echo
