#include "TipCalcView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/TipCalcVm.h"

#include "aria/binding/binding_engine.hpp"
#include "aria/binding/converter.hpp"

#include <optional>
#include <string>

namespace wb::tipcalc::iosview {

namespace {

std::string fmt2(double v) {
    char buf[32];
    snprintf(buf, sizeof buf, "%.2f", v);
    return buf;
}

std::string fmt0(double v) {
    char buf[32];
    snprintf(buf, sizeof buf, "%.0f", v);
    return buf;
}

aria::binding::Converter<double, std::string> make_dbl_conv() {
    return aria::binding::Converter<double, std::string>{
        [](const double& v) { return fmt2(v); },
        [](const std::string& s) { return std::stod(s); },
        [](const std::string& s) -> std::optional<double> {
            try { return std::stod(s); } catch (...) { return std::nullopt; }
        }};
}

aria::binding::Converter<int, std::string> make_int_conv() {
    return aria::binding::Converter<int, std::string>{
        [](const int& v) { return std::to_string(v); },
        [](const std::string& s) { return std::stoi(s); },
        [](const std::string& s) -> std::optional<int> {
            try { return std::stoi(s); } catch (...) { return std::nullopt; }
        }};
}

}  // namespace

TipCalcView::TipCalcView(TipCalcVm& vm, aria::binding::BindingEngine& be)
    : vc_(nil) {
    UILabel*  title   = wb::ios::ui::make_title(@"");
    UILabel*  desc    = wb::ios::ui::make_label(@"");
    UILabel*  billLbl = wb::ios::ui::make_label(@"");
    UITextField* billField = wb::ios::ui::make_field(@"");
    UILabel*  tipLbl  = wb::ios::ui::make_label(@"");
    UITextField* tipField = wb::ios::ui::make_field(@"");
    UILabel*  peopleLbl = wb::ios::ui::make_label(@"");
    UITextField* peopleField = wb::ios::ui::make_field(@"");
    UILabel*  tipAmountLbl = wb::ios::ui::make_label(@"");
    UILabel*  totalLbl  = wb::ios::ui::make_label(@"");
    UILabel*  perLbl    = wb::ios::ui::make_label(@"");
    UIButton* roundBtn  = wb::ios::ui::make_button(@"");

    vc_ = wb::ios::ui::make_stack_vc(@[
        title, desc,
        billLbl, billField,
        tipLbl, tipField,
        peopleLbl, peopleField,
        tipAmountLbl, totalLbl, perLbl,
        roundBtn]);

    // VM is the single source of truth; View binds only.
    be.bind_text_oneway(vm.title,       wb::ios::ui::view_for(title));
    be.bind_text_oneway(vm.desc,        wb::ios::ui::view_for(desc));
    be.bind_text_oneway(vm.billLabel,   wb::ios::ui::view_for(billLbl));
    be.bind_text_oneway(vm.tipLabel,    wb::ios::ui::view_for(tipLbl));
    be.bind_text_oneway(vm.peopleLabel, wb::ios::ui::view_for(peopleLbl));
    be.bind_command(vm.roundUp,         wb::ios::ui::view_for(roundBtn));

    // Two-way numeric fields via text converters.
    be.bind_text_converted(vm.bill,       wb::ios::ui::view_for(billField), make_dbl_conv());
    be.bind_text_converted(vm.tipPercent, wb::ios::ui::view_for(tipField),  make_int_conv());
    be.bind_text_converted(vm.people,     wb::ios::ui::view_for(peopleField), make_int_conv());

    // Read-only computed results (captions from i18n + formatted values).
    // Computed<T> is not Property<T>, so BindingEngine has no direct
    // overload; subscribe via on_changed and keep the subscription alive
    // via the process-wide keepalive bag (iOS has no per-VC SubscriptionBag).
    auto cap = [&vm](aria::Property<std::string>& caption, double v) {
        return caption.get() + ": " + fmt2(v);
    };
    auto& subs = wb::ios::ui::subs_keepalive();
    subs.push_back(vm.tipAmount.on_changed(
        [&vm, cap, tipAmountLbl](const double v) {
            tipAmountLbl.text = [NSString stringWithUTF8String:cap(vm.tipAmountText, v).c_str()];
        }));
    tipAmountLbl.text = [NSString stringWithUTF8String:cap(vm.tipAmountText, vm.tipAmount.get()).c_str()];
    subs.push_back(vm.total.on_changed(
        [&vm, cap, totalLbl](const double v) {
            totalLbl.text = [NSString stringWithUTF8String:cap(vm.totalText, v).c_str()];
        }));
    totalLbl.text = [NSString stringWithUTF8String:cap(vm.totalText, vm.total.get()).c_str()];
    subs.push_back(vm.perPerson.on_changed(
        [&vm, cap, perLbl](const double v) {
            perLbl.text = [NSString stringWithUTF8String:cap(vm.perPersonText, v).c_str()];
        }));
    perLbl.text = [NSString stringWithUTF8String:cap(vm.perPersonText, vm.perPerson.get()).c_str()];

    // Button text (i18n) via a projected binding on the caption property.
    be.bind_text_oneway(vm.roundUpText, wb::ios::ui::view_for(roundBtn));
}

}  // namespace wb::tipcalc::iosview

namespace wb::tipcalc {
void register_tipcalc_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "tipcalc", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto* view = new iosview::TipCalcView(static_cast<TipCalcVm&>(vm), be);
            return view->viewController();
        });
}
}  // namespace wb::tipcalc
