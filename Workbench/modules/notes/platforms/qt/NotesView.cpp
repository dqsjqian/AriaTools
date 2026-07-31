#include "support/QtViewFactory.h"
#include "support/UiHelpers.h"
#include "viewmodels/NotesVm.h"

#include "aria/binding/binding_engine.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

namespace wb::notes::qtview {

static QWidget* build(wb::notes::NotesVm& vm, aria::binding::BindingEngine& be) {
    auto* w = new QWidget;
    auto& s_subs = wb::ui::subs_attached_to(w);
    auto* lay = new QVBoxLayout(w);

    auto* title = wb::ui::make_title("");
    auto* hint  = wb::ui::make_info("");
    lay->addWidget(title);
    lay->addWidget(hint);

    auto* statusLbl = new QLabel;
    lay->addWidget(statusLbl);

    auto* list = new QListWidget;
    lay->addWidget(list, 1);

    auto* btnRow = new QHBoxLayout;
    auto* addBtn = new QPushButton;
    auto* delBtn = new QPushButton;
    btnRow->addWidget(addBtn);
    btnRow->addWidget(delBtn);
    btnRow->addStretch();
    lay->addLayout(btnRow);

    be.bind_text_oneway(vm.title, wb::ui::view_for(title));
    be.bind_text_oneway(vm.hint, wb::ui::view_for(hint));
    be.bind_text_oneway(vm.status, wb::ui::view_for(statusLbl));
    be.bind_text_oneway(vm.addLabel, wb::ui::view_for(addBtn));
    be.bind_text_oneway(vm.deleteLabel, wb::ui::view_for(delBtn));
    be.bind_command(vm.addNote, wb::ui::view_for(addBtn));
    be.bind_command(vm.deleteSelected, wb::ui::view_for(delBtn));

    auto rebuild = [list, &vm]() {
        list->clear();
        for (std::size_t i = 0; i < vm.notes.size(); ++i) {
            if (auto n = vm.notes.at(i))
                list->addItem(QString::fromStdString(n->title));
        }
    };
    rebuild();
    s_subs.push_back(vm.notes.on_any_change([rebuild]() { rebuild(); }));

    return w;
}

}  // namespace wb::notes::qtview

namespace wb::notes {

void register_notes_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "notes",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            return qtview::build(static_cast<NotesVm&>(vm), be);
        });
}

}  // namespace wb::notes
