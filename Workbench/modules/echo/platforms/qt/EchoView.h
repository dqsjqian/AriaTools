#pragma once
#include "support/UiHelpers.h"
#include "aria/binding/binding_engine.hpp"
#include <QWidget>
namespace wb::echo { class EchoVm; }
namespace wb::echo::qtview {
class EchoView {
public:
    EchoView(EchoVm& vm, aria::binding::BindingEngine& be);
    QWidget* widget() const { return root_; }
private:
    QWidget* root_;
};
}  // namespace wb::echo::qtview
