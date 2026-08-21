#include "AndroidShell.h"

#include "JniBind.h"

#include "platforms/android/CalendarJniBinding.h"
#include "platforms/android/DashboardJniBinding.h"
#include "platforms/android/FrameworkLabJniBinding.h"
#include "platforms/android/CartJniBinding.h"
#include "platforms/android/ChatJniBinding.h"
#include "platforms/android/LoginJniBinding.h"
#include "platforms/android/NotesJniBinding.h"
#include "platforms/android/SearchJniBinding.h"
#include "platforms/android/SettingsJniBinding.h"
#include "platforms/android/SignupJniBinding.h"
#include "platforms/android/SyncJniBinding.h"
#include "platforms/android/ThemeJniBinding.h"
#include "platforms/android/TipCalcJniBinding.h"
#include "platforms/android/ToolsJniBinding.h"
#include "platforms/android/UnitConvertJniBinding.h"
#include "platforms/android/WizardJniBinding.h"

namespace wb::android {

namespace {

/// Per-module JNI binding table (set_text / execute_command entries).
/// Each module registers itself by module id; the shell dispatches through
/// the table so no module-specific if-else lives here.
const wb::jni::BindingTable& module_bindings() {
    static const wb::jni::BindingTable table = [] {
        wb::jni::BindingTable t;
        wb::calendar::register_calendar_binding(t);
        wb::dashboard::register_dashboard_binding(t);
        wb::frameworklab::register_frameworklab_binding(t);
        wb::cart::register_cart_binding(t);
        wb::chat::register_chat_binding(t);
        wb::login::register_login_binding(t);
        wb::notes::register_notes_binding(t);
        wb::search::register_search_binding(t);
        wb::settings::register_settings_binding(t);
        wb::signup::register_signup_binding(t);
        wb::sync::register_sync_binding(t);
        wb::theme::register_theme_binding(t);
        wb::tipcalc::register_tipcalc_binding(t);
        wb::tools::register_tools_binding(t);
        wb::unitconvert::register_unitconvert_binding(t);
        wb::wizard::register_wizard_binding(t);
        return t;
    }();
    return table;
}

}  // namespace

AndroidShell::AndroidShell(std::string i18nBaseDir,
                           aria::async::IExecutor& uiExecutor,
                           aria::IDelayedScheduler& timer,
                           std::string initialLang)
    : core_(std::move(i18nBaseDir), std::move(initialLang)) {
    core_.set_ui_executor(&uiExecutor);
    core_.set_timer(&timer);
    core_.load_modules();
    for (const auto& m : core_.modules()) {
        modules_.push_back({m.id, m.navKey, core_.nav_title(m.navKey)});
    }
}

AndroidShell::~AndroidShell() {
    if (active_ && active_->is_active().get()) {
        active_->deactivate();
    }
    active_.reset();
}

void AndroidShell::activate_module(const std::string& id) {
    for (auto& m : core_.modules()) {
        if (m.id != id) {
            continue;
        }
        if (active_ && active_.get() != m.vm.get() && active_->is_active().get()) {
            active_->deactivate();
        }
        active_ = m.vm;
        if (!active_->is_active().get()) {
            active_->activate();
        }
        return;
    }
}

// ── Kotlin→C++ property write ──────────────────────────────────────────────
//  Routes by moduleId to the module's set_<mod>_text(), which in turn routes
//  to the underlying Property::set. Only the properties that are genuinely
//  user-editable from the View are handled there; read-only / Computed
//  properties are pushed C++→Kotlin only.
void AndroidShell::set_text(const std::string& moduleId, const std::string& propName,
                            const std::string& value) {
    const auto& table = module_bindings();
    for (auto& entry : core_.modules()) {
        if (entry.id != moduleId) continue;
        auto it = table.find(moduleId);
        if (it != table.end() && it->second.set_text) {
            it->second.set_text(*entry.vm, propName, value);
        }
        return;
    }
}

// ── Kotlin→C++ command execution ───────────────────────────────────────────
//  Routes by moduleId to the module's exec_<mod>_command(), which in turn
//  routes to the underlying Command::execute.
void AndroidShell::execute_command(const std::string& moduleId, const std::string& cmdName) {
    const auto& table = module_bindings();
    for (auto& entry : core_.modules()) {
        if (entry.id != moduleId) continue;
        auto it = table.find(moduleId);
        if (it != table.end() && it->second.exec_command) {
            it->second.exec_command(*entry.vm, cmdName);
        }
        return;
    }
}

}  // namespace wb::android
