#pragma once
//
// SearchView — Qt view for the "search" module.
//
// Binds directly to SearchVm.
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::search { class SearchVm; }

namespace wb::search::qtview {

class SearchView {
public:
    SearchView(SearchVm& host, aria::binding::BindingEngine& be);
    QWidget* widget() const { return root_; }
private:
    QWidget* root_;
};

}  // namespace wb::search::qtview
