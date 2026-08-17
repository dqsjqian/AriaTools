#pragma once
//
// NotesView — iOS UIKit view for the "notes" module.
//
#import <UIKit/UIKit.h>

#include "aria/binding/binding_engine.hpp"

namespace wb::notes { class NotesVm; }

namespace wb::notes::iosview {

class NotesView {
public:
    NotesView(NotesVm& vm, aria::binding::BindingEngine& be);
    UIViewController* viewController() const { return vc_; }
private:
    UIViewController* vc_;
};

}  // namespace wb::notes::iosview
