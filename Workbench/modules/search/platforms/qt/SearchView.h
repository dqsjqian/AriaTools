#pragma once
//
// SearchView — Qt view for the "search" module (Aria free-function view).
//
// Uses the HostVm pattern: build_view() receives SearchVmHostVm (so it can
// read host.desc) and derives the inner SearchVm from it.
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::search { class SearchVmHostVm; }

namespace wb::search::qtview {

QWidget* build_view(SearchVmHostVm& host, aria::binding::BindingEngine& be);

}  // namespace wb::search::qtview
