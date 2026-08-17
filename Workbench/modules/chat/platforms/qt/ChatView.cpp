#include "ChatView.h"
#include "support/UiHelpers.h"
#include "viewmodels/ChatVm.h"
#include "models/ChatMessage.h"
#include "aria/adapters/qt6/qt_list_model_adapter.hpp"

#include "aria/binding/binding_engine.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>
#include <QVBoxLayout>

namespace wb::chat::qtview {
using namespace wb::ui;

namespace {

// ─── Publisher sub-view ───────────────────────────────────────────────────
QWidget* build_publisher_view(ChatPublisherVm& pub, aria::binding::BindingEngine& be) {
    auto* row_ = new QWidget;
    auto* row = new QHBoxLayout(row_);
    auto* userEdit = new QLineEdit; userEdit->setPlaceholderText(QString::fromStdString(wb::i18n::str_in("chat", "user")));
    userEdit->setMaximumWidth(100);
    auto* draftEdit = new QLineEdit; draftEdit->setPlaceholderText(QString::fromStdString(wb::i18n::str_in("chat", "draft")));
    auto* sendBtn = new QPushButton(QString::fromStdString(wb::i18n::str_in("chat", "send")));
    row->addWidget(userEdit);
    row->addWidget(draftEdit, 1);
    row->addWidget(sendBtn);
    be.bind_text   (pub.user,  view_for(userEdit));
    be.bind_text   (pub.draft, view_for(draftEdit));
    be.bind_command(pub.send,  view_for(sendBtn));
    return row_;
}

// ─── Subscriber sub-view ──────────────────────────────────────────────────
QWidget* build_subscriber_view(ChatSubscriberVm& sub, aria::binding::BindingEngine&) {
    auto* list_ = new QListView;
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
    list_->setModel(model);
    return list_;
}

}  // namespace

// ─── Top-level view ────────────────────────────────────────────────────────
ChatView::ChatView(ChatVm& vm, aria::binding::BindingEngine& be)
    : root_(new QWidget) {
    auto& s_subs = subs_attached_to(root_);
    auto* lay = new QVBoxLayout(root_);
    // Hint banner: VM-owned desc property (i18n, auto-refreshes on language change).
    auto* info = wb::ui::make_info("");
    lay->addWidget(info);
    be.bind_text_oneway(vm.desc, view_for(info));

    auto* publisher  = build_publisher_view(*vm.publisher, be);
    auto* subscriber = build_subscriber_view(*vm.subscriber, be);

    lay->addWidget(new QLabel("<b>Publisher</b>"));
    lay->addWidget(publisher);
    lay->addWidget(new QLabel("<b>Subscriber</b>"));
    lay->addWidget(subscriber, 1);
}

}  // namespace wb::chat::qtview
