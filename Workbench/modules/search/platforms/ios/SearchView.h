#pragma once
//
// SearchView — iOS UIKit view for the "search" module.
//
// Binds directly to SearchVm.
//
#import <UIKit/UIKit.h>

#include "aria/binding/binding_engine.hpp"

namespace wb::search { class SearchVm; }

namespace wb::search::iosview {

class SearchView {
public:
    SearchView(SearchVm& host, aria::binding::BindingEngine& be);
    UIViewController* viewController() const { return vc_; }
private:
    UIViewController* __weak vc_;
};

}  // namespace wb::search::iosview
