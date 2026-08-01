#include "support/QtViewFactory.h"
#include "support/UiHelpers.h"
#include "viewmodels/NotesVm.h"

#include "aria/binding/binding_engine.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSignalBlocker>
#include <QVBoxLayout>

#include <string>

namespace wb::notes::qtview {

// 用列表项的 UserRole 存 note id，避免依赖行号。
static constexpr int kNoteIdRole = Qt::UserRole + 1;

static QString display_title(const Note& note) {
    return note.title.empty() ? QStringLiteral("—")
                              : QString::fromStdString(note.title);
}

static QWidget* build(wb::notes::NotesVm& vm, aria::binding::BindingEngine& be) {
    auto* w = new QWidget;
    auto& subs = wb::ui::subs_attached_to(w);
    auto* root = new QVBoxLayout(w);

    auto* title = wb::ui::make_title("");
    auto* hint = wb::ui::make_info("");
    root->addWidget(title);
    root->addWidget(hint);
    be.bind_text_oneway(vm.title, wb::ui::view_for(title));
    be.bind_text_oneway(vm.hint, wb::ui::view_for(hint));

    auto* body = new QHBoxLayout;
    root->addLayout(body, 1);

    // 左侧：列表 + 状态 + 新建按钮。
    auto* leftCol = new QVBoxLayout;
    auto* list = new QListWidget;
    auto* statusLbl = new QLabel;
    auto* addBtn = new QPushButton;
    leftCol->addWidget(list, 1);
    leftCol->addWidget(statusLbl);
    leftCol->addWidget(addBtn);
    body->addLayout(leftCol, 1);
    be.bind_text_oneway(vm.status, wb::ui::view_for(statusLbl));
    be.bind_text_oneway(vm.addLabel, wb::ui::view_for(addBtn));
    be.bind_command(vm.addNote, wb::ui::view_for(addBtn));

    // 右侧：标题编辑 + 正文编辑 + 保存/删除。
    auto* rightCol = new QVBoxLayout;
    auto* titleEdit = new QLineEdit;
    auto* bodyEdit = new QPlainTextEdit;
    auto* actionRow = new QHBoxLayout;
    auto* saveBtn = new QPushButton;
    auto* delBtn = new QPushButton;
    actionRow->addWidget(saveBtn);
    actionRow->addWidget(delBtn);
    actionRow->addStretch();
    rightCol->addWidget(titleEdit);
    rightCol->addWidget(bodyEdit, 1);
    rightCol->addLayout(actionRow);
    body->addLayout(rightCol, 2);

    // 占位文案随语言更新。
    auto applyPlaceholders = [titleEdit, bodyEdit, &vm]() {
        titleEdit->setPlaceholderText(QString::fromStdString(vm.titlePlaceholder.get()));
        bodyEdit->setPlaceholderText(QString::fromStdString(vm.bodyPlaceholder.get()));
    };
    applyPlaceholders();
    subs.push_back(vm.titlePlaceholder.on_changed(
        [applyPlaceholders](const std::string&) { applyPlaceholders(); }));
    subs.push_back(vm.bodyPlaceholder.on_changed(
        [applyPlaceholders](const std::string&) { applyPlaceholders(); }));

    // 用户输入 → VM（bind_editable_text 只做 View→VM，避免打字时光标重置）。
    wb::ui::bind_editable_text(be, vm.editTitle, titleEdit);
    wb::ui::bind_editable_text(be, vm.editBody, bodyEdit);
    be.bind_text_oneway(vm.saveLabel, wb::ui::view_for(saveBtn));
    be.bind_text_oneway(vm.deleteLabel, wb::ui::view_for(delBtn));
    be.bind_command(vm.saveNote, wb::ui::view_for(saveBtn));
    be.bind_command(vm.deleteSelected, wb::ui::view_for(delBtn));

    // 编辑器仅在有选中时可用。
    auto applyEnabled = [titleEdit, bodyEdit, saveBtn, delBtn, &vm]() {
        const bool on = vm.hasSelection.get();
        titleEdit->setEnabled(on);
        bodyEdit->setEnabled(on);
        saveBtn->setEnabled(on);
        delBtn->setEnabled(on);
    };
    applyEnabled();
    subs.push_back(vm.hasSelection.on_changed(
        [applyEnabled](bool) { applyEnabled(); }));

    // VM → View：把 VM 编辑器 Property 灌入 widget。仅当内容确有差异时才
    // setText，避免用户打字时（widget 已等于新值）触发光标复位。
    // 关键：订阅 editTitle/editBody 的变化而非 selectedId——VM 在 model.select
    // 之后才更新这两个 Property，此时读到的才是目标笔记内容；若改订阅 selectedId，
    // 回调会在 VM 尚未刷新编辑器 Property 时触发，读到上一条笔记的陈旧文本。
    auto pushTitle = [titleEdit, &vm]() {
        const QString v = QString::fromStdString(vm.editTitle.get());
        if (titleEdit->text() == v) return;
        const QSignalBlocker b(titleEdit);
        titleEdit->setText(v);
    };
    auto pushBody = [bodyEdit, &vm]() {
        const QString v = QString::fromStdString(vm.editBody.get());
        if (bodyEdit->toPlainText() == v) return;
        const QSignalBlocker b(bodyEdit);
        bodyEdit->setPlainText(v);
    };
    pushTitle();
    pushBody();
    subs.push_back(vm.editTitle.on_changed([pushTitle](const std::string&) { pushTitle(); }));
    subs.push_back(vm.editBody.on_changed([pushBody](const std::string&) { pushBody(); }));

    // 列表重建：Model 列表变更时刷新并保持选中高亮。
    auto rebuild = [list, &vm]() {
        const QSignalBlocker blocker(list);
        list->clear();
        const std::string sel = vm.selectedId.get();
        for (std::size_t i = 0; i < vm.notes.size(); ++i) {
            auto n = vm.notes.at(i);
            if (!n) continue;
            auto* item = new QListWidgetItem(display_title(*n));
            item->setData(kNoteIdRole, QString::fromStdString(n->id));
            list->addItem(item);
            if (n->id == sel) list->setCurrentItem(item);
        }
    };
    rebuild();
    subs.push_back(vm.notes.on_any_change([rebuild]() { rebuild(); }));
    subs.push_back(vm.selectedId.on_changed(
        [rebuild](const std::string&) { rebuild(); }));

    // 列表点击 → 通知 VM 选中。
    QObject::connect(list, &QListWidget::currentItemChanged,
                     [&vm](QListWidgetItem* current, QListWidgetItem*) {
                         if (!current) return;
                         const std::string id =
                             current->data(kNoteIdRole).toString().toStdString();
                         if (id != vm.selectedId.get()) vm.selectNote.execute(id);
                     });

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
