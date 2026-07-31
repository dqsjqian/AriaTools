#include "support/QtViewFactory.h"
#include "support/UiHelpers.h"
#include "viewmodels/CalendarVm.h"

#include "aria/binding/binding_engine.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace wb::calendar::qtview {

static QWidget* build(wb::calendar::CalendarVm& vm, aria::binding::BindingEngine& be) {
    auto* w = new QWidget;
    auto* lay = new QVBoxLayout(w);

    auto* title = wb::ui::make_title("");
    auto* hint  = wb::ui::make_info("");
    lay->addWidget(title);
    lay->addWidget(hint);

    auto* row = new QHBoxLayout;
    auto* urlEdit = new QLineEdit;
    auto* addBtn = new QPushButton;
    row->addWidget(urlEdit, 1);
    row->addWidget(addBtn);
    lay->addLayout(row);

    auto* statusLbl = new QLabel;
    statusLbl->setWordWrap(true);
    lay->addWidget(statusLbl);
    lay->addStretch();

    be.bind_text_oneway(vm.title, wb::ui::view_for(title));
    be.bind_text_oneway(vm.hint, wb::ui::view_for(hint));
    // QLineEdit 的 placeholder 用 VM 文案（随语言）。
    urlEdit->setPlaceholderText(QString::fromStdString(vm.urlPlaceholder.get()));
    wb::ui::subs_attached_to(w).push_back(
        vm.urlPlaceholder.on_changed([urlEdit](const std::string& s) {
            urlEdit->setPlaceholderText(QString::fromStdString(s));
        }));
    be.bind_text_oneway(vm.subscribeLabel, wb::ui::view_for(addBtn));
    be.bind_text(vm.subscribeUrl, wb::ui::view_for(urlEdit));
    be.bind_command(vm.addSubscription, wb::ui::view_for(addBtn));
    be.bind_text_oneway(vm.status, wb::ui::view_for(statusLbl));
    return w;
}

}  // namespace wb::calendar::qtview

namespace wb::calendar {

void register_calendar_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "calendar",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            return qtview::build(static_cast<CalendarVm&>(vm), be);
        });
}

}  // namespace wb::calendar
