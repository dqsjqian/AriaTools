#pragma once
//
// QtAppShell — Qt 端外壳：持有平台相关的 dispatcher/adapter/BindingEngine，
// 复用平台无关的 wb::core::AppCore（infra 服务 + 各模块 VM）。
// View 通过 QtViewFactory 按 moduleId 构建。
//
#include "aria/adapters/qt6/qt_adapter.hpp"
#include "aria/adapters/qt6/qt_dispatcher.hpp"
#include "aria/binding/binding_engine.hpp"

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

    /// 用 QtViewFactory 为第 index 个模块构建 View。
    QWidget* build_page(int index);

    /// i18n 运行目录：可执行文件旁 i18n/；开发期回退源码树。
    static std::string resolve_i18n_dir();

private:
    std::shared_ptr<aria::adapters::qt6::QtDispatcher> dispatcher_;
    std::shared_ptr<aria::adapters::qt6::QtAdapter>    adapter_;
    aria::binding::BindingEngine                       be_;
    wb::core::AppCore                                  core_;
};

}  // namespace wb::qt
