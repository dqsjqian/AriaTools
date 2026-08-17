#pragma once
//
// ThemeView — Qt view for the "theme" module.
//
// Binds directly to ThemeVm.
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::theme { class ThemeVm; }

namespace wb::theme::qtview {

class ThemeView {
public:
    ThemeView(ThemeVm& host, aria::binding::BindingEngine& be);
    QWidget* widget() const { return root_; }
private:
    QWidget* root_;
};

}  // namespace wb::theme::qtview
