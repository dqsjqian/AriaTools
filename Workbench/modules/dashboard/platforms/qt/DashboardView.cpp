#include "support/QtViewFactory.h"
#include "support/UiHelpers.h"
#include "viewmodels/DashboardVm.h"

#include "aria/binding/binding_engine.hpp"

#include <QLabel>
#include <QVBoxLayout>

namespace wb::dashboard::qtview {

static QWidget* build(wb::dashboard::DashboardVm& vm, aria::binding::BindingEngine& be) {
    auto* w = new QWidget;
    auto* lay = new QVBoxLayout(w);

    auto* title = wb::ui::make_title("");
    auto* info  = wb::ui::make_info("");
    lay->addWidget(title);
    lay->addWidget(info);
    lay->addStretch();

    be.bind_text_oneway(vm.welcome, wb::ui::view_for(title));
    be.bind_text_oneway(vm.summary, wb::ui::view_for(info));
    return w;
}

}  // namespace wb::dashboard::qtview

namespace wb::dashboard {

void register_dashboard_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "dashboard",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            return qtview::build(static_cast<DashboardVm&>(vm), be);
        });
}

}  // namespace wb::dashboard
