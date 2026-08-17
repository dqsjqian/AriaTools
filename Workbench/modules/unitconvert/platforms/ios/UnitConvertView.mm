#include "UnitConvertView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "infra/i18n/I18n.h"
#include "viewmodels/UnitConvertVm.h"
#include "viewmodels/UnitConvertVmHostVm.h"

#include "aria/binding/binding_engine.hpp"
#include "aria/binding/converter.hpp"

#include <optional>
#include <string>

namespace wb::unitconvert::iosview {

namespace {

aria::binding::Converter<double, std::string> make_dbl_conv() {
    return aria::binding::Converter<double, std::string>{
        [](const double& v) {
            char buf[32]; snprintf(buf, sizeof buf, "%.3f", v); return std::string(buf);
        },
        [](const std::string& s) { return std::stod(s); },
        [](const std::string& s) -> std::optional<double> {
            try { return std::stod(s); } catch (...) { return std::nullopt; }
        }};
}

}  // namespace

UnitConvertView::UnitConvertView(UnitConvertVmHostVm& host, aria::binding::BindingEngine& be)
    : vc_(nil) {
    auto& vm = host.inner();

    UILabel*     title   = wb::ios::ui::make_title(@"");
    UILabel*     desc    = wb::ios::ui::make_label(@"");
    UIButton*    catTemp = wb::ios::ui::make_button(@"");
    UIButton*    catLen  = wb::ios::ui::make_button(@"");
    UIButton*    catWt   = wb::ios::ui::make_button(@"");
    UILabel*     inputLbl = wb::ios::ui::make_label(@"");
    UITextField* valField = wb::ios::ui::make_field(@"");
    UILabel*     fromLbl  = wb::ios::ui::make_label(@"");
    UILabel*     outLbl   = wb::ios::ui::make_label(@"");

    vc_ = wb::ios::ui::make_stack_vc(
        @[title, desc, catTemp, catLen, catWt, inputLbl, valField, fromLbl, outLbl]);

    be.bind_text_oneway(host.title, wb::ios::ui::view_for(title));
    be.bind_text_oneway(host.desc,  wb::ios::ui::view_for(desc));

    // Category buttons: localized labels via i18n + command wiring.
    [catTemp setTitle:[NSString stringWithUTF8String:wb::i18n::str_in("unitconvert","cat_temperature").c_str()] forState:UIControlStateNormal];
    [catLen  setTitle:[NSString stringWithUTF8String:wb::i18n::str_in("unitconvert","cat_length").c_str()]     forState:UIControlStateNormal];
    [catWt   setTitle:[NSString stringWithUTF8String:wb::i18n::str_in("unitconvert","cat_weight").c_str()]     forState:UIControlStateNormal];
    be.bind_command(vm.selectTemperature, wb::ios::ui::view_for(catTemp));
    be.bind_command(vm.selectLength,      wb::ios::ui::view_for(catLen));
    be.bind_command(vm.selectWeight,      wb::ios::ui::view_for(catWt));

    // Two-way numeric input.
    be.bind_text_converted(vm.value, wb::ios::ui::view_for(valField), make_dbl_conv());

    // fromLabel / toLabel / converted are Computed<T>, not Property<T>,
    // so BindingEngine has no direct overload. Subscribe via on_changed
    // and keep subscriptions alive via the process-wide keepalive bag.
    auto& subs = wb::ios::ui::subs_keepalive();
    auto syncFrom = [fromLbl, &vm]() {
        fromLbl.text = [NSString stringWithUTF8String:vm.fromLabel.get().c_str()];
    };
    syncFrom();
    subs.push_back(vm.fromLabel.on_changed([syncFrom](const std::string&){ syncFrom(); }));
    auto syncOut = [outLbl, &vm]() {
        char buf[64];
        snprintf(buf, sizeof buf, "%.3f", vm.converted.get());
        outLbl.text = [NSString stringWithUTF8String:(std::string(buf) + " " + vm.toLabel.get()).c_str()];
    };
    syncOut();
    subs.push_back(vm.converted.on_changed([syncOut](double){ syncOut(); }));
    subs.push_back(vm.toLabel  .on_changed([syncOut](const std::string&){ syncOut(); }));
}

}  // namespace wb::unitconvert::iosview

namespace wb::unitconvert {
void register_unitconvert_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "unitconvert", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto& host = static_cast<UnitConvertVmHostVm&>(vm);
            auto* view = new iosview::UnitConvertView(host, be);
            return view->viewController();
        });
}
}  // namespace wb::unitconvert
