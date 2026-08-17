// ────────────────────────────────────────────────────────────────────────────
//  jni_bridge.cpp — JNI side-channel bridge between the Workbench C++ core
//  and the Kotlin/Compose UI layer.
//
//  Architecture (same pattern as Aria demo5):
//    C++ AppCore / module VMs (aria::Property, Command)
//      → on_changed subscription
//      → JNI callback (JniBridge.onPropertyChanged)
//      → Kotlin MutableStateFlow
//      → Compose recomposition
//
//  The C++ side owns ALL business logic; Kotlin is a thin StateFlow shell.
//  Naming follows the framework convention: ARIA_JNI_* (tech names, not
//  platform names) — mirroring demo5.
//
//  Threading: the Workbench VM layer runs on the Android main thread (the
//  shell is created and activated there), and the JNI callbacks below are
//  invoked synchronously from Property::set — safe for the current scope.
//  If a future module pushes from a background thread, marshal through the
//  main Looper (postToMain + a drain queue, as demo5 does).
// ────────────────────────────────────────────────────────────────────────────
#include <jni.h>
#include <android/log.h>

#include <memory>
#include <string>
#include <vector>

#include "AndroidShell.h"
#include "app/AppCore.h"

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

#include "module_api/BaseVm.h"

#include "aria/subscription.hpp"

static constexpr const char* kTag = "WbJniBridge";
static constexpr const char* kBridgeClass = "com/dqsjqian/ariatools/JniBridge";

