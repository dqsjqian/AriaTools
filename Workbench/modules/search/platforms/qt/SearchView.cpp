
#include "support/UiHelpers.h"
#include "support/QtViewFactory.h"
#include "viewmodels/SearchVm.h"
#include "viewmodels/SearchVmHostVm.h"
#include "aria/adapters/qt6/qt_list_model_adapter.hpp"
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QVBoxLayout>
namespace wb::search::qtview {
using namespace wb::ui;
static QWidget* build(SearchVm& vm, SearchVmHostVm& host, aria::binding::BindingEngine& be) {
    auto* w = new QWidget;
    auto& s_subs = subs_attached_to(w);
    auto* lay = new QVBoxLayout(w);
    // Hint banner: VM-owned desc property (i18n, auto-refreshes on language change).
    auto* info = wb::ui::make_info("");
    lay->addWidget(info);
    be.bind_text_oneway(host.desc, view_for(info));

    auto* input = new QLineEdit;
    input->setPlaceholderText(QString::fromStdString(wb::i18n::str_in("search", "placeholder")));
    lay->addWidget(input);
    be.bind_text(vm.query, view_for(input));
    auto* rawLbl = new QLabel;
    auto* debLbl = new QLabel;
    auto* disLbl = new QLabel;
    for (auto* l : {rawLbl, debLbl, disLbl}) {
        l->setStyleSheet("QLabel { font-family:monospace; font-size:11px; color:#546e7a; }");
    }
    lay->addWidget(rawLbl);
    lay->addWidget(debLbl);
    lay->addWidget(disLbl);
    auto syncRaw = [rawLbl](const std::string& s) {
        rawLbl->setText(QStringLiteral("  raw       : ") + QString::fromStdString(s));
    };
    auto syncDeb = [debLbl](const std::string& s) {
        debLbl->setText(QStringLiteral("  debounced : ") + QString::fromStdString(s));
    };
    auto syncDis = [disLbl](const std::string& s) {
        disLbl->setText(QStringLiteral("  distinct  : ") + QString::fromStdString(s));
    };
    syncRaw(vm.query.get());
    syncDeb(vm.debounced->get());
    syncDis(vm.distinct ->get());
    s_subs.push_back(vm.query    .on_changed(syncRaw));
    s_subs.push_back(vm.debounced->on_changed(syncDeb));
    s_subs.push_back(vm.distinct ->on_changed(syncDis));
    auto* history = new QLabel(QString::fromStdString(wb::i18n::str_in("search", "searches")));
    history->setStyleSheet("QLabel { color:#263238; margin-top:6px; }");
    lay->addWidget(history);
    auto* listView = new QListView;
    auto* model = new aria::adapters::qt6::ObservableListModel<SearchHit>(
        vm.hits,
        {{Qt::DisplayRole, "display"}},
        [](const SearchHit& h, int role) -> QVariant {
            if (role == Qt::DisplayRole) {
                return QString(QString::fromStdString(wb::i18n::str_in("search", "item")))
                    .arg(h.seq)
                    .arg(QString::fromStdString(h.q));
            }
            return {};
        });
    listView->setModel(model);
    lay->addWidget(listView, 1);
    return w;
}
}  // namespace wb::search::qtview

namespace wb::search {
void register_search_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "search",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto& host = static_cast<SearchVmHostVm&>(vm);
            return qtview::build(host.inner(), host, be);
        });
}
}  // namespace wb::search
