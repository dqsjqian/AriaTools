#include "AndroidShell.h"

#include "viewmodels/DashboardVm.h"
#include "viewmodels/NotesVm.h"
#include "viewmodels/CalendarVm.h"
#include "viewmodels/ToolsVm.h"
#include "viewmodels/SettingsVm.h"
#include "viewmodels/SyncVm.h"
#include "viewmodels/TipCalcVm.h"
#include "viewmodels/UnitConvertVm.h"
#include "viewmodels/CartVm.h"
#include "viewmodels/SignupVm.h"
#include "viewmodels/SearchVm.h"
#include "viewmodels/LoginVm.h"
#include "viewmodels/ChatVm.h"
#include "viewmodels/ThemeVm.h"
#include "viewmodels/WizardVm.h"

#include "viewmodels/UnitConvertVmHostVm.h"
#include "viewmodels/SignupVmHostVm.h"
#include "viewmodels/SearchVmHostVm.h"
#include "viewmodels/ThemeVmHostVm.h"
#include "viewmodels/WizardVmHostVm.h"

namespace wb::android {

AndroidShell::AndroidShell(std::string i18nBaseDir, std::string initialLang)
    : core_(std::move(i18nBaseDir), std::move(initialLang)) {
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
//  Routes by (moduleId, propName) to the underlying Property<string>::set.
//  Only the properties that are genuinely user-editable from the View are
//  listed here; read-only / Computed properties are pushed C++→Kotlin only.
void AndroidShell::set_text(const std::string& moduleId, const std::string& propName,
                             const std::string& value) {
    using namespace std::string_literals;
    for (auto& entry : core_.modules()) {
        if (entry.id != moduleId) continue;
        auto& vm = *entry.vm;

        if (moduleId == "dashboard") {
            auto& d = static_cast<dashboard::DashboardVm&>(vm);
            (void)d;  // no editable string props
        } else if (moduleId == "notes") {
            auto& n = static_cast<notes::NotesVm&>(vm);
            if (propName == "editTitle") n.editTitle.set(value);
            else if (propName == "editBody")  n.editBody.set(value);
        } else if (moduleId == "calendar") {
            auto& c = static_cast<calendar::CalendarVm&>(vm);
            if (propName == "subscribeUrl") c.subscribeUrl.set(value);
        } else if (moduleId == "tools") {
            auto& t = static_cast<tools::ToolsVm&>(vm);
            if (propName == "base64Input")      t.base64Input.set(value);
            else if (propName == "randomInput")  t.randomInput.set(value);
            else if (propName == "jsonInput")    t.jsonInput.set(value);
        } else if (moduleId == "settings") {
            // Language switch goes through the command, not a property set.
        } else if (moduleId == "sync") {
            auto& s = static_cast<sync::SyncVm&>(vm);
            if (propName == "dataDir")      s.dataDir.set(value);
            else if (propName == "remote")  s.remoteUrl.set(value);
            else if (propName == "branch")  s.branch.set(value);
            else if (propName == "username")s.username.set(value);
            else if (propName == "token")   s.token.set(value);
        } else if (moduleId == "tipcalc") {
            // Numeric inputs go through nativeSetDouble / nativeSetInt,
            // not set_text. No string props to set here.
        } else if (moduleId == "tipcalc") {
            // Numeric inputs arrive as strings; parse to the underlying type.
            auto& t = static_cast<tipcalc::TipCalcVm&>(vm);
            try {
                if (propName == "bill")       t.bill.set(std::stod(value));
                else if (propName == "tipPercent") t.tipPercent.set(std::stoi(value));
                else if (propName == "people")     t.people.set(std::stoi(value));
            } catch (...) { /* ignore malformed numeric input */ }
        } else if (moduleId == "unitconvert") {
            auto& host = static_cast<unitconvert::UnitConvertVmHostVm&>(vm);
            try {
                if (propName == "value") host.inner().value.set(std::stod(value));
            } catch (...) {}
        } else if (moduleId == "cart") {
            auto& c = static_cast<cart::CartVm&>(vm);
            if (propName == "draftName") c.draftName.set(value);
            else try { if (propName == "draftPrice") c.draftPrice.set(std::stod(value)); } catch (...) {}
        } else if (moduleId == "signup") {
            auto& host = static_cast<signup::SignupVmHostVm&>(vm);
            auto& s = host.inner();
            if (propName == "username") s.username.value.set(value);
            else if (propName == "email")    s.email.value.set(value);
            else if (propName == "password") s.password.value.set(value);
            else if (propName == "confirm")  s.confirm.value.set(value);
        } else if (moduleId == "search") {
            auto& host = static_cast<search::SearchVmHostVm&>(vm);
            if (propName == "query") host.inner().query.set(value);
        } else if (moduleId == "login") {
            auto& l = static_cast<login::LoginVm&>(vm);
            if (propName == "username") l.username.set(value);
            else if (propName == "password") l.password.set(value);
        } else if (moduleId == "chat") {
            auto& c = static_cast<chat::ChatVm&>(vm);
            if (propName == "user")      c.publisher->user.set(value);
            else if (propName == "draft") c.publisher->draft.set(value);
        } else if (moduleId == "theme") {
            // Theme picked via command, not property set.
        } else if (moduleId == "wizard") {
            auto& host = static_cast<wizard::WizardVmHostVm&>(vm);
            if (propName == "draftUsername") host.inner().draft->username.set(value);
            else if (propName == "draftEmail") host.inner().draft->email.set(value);
        }
        return;
    }
}

// ── Kotlin→C++ command execution ───────────────────────────────────────────
void AndroidShell::execute_command(const std::string& moduleId, const std::string& cmdName) {
    for (auto& entry : core_.modules()) {
        if (entry.id != moduleId) continue;
        auto& vm = *entry.vm;

        if (moduleId == "notes") {
            auto& n = static_cast<notes::NotesVm&>(vm);
            if (cmdName == "addNote")        n.addNote.execute();
            else if (cmdName == "saveNote")   n.saveNote.execute();
            else if (cmdName == "deleteSelected") n.deleteSelected.execute();
        } else if (moduleId == "calendar") {
            auto& c = static_cast<calendar::CalendarVm&>(vm);
            if (cmdName == "prevMonth")       c.prevMonth.execute();
            else if (cmdName == "nextMonth")  c.nextMonth.execute();
            else if (cmdName == "today")      c.today.execute();
            else if (cmdName == "refresh")     c.refresh.execute();
            else if (cmdName == "addSubscription") c.addSubscription.execute();
        } else if (moduleId == "settings") {
            auto& s = static_cast<settings::SettingsVm&>(vm);
            if (cmdName == "switchLanguage-zh-CN") s.switchLanguage.execute("zh-CN"s);
            else if (cmdName == "switchLanguage-en") s.switchLanguage.execute("en"s);
        } else if (moduleId == "sync") {
            auto& s = static_cast<sync::SyncVm&>(vm);
            if (cmdName == "saveConfig") s.saveConfig.execute();
            else if (cmdName == "syncNow") s.syncNow.execute();
            else if (cmdName == "pull")    s.pullOnly.execute();
            else if (cmdName == "push")    s.pushOnly.execute();
        } else if (moduleId == "tipcalc") {
            auto& t = static_cast<tipcalc::TipCalcVm&>(vm);
            if (cmdName == "roundUp") t.roundUp.execute();
        } else if (moduleId == "cart") {
            auto& c = static_cast<cart::CartVm&>(vm);
            if (cmdName == "addItem") c.addItem.execute();
        } else if (moduleId == "signup") {
            auto& host = static_cast<signup::SignupVmHostVm&>(vm);
            if (cmdName == "submit") host.inner().submit.execute();
        } else if (moduleId == "login") {
            auto& l = static_cast<login::LoginVm&>(vm);
            if (cmdName == "submit") l.submitCmd.execute();
        } else if (moduleId == "chat") {
            auto& c = static_cast<chat::ChatVm&>(vm);
            if (cmdName == "send") c.publisher->send.execute();
        } else if (moduleId == "theme") {
            auto& host = static_cast<theme::ThemeVmHostVm&>(vm);
            if (cmdName == "pickLight")      host.inner().pickLight.execute();
            else if (cmdName == "pickDark")  host.inner().pickDark.execute();
            else if (cmdName == "pickSolarized") host.inner().pickSolarized.execute();
        } else if (moduleId == "unitconvert") {
            auto& host = static_cast<unitconvert::UnitConvertVmHostVm&>(vm);
            if (cmdName == "selectTemperature") host.inner().selectTemperature.execute();
            else if (cmdName == "selectLength")  host.inner().selectLength.execute();
            else if (cmdName == "selectWeight")  host.inner().selectWeight.execute();
        } else if (moduleId == "wizard") {
            auto& host = static_cast<wizard::WizardVmHostVm&>(vm);
            if (cmdName == "pickLight")     host.inner().step2->pickLight.execute();
            else if (cmdName == "pickDark")  host.inner().step2->pickDark.execute();
            else if (cmdName == "pickSolarized") host.inner().step2->pickSolarized.execute();
            else if (cmdName == "finish")   host.inner().step3->finishCmd.execute();
        } else if (moduleId == "tools") {
            auto& t = static_cast<tools::ToolsVm&>(vm);
            if (cmdName == "encodeBase64") t.base64Encode.execute();
            else if (cmdName == "decodeBase64") t.base64Decode.execute();
            else if (cmdName == "generateRandom") t.genRandom.execute();
            else if (cmdName == "formatJson") t.jsonFormat.execute();
            else if (cmdName == "minifyJson") t.jsonMinify.execute();
        }
        return;
    }
}

}  // namespace wb::android
