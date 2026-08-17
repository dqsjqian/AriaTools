#pragma once
//
// ToolsView — Qt view for the "tools" module (Aria free-function view).
//
// Decomposed into sub-views, each owning its own QGroupBox + bindings:
//   base64 group   — base64 encode/decode
//   random group   — random string generator
//   json group     — JSON format/minify
//   file crypto    — file encrypt/decrypt
//
// The top-level build_view() assembles them into a vertical layout and wires
// the shared VM. register_tools_view() registers the builder with the
// QtViewFactory.
//
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

namespace wb::tools { class ToolsVm; }

namespace wb::tools::qtview {

QWidget* build_view(ToolsVm& vm, aria::binding::BindingEngine& be);

}  // namespace wb::tools::qtview
