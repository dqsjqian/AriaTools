#include "ThemeView.h"
#include "support/QtViewFactory.h"
#include "support/UiHelpers.h"
#include "infra/i18n/I18n.h"
#include "viewmodels/ThemeVm.h"
#include "viewmodels/ThemeVmHostVm.h"

#include "aria/binding/binding_engine.hpp"

#include <QComboBox>
#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>

namespace wb::theme::qtview {
using namespace wb::ui;
namespace {
void apply_theme(QFrame* card, QLabel* title, QLabel* body, const ITheme& th) {
    auto s = [](std::string_view v) {
        return QString::fromUtf8(v.data(), static_cast<int>(v.size()));
    };
    card->setStyleSheet(QString(
        "QFrame { background:%1; border:1px solid %2; border-radius:10px; }")
        .arg(s(th.cardBg())).arg(s(th.cardBorder())));
    const QString fg = s(th.cardFg());
    title->setStyleSheet(QString("QLabel { color:%1; font-size:18px; font-weight:bold; }").arg(fg));
    body ->setStyleSheet(QString("QLabel { color:%1; font-size:12px; }").arg(fg));
}

/// Rebuild the picker items from the VM's available_themes() list.
/// Called once on construction and again on language change so the
/// option labels stay localized.
void repopulate_picker(QComboBox* picker, ThemeVm& vm) {
    const QString currentId = QString::fromStdString(vm.currentId.get());
    picker->blockSignals(true);
    picker->clear();
    for (const auto& opt : vm.available_themes()) {
        picker->addItem(QString::fromStdString(opt.displayName),
                         QString::fromStdString(opt.id));
    }
    for (int i = 0; i < picker->count(); ++i) {
        if (picker->itemData(i).toString() == currentId) {
            picker->setCurrentIndex(i);
            break;
        }
    }
    picker->blockSignals(false);
}
}  // namespace

ThemeView::ThemeView(ThemeVmHostVm& host, aria::binding::BindingEngine& be)
    : root_(new QWidget) {
    auto& vm = host.inner();
    auto& s_subs = subs_attached_to(root_);
    auto* lay = new QVBoxLayout(root_);
    // Hint banner: VM-owned desc property (i18n, auto-refreshes on language change).
    auto* info = wb::ui::make_info("");
    lay->addWidget(info);
    be.bind_text_oneway(vm.currentDisplayName, view_for(info));

    auto* picker = new QComboBox;
    repopulate_picker(picker, vm);
    lay->addWidget(picker);

    auto* card = new QFrame;
    auto* cardLay = new QVBoxLayout(card);
    auto* title = new QLabel(QString::fromStdString(wb::i18n::str_in("theme", "card_title")));
    auto* body = new QLabel(QString::fromStdString(wb::i18n::str_in("theme", "card_body")));
    body->setWordWrap(true);
    cardLay->addWidget(title);
    cardLay->addWidget(body);
    lay->addWidget(card, 1);
    apply_theme(card, title, body, *vm.theme());

    // Picker -> VM.pick(id)
    QObject::connect(picker, QOverload<int>::of(&QComboBox::currentIndexChanged),
                     [picker, &vm](int) {
        vm.pick(picker->currentData().toString().toStdString());
    });

    // VM -> picker + card refresh
    s_subs.push_back(vm.currentId.on_changed(
        [picker, card, title, body, &vm](const std::string&) {
            repopulate_picker(picker, vm);
            apply_theme(card, title, body, *vm.theme());
        }));
    // Language change already re-resolves currentDisplayName via the
    // HostVm's localize() hook; we also need to refresh the picker
    // labels and card text.
    s_subs.push_back(vm.currentDisplayName.on_changed(
        [picker, &vm, title, body](const std::string&) {
            repopulate_picker(picker, vm);
            title->setText(QString::fromStdString(wb::i18n::str_in("theme", "card_title")));
            body ->setText(QString::fromStdString(wb::i18n::str_in("theme", "card_body")));
        }));
}

}  // namespace wb::theme::qtview

namespace wb::theme {
void register_theme_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "theme",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto& host = static_cast<ThemeVmHostVm&>(vm);
            auto* view = new qtview::ThemeView(host, be);
            return view->widget();
        });
}
}  // namespace wb::theme
