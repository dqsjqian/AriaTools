#pragma once
//
// ToolsView — Qt view for the "tools" module.
//
// Decomposed into sub-views, each owning its own QGroupBox + bindings:
//   base64 group   — base64 encode/decode
//   random group   — random string generator
//   json group     — JSON format/minify
//   file crypto    — file encrypt/decrypt
//
// The top-level ToolsView constructor assembles them into a vertical layout
// and wires the shared VM.
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::tools { class ToolsVm; }

namespace wb::tools::qtview {

class ToolsView {
public:
    ToolsView(ToolsVm& vm, aria::binding::BindingEngine& be);
    QWidget* widget() const { return root_; }
private:
    QWidget* root_;
};

}  // namespace wb::tools::qtview
