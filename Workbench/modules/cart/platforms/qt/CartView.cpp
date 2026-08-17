#include "CartView.h"
#include "support/UiHelpers.h"
#include "viewmodels/CartVm.h"
#include "aria/adapters/qt6/qt_list_model_adapter.hpp"

#include "aria/binding/binding_engine.hpp"

#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>
#include <QVBoxLayout>

namespace wb::cart::qtview {
using namespace wb::ui;

CartView::CartView(CartVm& vm, aria::binding::BindingEngine& be)
    : root_(new QWidget) {
    auto& s_subs = subs_attached_to(root_);
    auto* lay = new QVBoxLayout(root_);
    // Hint banner: VM-owned desc property (i18n, auto-refreshes on language change).
    auto* info = wb::ui::make_info("");
    lay->addWidget(info);
    be.bind_text_oneway(vm.desc, view_for(info));
    // Add-item form
    auto* form = new QFormLayout;
    auto* nameEdit = new QLineEdit;
    auto* priceSpin = new QDoubleSpinBox;
    priceSpin->setRange(0.01, 9999.0);
    priceSpin->setDecimals(2);
    priceSpin->setPrefix("¥ ");
    auto* addBtn = new QPushButton(QString::fromStdString(wb::i18n::str_in("cart", "add")));
    form->addRow(QString::fromStdString(wb::i18n::str_in("cart", "name_label")), nameEdit);
    form->addRow(QString::fromStdString(wb::i18n::str_in("cart", "price_label")), priceSpin);
    form->addRow("",     addBtn);
    lay->addLayout(form);
    be.bind_text  (vm.draftName,  view_for(nameEdit));
    be.bind_double(vm.draftPrice, view_for(priceSpin));
    be.bind_command(vm.addItem,   view_for(addBtn));
    // List
    auto* listView = new QListView;
    listView->setAlternatingRowColors(true);
    auto* model = new aria::adapters::qt6::ObservableListModel<CartItem>(
        vm.items,
        {{Qt::DisplayRole, "display"}},
        [](const CartItem& it, int role) -> QVariant {
            if (role == Qt::DisplayRole) {
                return QString("  %1  ×  %2     ¥ %3  →  ¥ %4")
                    .arg(QString::fromStdString(it.name()))
                    .arg(it.qty_value())
                    .arg(it.price(),    0, 'f', 2)
                    .arg(it.subtotal(), 0, 'f', 2);
            }
            return {};
        });
    listView->setModel(model);
    lay->addWidget(listView, 1);
    // Action buttons
    auto* ops = new QHBoxLayout;
    auto* plusBtn  = new QPushButton(QString::fromStdString(wb::i18n::str_in("cart", "inc")));
    auto* minusBtn = new QPushButton(QString::fromStdString(wb::i18n::str_in("cart", "dec")));
    auto* delBtn   = new QPushButton(QString::fromStdString(wb::i18n::str_in("cart", "remove")));
    ops->addWidget(plusBtn);
    ops->addWidget(minusBtn);
    ops->addWidget(delBtn);
    lay->addLayout(ops);
    auto currentItem = [listView, &vm]() -> std::shared_ptr<CartItem> {
        auto idx = listView->currentIndex();
        if (!idx.isValid()) return nullptr;
        auto snap = vm.items.snapshot();
        const auto row = idx.row();
        if (row < 0 || static_cast<std::size_t>(row) >= snap.size()) return nullptr;
        return snap[static_cast<std::size_t>(row)];
    };
    QObject::connect(plusBtn, &QPushButton::clicked, [currentItem] {
        if (auto it = currentItem()) it->qty().set(it->qty_value() + 1);
    });
    QObject::connect(minusBtn, &QPushButton::clicked, [currentItem] {
        if (auto it = currentItem()) {
            const int n = it->qty_value();
            if (n > 1) it->qty().set(n - 1);
        }
    });
    QObject::connect(delBtn, &QPushButton::clicked, [listView, &vm] {
        auto idx = listView->currentIndex();
        if (idx.isValid()) vm.items.remove_at(static_cast<std::size_t>(idx.row()));
    });
    // Summary area
    auto* countLbl    = new QLabel;
    auto* subtotalLbl = new QLabel;
    auto* taxLbl      = new QLabel;
    auto* totalLbl    = new QLabel;
    lay->addWidget(countLbl);
    lay->addWidget(subtotalLbl);
    lay->addWidget(taxLbl);
    lay->addWidget(totalLbl);
    auto fmt = [](double v) { return QString::number(v, 'f', 2); };
    auto sC = [countLbl](int n)             { countLbl   ->setText(QString(QString::fromStdString(wb::i18n::str_in("cart", "count"))).arg(n)); };
    auto sS = [subtotalLbl, fmt](double v)  { subtotalLbl->setText(QString::fromStdString(wb::i18n::str_in("cart", "subtotal")) + fmt(v)); };
    auto sT = [taxLbl, fmt](double v)       { taxLbl     ->setText(QString::fromStdString(wb::i18n::str_in("cart", "tax")) + fmt(v)); };
    auto sG = [totalLbl, fmt](double v)     { totalLbl   ->setText(QString::fromStdString(wb::i18n::str_in("cart", "total")) + fmt(v)); };
    sC(vm.itemCount.get());
    sS(vm.subtotal .get());
    sT(vm.tax      .get());
    sG(vm.total    .get());
    s_subs.push_back(vm.itemCount.on_changed(sC));
    s_subs.push_back(vm.subtotal .on_changed(sS));
    s_subs.push_back(vm.tax      .on_changed(sT));
    s_subs.push_back(vm.total    .on_changed(sG));
}

}  // namespace wb::cart::qtview
