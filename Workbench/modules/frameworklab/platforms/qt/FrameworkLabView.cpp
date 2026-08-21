#include "FrameworkLabView.h"

#include "support/UiHelpers.h"
#include "viewmodels/FrameworkLabVm.h"
#include "aria/adapters/qt6/qt_list_model_adapter.hpp"

#include <QAbstractItemView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace wb::frameworklab::qtview {

FrameworkLabView::FrameworkLabView(FrameworkLabVm& vm,
                                   aria::binding::BindingEngine& be)
    : root_(new QWidget) {
    auto& subs = wb::ui::subs_attached_to(root_);
    auto* layout = new QVBoxLayout(root_);

    auto* title = new QLabel;
    auto* desc = wb::ui::make_info("");
    layout->addWidget(title);
    layout->addWidget(desc);
    be.bind_text_oneway(vm.title, wb::ui::view_for(title));
    be.bind_text_oneway(vm.desc, wb::ui::view_for(desc));

    auto* form = new QHBoxLayout;
    auto* draftLabel = new QLabel;
    auto* draft = new QLineEdit;
    auto* add = new QPushButton;
    form->addWidget(draftLabel);
    form->addWidget(draft, 1);
    form->addWidget(add);
    layout->addLayout(form);
    be.bind_text(vm.draft, wb::ui::view_for(draft));
    be.bind_text_oneway(vm.draftLabel, wb::ui::view_for(draftLabel));
    be.bind_text_oneway(vm.addLabel, wb::ui::view_for(add));
    be.bind_command(vm.addTask, wb::ui::view_for(add));

    auto* list = new QListView;
    list->setSelectionMode(QAbstractItemView::SingleSelection);
    auto* model = new aria::adapters::qt6::ObservableListModel<LabTask>(
        *vm.tasks, {{Qt::DisplayRole, "display"}},
        [](const LabTask& task, int role) -> QVariant {
            if (role == Qt::DisplayRole) {
                return QString("%1  %2")
                    .arg(task.completed ? QStringLiteral("[x]") : QStringLiteral("[ ]"))
                    .arg(QString::fromStdString(task.title));
            }
            return {};
        });
    list->setModel(model);
    layout->addWidget(list, 1);
    QObject::connect(list->selectionModel(), &QItemSelectionModel::currentRowChanged,
                     [&vm](const QModelIndex& current, const QModelIndex&) {
                         if (current.isValid()) vm.select_index(static_cast<std::size_t>(current.row()));
                     });

    auto* selected = new QLabel;
    auto sync_selected = [selected](const std::string& value) {
        selected->setText(QString::fromStdString(value));
    };
    sync_selected(vm.selectedText.get());
    subs.push_back(vm.selectedText.on_changed(sync_selected));
    layout->addWidget(selected);

    auto* summary = new QLabel;
    auto sync_summary = [summary](const std::string& value) {
        summary->setText(QString::fromStdString(value));
    };
    sync_summary(vm.summaryText.get());
    subs.push_back(vm.summaryText.on_changed(sync_summary));
    layout->addWidget(summary);

    auto* actions = new QHBoxLayout;
    auto* toggle = new QPushButton;
    auto* remove = new QPushButton;
    auto* clear = new QPushButton;
    actions->addWidget(toggle);
    actions->addWidget(remove);
    actions->addWidget(clear);
    layout->addLayout(actions);
    be.bind_text_oneway(vm.toggleLabel, wb::ui::view_for(toggle));
    be.bind_text_oneway(vm.removeLabel, wb::ui::view_for(remove));
    be.bind_text_oneway(vm.clearLabel, wb::ui::view_for(clear));
    be.bind_command(vm.toggleSelected, wb::ui::view_for(toggle));
    be.bind_command(vm.removeSelected, wb::ui::view_for(remove));
    be.bind_command(vm.clearCompleted, wb::ui::view_for(clear));

    auto* graphTitle = new QLabel;
    auto* refresh = new QPushButton;
    auto* graph = new QPlainTextEdit;
    graph->setReadOnly(true);
    layout->addWidget(graphTitle);
    layout->addWidget(refresh);
    layout->addWidget(graph, 1);
    be.bind_text_oneway(vm.graphLabel, wb::ui::view_for(graphTitle));
    be.bind_text_oneway(vm.refreshLabel, wb::ui::view_for(refresh));
    be.bind_command(vm.refreshGraph, wb::ui::view_for(refresh));
    auto sync_graph = [graph](const std::string& value) {
        graph->setPlainText(QString::fromStdString(value));
    };
    sync_graph(vm.graphSnapshot.get());
    subs.push_back(vm.graphSnapshot.on_changed(sync_graph));
}

}  // namespace wb::frameworklab::qtview
