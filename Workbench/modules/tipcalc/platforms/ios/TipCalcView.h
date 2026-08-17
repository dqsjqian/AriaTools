#pragma once
//
// TipCalcView — iOS UIKit view for the "tipcalc" module.
//
// Computed<T> results (tipAmount/total/perPerson) are subscribed via
// on_changed and kept alive through wb::ios::ui::subs_keepalive().
//
#import <UIKit/UIKit.h>

#include "aria/binding/binding_engine.hpp"

namespace wb::tipcalc { class TipCalcVm; }

namespace wb::tipcalc::iosview {

class TipCalcView {
public:
    TipCalcView(TipCalcVm& vm, aria::binding::BindingEngine& be);
    UIViewController* viewController() const { return vc_; }
private:
    UIViewController* vc_;
};

}  // namespace wb::tipcalc::iosview
