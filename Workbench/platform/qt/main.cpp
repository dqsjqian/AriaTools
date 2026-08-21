// ────────────────────────────────────────────────────────────────────────────
//  Workbench — Cross-platform workbench (Qt6 desktop).
//
//  Layout: left nav list + right stacked content area. Switching nav items activates/deactivates the corresponding VM.
//  All business logic lives in module libraries (pure C++, no Qt); this file only does assembly + shell.
//  All text comes from i18n (VM dispatch / core.nav_title), zero hard-coded text in View.
// ────────────────────────────────────────────────────────────────────────────
#include "App/QtAppShell.h"
#include "support/QtViewFactory.h"
#include "infra/log/Log.h"
#include "utils/Platform.h"

#include "aria/binding/view_model.hpp"

#include <QApplication>
#include <QHBoxLayout>
#include <QListWidget>
#include <QMainWindow>
#include <QStackedWidget>
#include <QWidget>

#include <memory>
#include <string_view>
#include <vector>

namespace {

bool has_arg(int argc, char** argv, std::string_view expected) {
    for (int i = 1; i < argc; ++i) {
        if (std::string_view{argv[i]} == expected) return true;
    }
    return false;
}

int run_probe(wb::qt::QtAppShell& shell) {
    auto& core = shell.core();
    const auto& modules = core.modules();
    if (modules.empty()) {
        WB_LOGE("probe", "no business modules loaded");
        return 2;
    }

    std::vector<std::unique_ptr<QWidget>> pages;
    pages.reserve(modules.size());
    for (int i = 0; i < static_cast<int>(modules.size()); ++i) {
        const auto& module = modules[static_cast<std::size_t>(i)];
        if (module.id.empty() || module.navKey.empty() || !module.vm) {
            WB_LOGE("probe", "invalid module entry at index " << i);
            return 3;
        }
        if (!wb::qt::QtViewFactory::instance().has_builder(module.id)) {
            WB_LOGE("probe", "missing Qt View builder for module " << module.id);
            return 4;
        }
        auto page = std::unique_ptr<QWidget>{shell.build_page(i)};
        if (!page) {
            WB_LOGE("probe", "View builder returned null for module " << module.id);
            return 5;
        }
        pages.push_back(std::move(page));
    }

    WB_LOGI("probe", "all " << modules.size()
            << " modules and Qt Views instantiated successfully");
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    wb::log::init_default_sink();
    WB_LOGI("app", "Workbench starting on " << wb::platform_name());

    wb::qt::QtAppShell shell{&app};
    auto& core = shell.core();

    if (has_arg(argc, argv, "--probe")) {
        return run_probe(shell);
    }

    QMainWindow win;
    // Window title from i18n common/app_name.
    win.setWindowTitle(QString::fromStdString(core.i18n().tr("common", "app_name")));
    win.resize(960, 680);

    auto* central = new QWidget;
    auto* root = new QHBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    auto* nav = new QListWidget;
    nav->setFixedWidth(180);
    nav->setStyleSheet(
        "QListWidget { background:#263238; border:none; font-size:14px; outline:none; }"
        "QListWidget::item { padding:12px 16px; color:#eceff1; }"
        "QListWidget::item:hover { background:#37474f; }"
        "QListWidget::item:selected { background:#1a237e; color:#ffffff; }");

    auto* stack = new QStackedWidget;
    stack->setContentsMargins(16, 16, 16, 16);

    const auto& mods = core.modules();
    for (int i = 0; i < static_cast<int>(mods.size()); ++i) {
        const auto& m = mods[static_cast<std::size_t>(i)];
        nav->addItem(QString::fromStdString(core.nav_title(m.navKey)));
        stack->addWidget(shell.build_page(i));
    }

    root->addWidget(nav);
    root->addWidget(stack, 1);
    win.setCentralWidget(central);

    // Nav text refreshes instantly on language change. Keep the subscription
    // local so it disconnects before the window, nav, and shell are destroyed.
    aria::Subscription langSub =
        core.i18n().language().on_changed(
            [&core, nav, &win](const std::string&) {
                const auto& ms = core.modules();
                for (int i = 0; i < static_cast<int>(ms.size()); ++i)
                    if (auto* it = nav->item(i))
                        it->setText(QString::fromStdString(core.nav_title(ms[static_cast<std::size_t>(i)].navKey)));
                win.setWindowTitle(QString::fromStdString(core.i18n().tr("common", "app_name")));
            });

    // VM lifetime follows the current nav item.
    auto vm_at = [&core](int i) -> std::shared_ptr<aria::binding::ViewModel> {
        const auto& ms = core.modules();
        if (i < 0 || i >= static_cast<int>(ms.size())) return nullptr;
        return ms[static_cast<std::size_t>(i)].vm;
    };

    int last = 0;
    if (auto vm = vm_at(0)) vm->activate();
    QObject::connect(nav, &QListWidget::currentRowChanged, [&, vm_at](int idx) {
        if (idx < 0) return;
        if (auto prev = vm_at(last)) prev->deactivate();
        if (auto cur  = vm_at(idx))  cur->activate();
        stack->setCurrentIndex(idx);
        last = idx;
    });
    nav->setCurrentRow(0);

    QObject::connect(&app, &QApplication::aboutToQuit, [&, vm_at] {
        if (auto cur = vm_at(last)) cur->deactivate();
    });

    win.show();
    return app.exec();
}
