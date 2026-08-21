#pragma once
//
// QtAppShell — Qt-side shell: holds platform-specific dispatcher/adapter/BindingEngine,
// reuses the platform-agnostic wb::core::AppCore (infra services + each module's VM).
// Views are built by QtViewFactory keyed by moduleId.
//
#include "aria/adapters/qt6/qt_adapter.hpp"
#include "aria/adapters/qt6/qt_dispatcher.hpp"
#include "aria/binding/binding_engine.hpp"
#include "aria/runtime/dispatcher_executor.hpp"

#include "app/AppCore.h"

#include <QObject>
#include <QWidget>

#include <memory>
#include <string>

namespace wb::qt {

class QtAppShell {
public:
    explicit QtAppShell(QObject* qt_ctx);
    ~QtAppShell();

    QtAppShell(const QtAppShell&) = delete;
    QtAppShell& operator=(const QtAppShell&) = delete;

    [[nodiscard]] wb::core::AppCore& core() { return core_; }

    /// Build a View for the index-th module using QtViewFactory.
    QWidget* build_page(int index);

    /// i18n runtime directory: i18n/ next to the executable; falls back to the source tree during development.
    static std::string resolve_i18n_dir();

private:
    std::shared_ptr<aria::adapters::qt6::QtDispatcher> dispatcher_;
    aria::runtime::DispatcherExecutor                  ui_exec_;
    aria::runtime::DispatcherScheduler                 delay_;
    std::shared_ptr<aria::adapters::qt6::QtAdapter>   adapter_;
    aria::binding::BindingEngine                      be_;
    wb::core::AppCore                                 core_;
};

}  // namespace wb::qt
