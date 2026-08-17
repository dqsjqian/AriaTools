#include "CartView.h"
#include "support/IosUi.h"
#include "infra/i18n/I18n.h"
#include "viewmodels/CartVm.h"

#include "aria/binding/binding_engine.hpp"
#include "aria/binding/converter.hpp"

#include <optional>
#include <string>

namespace wb::cart::iosview {

namespace {

aria::binding::Converter<double, std::string> make_dbl_conv() {
    return aria::binding::Converter<double, std::string>{
        [](const double& v) {
            char buf[32]; snprintf(buf, sizeof buf, "%.2f", v); return std::string(buf);
        },
        [](const std::string& s) { return std::stod(s); },
        [](const std::string& s) -> std::optional<double> {
            try { return std::stod(s); } catch (...) { return std::nullopt; }
        }};
}

}  // namespace

CartView::CartView(CartVm& vm, aria::binding::BindingEngine& be)
    : vc_(nil) {
    UILabel*     title     = wb::ios::ui::make_title(@"");
    UILabel*     desc      = wb::ios::ui::make_label(@"");
    UILabel*     nameLbl   = wb::ios::ui::make_label(@"");
    UITextField* nameField  = wb::ios::ui::make_field(@"");
    UILabel*     priceLbl  = wb::ios::ui::make_label(@"");
    UITextField* priceField = wb::ios::ui::make_field(@"");
    UIButton*    addBtn    = wb::ios::ui::make_button(@"");
    UILabel*     countLbl  = wb::ios::ui::make_label(@"");
    UILabel*     subLbl     = wb::ios::ui::make_label(@"");
    UILabel*     taxLbl     = wb::ios::ui::make_label(@"");
    UILabel*     totalLbl   = wb::ios::ui::make_label(@"");

    vc_ = wb::ios::ui::make_stack_vc(
        @[title, desc, nameLbl, nameField, priceLbl, priceField, addBtn,
          countLbl, subLbl, taxLbl, totalLbl]);

    be.bind_text_oneway(vm.title, wb::ios::ui::view_for(title));
    be.bind_text_oneway(vm.desc,  wb::ios::ui::view_for(desc));

    // Field labels (i18n).
    nameLbl.text  = [NSString stringWithUTF8String:wb::i18n::str_in("cart","name_label").c_str()];
    priceLbl.text = [NSString stringWithUTF8String:wb::i18n::str_in("cart","price_label").c_str()];
    [addBtn setTitle:[NSString stringWithUTF8String:wb::i18n::str_in("cart","add").c_str()]
            forState:UIControlStateNormal];

    be.bind_text(vm.draftName,  wb::ios::ui::view_for(nameField));
    be.bind_text_converted(vm.draftPrice, wb::ios::ui::view_for(priceField), make_dbl_conv());
    be.bind_command(vm.addItem, wb::ios::ui::view_for(addBtn));

    // Summary values are Computed<T>; keep the manual subscriptions scoped
    // to this View wrapper until BindingEngine gains read-only source support.
    auto sync_count = [countLbl](int value) {
        const auto text = wb::i18n::str_in("cart", "count")
                        + " " + std::to_string(value);
        countLbl.text = [NSString stringWithUTF8String:text.c_str()];
    };
    auto fmt_money = [](const char* key, double value) {
        char buf[32];
        snprintf(buf, sizeof buf, "%.2f", value);
        return wb::i18n::str_in("cart", key) + std::string(buf);
    };
    auto sync_money = [fmt_money](UILabel* label, const char* key, double value) {
        const auto text = fmt_money(key, value);
        label.text = [NSString stringWithUTF8String:text.c_str()];
    };

    sync_count(vm.itemCount.get());
    subscriptions_.push_back(vm.itemCount.on_changed(
        [sync_count](int value) { sync_count(value); }));
    sync_money(subLbl, "subtotal", vm.subtotal.get());
    subscriptions_.push_back(vm.subtotal.on_changed(
        [sync_money, subLbl](double value) { sync_money(subLbl, "subtotal", value); }));
    sync_money(taxLbl, "tax", vm.tax.get());
    subscriptions_.push_back(vm.tax.on_changed(
        [sync_money, taxLbl](double value) { sync_money(taxLbl, "tax", value); }));
    sync_money(totalLbl, "total", vm.total.get());
    subscriptions_.push_back(vm.total.on_changed(
        [sync_money, totalLbl](double value) { sync_money(totalLbl, "total", value); }));
}

}  // namespace wb::cart::iosview
