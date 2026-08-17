#pragma once
//
// CalendarViewController — MVC Controller for the "calendar" module (iOS).
//
// Follows Aria demo3 pattern: Controller inherits UIViewController, owns
// a reference to the VM + BindingEngine, creates the CalendarView in
// loadView, and wires all bindings in viewDidLoad. The View is pure UI;
// all VM↔View binding logic lives here.
//
// VM lifetime is managed by AppCore (longer than the Controller); the
// Controller holds a raw reference, not a shared_ptr.
//
#import <UIKit/UIKit.h>

#include "aria/binding/binding_engine.hpp"

namespace wb::calendar { class CalendarVm; }

@interface CalendarViewController : UIViewController

- (instancetype)initWithViewModelRef:(wb::calendar::CalendarVm&)vm
                                engine:(aria::binding::BindingEngine&)be;

@end
