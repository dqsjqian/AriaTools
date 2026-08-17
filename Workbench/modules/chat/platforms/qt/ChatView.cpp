
#include "support/UiHelpers.h"
#include "support/QtViewFactory.h"
#include "viewmodels/ChatVm.h"
#include "aria/adapters/qt6/qt_list_model_adapter.hpp"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>
#include <QVBoxLayout>
namespace wb::chat::qtview {
using namespace wb::ui;
static QWidget* build(ChatVm& vm, aria::binding::BindingEngine& be) {
    auto* w = new QWidget;
    auto& s_subs = subs_attached_to(w);
    auto* lay = new QVBoxLayout(w);
    // Hint banner: VM-owned desc property (i18n, auto-refreshes on language change).
    auto* info = wb::ui::make_info("");
    lay->addWidget(info);
    be.bind_text_oneway(vm.desc, view_for(info));
    auto& pub = *vm.publisher;
    auto& sub = *vm.subscriber;
    lay->addWidget(new QLabel("<b>Publisher</b>"));
    auto* row = new QHBoxLayout;
    auto* userEdit = new QLineEdit; userEdit->setPlaceholderText(QString::fromStdString(wb::i18n::str_in("chat", "user")));
    userEdit->setMaximumWidth(100);
    auto* draftEdit = new QLineEdit; draftEdit->setPlaceholderText(QString::fromStdString(wb::i18n::str_in("chat", "draft")));
    auto* sendBtn = new QPushButton(QString::fromStdString(wb::i18n::str_in("chat", "send")));
    row->addWidget(userEdit);
    row->addWidget(draftEdit, 1);
    row->addWidget(sendBtn);
    lay->addLayout(row);
    be.bind_text   (pub.user,  view_for(userEdit));
    be.bind_text   (pub.draft, view_for(draftEdit));
    be.bind_command(pub.send,  view_for(sendBtn));
    lay->addWidget(new QLabel("<b>Subscriber</b>"));
    auto* listView = new QListView;
    auto* model = new aria::adapters::qt6::ObservableListModel<ChatMessage>(
        sub.messages,
        {{Qt::DisplayRole, "display"}},
        [](const ChatMessage& m, int role) -> QVariant {
            if (role == Qt::DisplayRole) {
                return QString("  %1:  %2")
                    .arg(QString::fromStdString(m.user))
                    .arg(QString::fromStdString(m.text));
            }
            return {};
        });
    listView->setModel(model);
    lay->addWidget(listView, 1);
    return w;
}
}  // namespace wb::chat::qtview

namespace wb::chat {
void register_chat_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "chat",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            return qtview::build(static_cast<ChatVm&>(vm), be);
        });
}
}  // namespace wb::chat