#define WB_LOG(fmt, ...) \
    __android_log_print(ANDROID_LOG_INFO, kTag, fmt, ##__VA_ARGS__)

// ── Global JNI state ────────────────────────────────────────────────────────
static JavaVM* g_jvm = nullptr;
static jclass g_bridgeClass = nullptr;
static jmethodID g_onModulesChanged = nullptr;
static jmethodID g_onPropertyChanged = nullptr;

// ── The shell (owned by this bridge) ────────────────────────────────────────
static std::unique_ptr<wb::android::AndroidShell> g_shell;

// ── Property subscriptions (must outlive the owning VMs) ────────────────────
static std::vector<aria::Subscription> g_propertySubs;

// ── JNIEnv helpers ──────────────────────────────────────────────────────────
static JNIEnv* env_of() {
    JNIEnv* env = nullptr;
    if (g_jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        // Attach if this thread is not yet attached (JNI_OnLoad ran on the
        // main thread; callbacks may come from VM-owned threads).
        g_jvm->AttachCurrentThread(&env, nullptr);
    }
    return env;
}

/// Push a property change to Kotlin (JniBridge.onPropertyChanged).
static void push_property(const std::string& moduleId, const std::string& name,
                          const std::string& value) {
    JNIEnv* env = env_of();
    jstring jm = env->NewStringUTF(moduleId.c_str());
    jstring jn = env->NewStringUTF(name.c_str());
    jstring jv = env->NewStringUTF(value.c_str());
    env->CallStaticVoidMethod(g_bridgeClass, g_onPropertyChanged, jm, jn, jv);
    env->DeleteLocalRef(jv);
    env->DeleteLocalRef(jn);
    env->DeleteLocalRef(jm);
}

/// Subscribe a string Property and push its current + future values.
static void bind_str(const std::string& module, const std::string& name,
                     aria::Property<std::string>& prop) {
    g_propertySubs.push_back(prop.on_changed(
        [&module, &name](const std::string& v) { push_property(module, name, v); }));
    push_property(module, name, prop.get());
}

/// Subscribe a double Property (formatted as string) and push its current + future values.
static void bind_dbl(const std::string& module, const std::string& name,
                     aria::Property<double>& prop) {
    auto fmt = [](double v) {
        char buf[32]; snprintf(buf, sizeof buf, "%.3f", v);
        return std::string(buf);
    };
    g_propertySubs.push_back(prop.on_changed(
        [&module, &name, fmt](double v) { push_property(module, name, fmt(v)); }));
    push_property(module, name, fmt(prop.get()));
}

/// Subscribe an int Property (formatted as string).
static void bind_int(const std::string& module, const std::string& name,
                     aria::Property<int>& prop) {
    g_propertySubs.push_back(prop.on_changed(
        [&module, &name](int v) { push_property(module, name, std::to_string(v)); }));
    push_property(module, name, std::to_string(prop.get()));
}

// ── Per-module subscription ─────────────────────────────────────────────────
//  Each module subscribes its headline text + a few key state properties so
//  the Android Page can render real VM data instead of a placeholder shell.
//  List-valued modules (cart/chat/search/notes) push a count until a full
//  list bridge lands; the count alone is enough to prove the VM is live.

static void subscribe_all(wb::core::AppCore& core) {
    for (auto& entry : core.modules()) {
        const std::string& id = entry.id;
        if (id == "dashboard") {
            auto& vm = static_cast<wb::dashboard::DashboardVm&>(*entry.vm);
            bind_str(id, "welcome", vm.welcome);
            bind_str(id, "summary", vm.summary);
        } else if (id == "notes") {
            auto& vm = static_cast<wb::notes::NotesVm&>(*entry.vm);
            bind_str(id, "title", vm.title);
            bind_str(id, "hint",  vm.hint);
            bind_str(id, "status", vm.status);
            bind_int(id, "count", vm.count);
            bind_str(id, "editTitle", vm.editTitle);
            bind_str(id, "editBody",  vm.editBody);
            bind_str(id, "add",   vm.addLabel);
            bind_str(id, "save",  vm.saveLabel);
            bind_str(id, "delete",vm.deleteLabel);
            bind_str(id, "title_placeholder", vm.titlePlaceholder);
            bind_str(id, "body_placeholder",  vm.bodyPlaceholder);
            // Note list: push as newline-joined titles.
            auto sync_notes = [&vm]() {
                std::string joined;
                for (const auto& n : vm.notes.snapshot()) {
                    if (!joined.empty()) joined += "\n";
                    joined += n->title.empty() ? "(untitled)" : n->title;
                }
                push_property("notes", "noteList", joined);
            };
            sync_notes();
            g_propertySubs.push_back(vm.notes.on_any_change(
                [sync_notes]() { sync_notes(); }));
        } else if (id == "calendar") {
            auto& vm = static_cast<wb::calendar::CalendarVm&>(*entry.vm);
            bind_str(id, "title",      vm.title);
            bind_str(id, "hint",       vm.hint);
            bind_str(id, "monthTitle",  vm.monthTitle);
            bind_str(id, "status",      vm.status);
            bind_str(id, "subscribeUrl",vm.subscribeUrl);
            // Labels for nav buttons (i18n).
            bind_str(id, "prev",      vm.prevLabel);
            bind_str(id, "next",      vm.nextLabel);
            bind_str(id, "today",     vm.todayLabel);
            bind_str(id, "refresh",   vm.refreshLabel);
            bind_str(id, "subscribe", vm.subscribeLabel);
            bind_str(id, "url_placeholder", vm.urlPlaceholder);
            // Events list: join the current month's event titles into a
            // newline-joined string so the Compose LazyColumn can render.
            auto sync_events = [&vm]() {
                std::string joined;
                for (const auto& day : vm.days.snapshot()) {
                    if (!day || day->eventTitles.empty()) continue;
                    for (const auto& t : day->eventTitles) {
                        if (!joined.empty()) joined += "\n";
                        joined += day->label + "  " + t;
                    }
                }
                push_property("calendar", "events", joined);
            };
            sync_events();
            g_propertySubs.push_back(vm.days.on_any_change(
                [sync_events]() { sync_events(); }));
        } else if (id == "tools") {
            auto& vm = static_cast<wb::tools::ToolsVm&>(*entry.vm);
            bind_str(id, "title", vm.title);
            bind_str(id, "base64Input",  vm.base64Input);
            bind_str(id, "base64Output", vm.base64Output);
            bind_str(id, "randomInput",  vm.randomInput);
            bind_str(id, "randomOutput", vm.randomOutput);
            bind_str(id, "jsonInput",    vm.jsonInput);
            bind_str(id, "jsonOutput",   vm.jsonOutput);
            // Labels (i18n).
            bind_str(id, "base64_group", vm.base64GroupLabel);
            bind_str(id, "random_group", vm.randomGroupLabel);
            bind_str(id, "json_group",   vm.jsonGroupLabel);
            bind_str(id, "input",  vm.inputLabel);
            bind_str(id, "output", vm.outputLabel);
            bind_str(id, "encode", vm.encodeLabel);
            bind_str(id, "decode", vm.decodeLabel);
            bind_str(id, "length", vm.lengthLabel);
            bind_str(id, "generate", vm.generateLabel);
            bind_str(id, "format", vm.formatLabel);
            bind_str(id, "minify", vm.minifyLabel);
        } else if (id == "settings") {
            auto& vm = static_cast<wb::settings::SettingsVm&>(*entry.vm);
            bind_str(id, "title",         vm.title);
            bind_str(id, "hint",          vm.hint);
            bind_str(id, "language",      vm.language);
            bind_str(id, "languageLabel", vm.languageLabel);
        } else if (id == "sync") {
            auto& vm = static_cast<wb::sync::SyncVm&>(*entry.vm);
            bind_str(id, "title", vm.title);
            bind_str(id, "hint",  vm.hint);
            bind_str(id, "status", vm.status);
            // Config fields (two-way editable).
            bind_str(id, "dataDir",   vm.dataDir);
            bind_str(id, "remote",     vm.remoteUrl);
            bind_str(id, "branch",     vm.branch);
            bind_str(id, "username",   vm.username);
            bind_str(id, "token",      vm.token);
            // Labels (i18n).
            bind_str(id, "data_dir",    vm.dataDirLabel);
            bind_str(id, "remote_label",vm.remoteLabel);
            bind_str(id, "branch_label",vm.branchLabel);
            bind_str(id, "username",    vm.usernameLabel);
            bind_str(id, "token",       vm.tokenLabel);
            bind_str(id, "save_config", vm.saveLabel);
            bind_str(id, "sync_now",    vm.syncLabel);
            bind_str(id, "pull",        vm.pullLabel);
            bind_str(id, "push",        vm.pushLabel);
        } else if (id == "tipcalc") {
            auto& vm = static_cast<wb::tipcalc::TipCalcVm&>(*entry.vm);
            bind_str(id, "title", vm.title);
            bind_str(id, "desc",  vm.desc);
            bind_dbl(id, "bill",       vm.bill);
            bind_int(id, "tipPercent", vm.tipPercent);
            bind_int(id, "people",     vm.people);
            bind_dbl(id, "tipAmount",  vm.tipAmount);
            bind_dbl(id, "total",      vm.total);
            bind_dbl(id, "perPerson",  vm.perPerson);
            // Labels (i18n).
            bind_str(id, "bill_label",   vm.billLabel);
            bind_str(id, "tip_label",    vm.tipLabel);
            bind_str(id, "people_label", vm.peopleLabel);
            bind_str(id, "tip_amount",   vm.tipAmountText);
            bind_str(id, "total",        vm.totalText);
            bind_str(id, "per_person",   vm.perPersonText);
            bind_str(id, "round_up",     vm.roundUpText);
        } else if (id == "unitconvert") {
            auto& host = static_cast<wb::unitconvert::UnitConvertVmHostVm&>(*entry.vm);
            bind_str(id, "title", host.title);
            bind_str(id, "desc",  host.desc);
            bind_dbl(id, "value",     host.inner().value);
            bind_dbl(id, "converted", host.inner().converted);
            // Labels (i18n).
            bind_str(id, "cat_temperature", host.inner().catTemperatureLabel);
            bind_str(id, "cat_length",      host.inner().catLengthLabel);
            bind_str(id, "cat_weight",      host.inner().catWeightLabel);
            bind_str(id, "input",           host.inner().inputLabel);
            bind_str(id, "equals",          host.inner().equalsLabel);
        } else if (id == "cart") {
            auto& vm = static_cast<wb::cart::CartVm&>(*entry.vm);
            bind_str(id, "title", vm.title);
            bind_str(id, "desc",  vm.desc);
            bind_str(id, "draftName",  vm.draftName);
            bind_dbl(id, "draftPrice", vm.draftPrice);
            bind_int(id, "itemCount", vm.itemCount);
            bind_dbl(id, "subtotal",  vm.subtotal);
            bind_dbl(id, "tax",        vm.tax);
            bind_dbl(id, "total",      vm.total);
            // Labels (i18n) — pulled from common i18n at VM construction.
            bind_str(id, "name_label",  vm.nameLabel);
            bind_str(id, "price_label", vm.priceLabel);
            bind_str(id, "add",          vm.addLabel);
            bind_str(id, "count",        vm.countLabel);
            bind_str(id, "subtotal",     vm.subtotalLabel);
            bind_str(id, "tax",           vm.taxLabel);
            bind_str(id, "total",         vm.totalLabel);
            // Item list: push as newline-joined string.
            auto sync_items = [&vm]() {
                std::string joined;
                for (const auto& it : vm.items.snapshot()) {
                    if (!joined.empty()) joined += "\n";
                    joined += it->name() + " x" + std::to_string(it->qty_value());
                }
                push_property("cart", "items", joined);
            };
            sync_items();
            g_propertySubs.push_back(vm.items.on_any_change(
                [sync_items]() { sync_items(); }));
        } else if (id == "signup") {
            auto& host = static_cast<wb::signup::SignupVmHostVm&>(*entry.vm);
            bind_str(id, "title", host.title);
            bind_str(id, "desc",  host.desc);
            bind_str(id, "submittedSummary", host.inner().submittedSummary);
            // Per-field error messages.
            bind_str(id, "username_error", host.inner().username.error);
            bind_str(id, "email_error",    host.inner().email.error);
            bind_str(id, "password_error", host.inner().password.error);
            bind_str(id, "confirm_error",  host.inner().confirm.error);
        } else if (id == "search") {
            auto& host = static_cast<wb::search::SearchVmHostVm&>(*entry.vm);
            bind_str(id, "title", host.title);
            bind_str(id, "desc",  host.desc);
            bind_str(id, "query", host.inner().query);
            bind_str(id, "debounced", *host.inner().debounced);
            bind_str(id, "distinct",  *host.inner().distinct);
            bind_str(id, "placeholder", host.inner().placeholder);
            bind_str(id, "searches",   host.inner().searchesLabel);
            // Hits list: push as newline-joined strings.
            auto sync_hits = [&host]() {
                std::string joined;
                for (const auto& h : host.inner().hits.snapshot()) {
                    if (!joined.empty()) joined += "\n";
                    joined += "#" + std::to_string(h.seq) + " " + h.q;
                }
                push_property("search", "hits", joined);
            };
            sync_hits();
            g_propertySubs.push_back(host.inner().hits.on_any_change(
                [sync_hits]() { sync_hits(); }));
        } else if (id == "login") {
            auto& vm = static_cast<wb::login::LoginVm&>(*entry.vm);
            bind_str(id, "title", vm.title);
            bind_str(id, "desc",  vm.desc);
            bind_str(id, "username", vm.username);
            bind_str(id, "welcome",
                vm.login.last_result.get().has_value()
                    ? vm.login.last_result.get()->welcome
                    : std::string{});
            bind_str(id, "error", vm.login.last_error_message);
            bind_int(id, "is_executing",
                vm.login.is_executing.get() ? 1 : 0);
        } else if (id == "chat") {
            auto& vm = static_cast<wb::chat::ChatVm&>(*entry.vm);
            bind_str(id, "title", vm.title);
            bind_str(id, "desc",  vm.desc);
            bind_str(id, "user",  vm.publisher->user);
            bind_str(id, "draft", vm.publisher->draft);
            // Message list: push as a newline-joined string so the
            // Compose LazyColumn can render each line. Resync on any
            // list mutation (Insert/Remove/ItemChanged).
            auto sync_messages = [&vm]() {
                std::string joined;
                for (const auto& m : vm.subscriber->messages.snapshot()) {
                    if (!joined.empty()) joined += "\n";
                    joined += m->user + ": " + m->text;
                }
                push_property("chat", "messages", joined);
            };
            sync_messages();
            g_propertySubs.push_back(vm.subscriber->messages.on_any_change(
                [sync_messages]() { sync_messages(); }));
        } else if (id == "theme") {
            auto& host = static_cast<wb::theme::ThemeVmHostVm&>(*entry.vm);
            bind_str(id, "title", host.title);
            bind_str(id, "desc",  host.desc);
            bind_str(id, "currentId",          host.inner().currentId);
            bind_str(id, "currentDisplayName", host.inner().currentDisplayName);
            // Theme picker labels (i18n).
            bind_str(id, "theme_light",     host.inner().themeLightLabel);
            bind_str(id, "theme_dark",      host.inner().themeDarkLabel);
            bind_str(id, "theme_solarized", host.inner().themeSolarizedLabel);
            bind_str(id, "card_title",      host.inner().cardTitleLabel);
            bind_str(id, "card_body",       host.inner().cardBodyLabel);
        } else if (id == "wizard") {
            auto& host = static_cast<wb::wizard::WizardVmHostVm&>(*entry.vm);
            bind_str(id, "title", host.title);
            bind_str(id, "desc",  host.desc);
            bind_str(id, "step1",    host.step1Label);
            bind_str(id, "step2",    host.step2Label);
            bind_str(id, "step3",    host.step3Label);
            bind_str(id, "username", host.usernameLabel);
            bind_str(id, "email",     host.emailLabel);
            bind_str(id, "finish",    host.finishLabel);
            bind_str(id, "unfinished",host.unfinishedLabel);
            bind_str(id, "theme_light",     host.themeLightLabel);
            bind_str(id, "theme_dark",      host.themeDarkLabel);
            bind_str(id, "theme_solarized", host.themeSolarizedLabel);
            bind_str(id, "finishedSummary", host.inner().step3->finishedSummary);
            bind_str(id, "draftUsername", host.inner().draft->username);
            bind_str(id, "draftEmail",    host.inner().draft->email);
            bind_str(id, "draftTheme",     host.inner().draft->theme);
        }
    }
}

// ── Native entry points (mirrored by Kotlin JniBridge) ──────────────────────
extern "C" {

JNIEXPORT void JNICALL
Java_com_dqsjqian_ariatools_JniBridge_nativeCreateShell(JNIEnv* env, jclass,
                                                      jstring i18nDir) {
    if (g_shell) {
        g_shell.reset();
        g_propertySubs.clear();
    }
    const char* dir = env->GetStringUTFChars(i18nDir, nullptr);
    std::string i18n(dir);
    env->ReleaseStringUTFChars(i18nDir, dir);

    g_shell = std::make_unique<wb::android::AndroidShell>(i18n);

    // Report the module list to Kotlin (id[] + title[]).
    const auto& mods = g_shell->modules();
    jclass strCls = env->FindClass("java/lang/String");
    jobjectArray ids = env->NewObjectArray(
        static_cast<jsize>(mods.size()), strCls, nullptr);
    jobjectArray titles = env->NewObjectArray(
        static_cast<jsize>(mods.size()), strCls, nullptr);
    for (size_t i = 0; i < mods.size(); ++i) {
        env->SetObjectArrayElement(ids, static_cast<jsize>(i),
                                   env->NewStringUTF(mods[i].id.c_str()));
        env->SetObjectArrayElement(titles, static_cast<jsize>(i),
                                   env->NewStringUTF(mods[i].title.c_str()));
    }
    env->CallStaticVoidMethod(g_bridgeClass, g_onModulesChanged, ids, titles);
    env->DeleteLocalRef(ids);
    env->DeleteLocalRef(titles);
    env->DeleteLocalRef(strCls);

    subscribe_all(g_shell->core());
    WB_LOG("shell created; i18n=%s modules=%zu", i18n.c_str(), mods.size());
}

JNIEXPORT void JNICALL
Java_com_dqsjqian_ariatools_JniBridge_nativeDestroyShell(JNIEnv*, jclass) {
    g_propertySubs.clear();
    g_shell.reset();
}

JNIEXPORT void JNICALL
Java_com_dqsjqian_ariatools_JniBridge_nativeActivateModule(JNIEnv* env, jclass,
                                                         jstring moduleId) {
    if (!g_shell) {
        return;
    }
    const char* id = env->GetStringUTFChars(moduleId, nullptr);
    std::string mid(id);
    env->ReleaseStringUTFChars(moduleId, id);
    g_shell->activate_module(mid);
}

JNIEXPORT void JNICALL
Java_com_dqsjqian_ariatools_JniBridge_nativeSetProperty(JNIEnv* env, jclass,
                                                         jstring moduleId,
                                                         jstring propName,
                                                         jstring value) {
    if (!g_shell) return;
    const char* m = env->GetStringUTFChars(moduleId, nullptr);
    const char* p = env->GetStringUTFChars(propName, nullptr);
    const char* v = env->GetStringUTFChars(value, nullptr);
    g_shell->set_text(m, p, v);
    env->ReleaseStringUTFChars(moduleId, m);
    env->ReleaseStringUTFChars(propName, p);
    env->ReleaseStringUTFChars(value, v);
}

JNIEXPORT void JNICALL
Java_com_dqsjqian_ariatools_JniBridge_nativeExecuteCommand(JNIEnv* env, jclass,
                                                            jstring moduleId,
                                                            jstring cmdName) {
    if (!g_shell) return;
    const char* m = env->GetStringUTFChars(moduleId, nullptr);
    const char* c = env->GetStringUTFChars(cmdName, nullptr);
    g_shell->execute_command(m, c);
    env->ReleaseStringUTFChars(moduleId, m);
    env->ReleaseStringUTFChars(cmdName, c);
}

}  // extern "C"

// ── JNI_OnLoad ──────────────────────────────────────────────────────────────
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void*) {
    JNIEnv* env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    g_jvm = vm;

    jclass cls = env->FindClass(kBridgeClass);
    if (!cls) {
        __android_log_print(ANDROID_LOG_ERROR, kTag,
                            "JNI_OnLoad: %s not found", kBridgeClass);
        return JNI_ERR;
    }
    g_bridgeClass = static_cast<jclass>(env->NewGlobalRef(cls));
    env->DeleteLocalRef(cls);

    g_onModulesChanged = env->GetStaticMethodID(
        g_bridgeClass, "onModulesChanged",
        "([Ljava/lang/String;[Ljava/lang/String;)V");
    g_onPropertyChanged = env->GetStaticMethodID(
        g_bridgeClass, "onPropertyChanged",
        "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    if (!g_onModulesChanged || !g_onPropertyChanged) {
        __android_log_print(ANDROID_LOG_ERROR, kTag,
                            "JNI_OnLoad: bridge method(s) not found");
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}
