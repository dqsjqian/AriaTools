#pragma once
//
// ToolsView — Qt view for the "tools" module.
//
// Decomposed into sub-views, each owning its own QGroupBox + bindings:
//   Base64GroupView   — base64 encode/decode
//   RandomGroupView   — random string generator
//   JsonGroupView     — JSON format/minify
//   FileCryptoGroupView — file encrypt/decrypt
//
// The top-level ToolsView assembles them into a vertical layout and wires
// the shared VM. register_tools_view() is a thin entry point that
// constructs the view and returns its root widget.
//
#include "support/UiHelpers.h"
#include "aria/binding/binding_engine.hpp"

#include <QWidget>

#include <memory>

class QGroupBox;
class QLabel;

namespace wb::tools { class ToolsVm; }

namespace wb::tools::qtview {

class Base64GroupView {
public:
    Base64GroupView(ToolsVm& vm, aria::binding::BindingEngine& be,
                    std::vector<aria::Subscription>& subs);
    QGroupBox* widget() const { return box_; }
private:
    QGroupBox* box_;
};

class RandomGroupView {
public:
    RandomGroupView(ToolsVm& vm, aria::binding::BindingEngine& be,
                    std::vector<aria::Subscription>& subs);
    QGroupBox* widget() const { return box_; }
private:
    QGroupBox* box_;
};

class JsonGroupView {
public:
    JsonGroupView(ToolsVm& vm, aria::binding::BindingEngine& be,
                  std::vector<aria::Subscription>& subs);
    QGroupBox* widget() const { return box_; }
private:
    QGroupBox* box_;
};

class FileCryptoGroupView {
public:
    FileCryptoGroupView(ToolsVm& vm, aria::binding::BindingEngine& be,
                        std::vector<aria::Subscription>& subs);
    QGroupBox* widget() const { return box_; }
private:
    QGroupBox* box_;
};

class ToolsView {
public:
    ToolsView(ToolsVm& vm, aria::binding::BindingEngine& be);
    QWidget* widget() const { return root_; }
private:
    QWidget* root_;
    std::unique_ptr<Base64GroupView>      base64_;
    std::unique_ptr<RandomGroupView>      random_;
    std::unique_ptr<JsonGroupView>        json_;
    std::unique_ptr<FileCryptoGroupView>  fileCrypto_;
};

}  // namespace wb::tools::qtview
