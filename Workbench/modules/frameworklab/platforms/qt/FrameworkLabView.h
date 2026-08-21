#pragma once

#include "aria/binding/binding_engine.hpp"
#include "aria/subscription.hpp"

#include <QWidget>
#include <vector>

namespace wb::frameworklab { class FrameworkLabVm; }

namespace wb::frameworklab::qtview {

class FrameworkLabView {
public:
    FrameworkLabView(FrameworkLabVm& vm, aria::binding::BindingEngine& be);
    QWidget* widget() const { return root_; }

private:
    QWidget* root_;
};

}  // namespace wb::frameworklab::qtview
