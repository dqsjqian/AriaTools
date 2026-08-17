#pragma once
//
// SearchView — Qt view for the "search" module.
//
// Uses the HostVm pattern: the constructor receives SearchVmHostVm (so it
// can read host.desc) and derives the inner SearchVm from it.
//
#include "support/UiHelpers.h"
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::search { class SearchVmHostVm; }

namespace wb::search::qtview {

class SearchView {
public:
    SearchView(SearchVmHostVm& host, aria::binding::BindingEngine& be);
    QWidget* widget() const { return root_; }
private:
    QWidget* root_;
};

}  // namespace wb::search::qtview
