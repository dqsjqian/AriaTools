#include "support/UIViewFactory.h"
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

static UIViewController* build(CartVm& vm, aria::binding::BindingEngine& be) {
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

    auto* vc = wb::ios::ui::make_stack_vc(
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

    // Summary: count / subtotal / tax / total — projected from VM Properties.
    be.bind_text_projected(vm.itemCount, wb::ios::ui::view_for(countLbl),
        [](const int n) {
            return wb::i18n::str_in("cart", "count")
                 + " " + std::to_string(n);
        });
    auto fmt_money = [](const char* key, double v) {
        char buf[32]; snprintf(buf, sizeof buf, "%.2f", v);
        return wb::i18n::str_in("cart", key) + std::string(buf);
    };
    be.bind_text_projected(vm.subtotal, wb::ios::ui::view_for(subLbl),
        [fmt_money](const double v) { return fmt_money("subtotal", v); });
    be.bind_text_projected(vm.tax,      wb::ios::ui::view_for(taxLbl),
        [fmt_money](const double v) { return fmt_money("tax", v); });
    be.bind_text_projected(vm.total,    wb::ios::ui::view_for(totalLbl),
        [fmt_money](const double v) { return fmt_money("total", v); });
    return vc;
}

}  // namespace wb::cart::iosview

namespace wb::cart {
void register_cart_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "cart", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            return iosview::build(static_cast<CartVm&>(vm), be);
        });
}
}  // namespace wb::cart
