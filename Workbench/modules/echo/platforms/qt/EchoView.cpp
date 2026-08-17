#include "EchoView.h"
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