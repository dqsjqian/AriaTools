#pragma once
//
// SearchView — iOS UIKit view for the "search" module.
//
// The View receives the HostVm (SearchVmHostVm) so it can bind
// host-owned title/desc properties; the inner SearchVm is accessed
// via host.inner().
//
#import <UIKit/UIKit.h>

#include "aria/binding/binding_engine.hpp"

namespace wb::search { class SearchVmHostVm; }

namespace wb::search::iosview {

class SearchView {
public:
    SearchView(SearchVmHostVm& host, aria::binding::BindingEngine& be);
    UIViewController* viewController() const { return vc_; }
private:
    UIViewController* vc_;
};

}  // namespace wb::search::iosview
