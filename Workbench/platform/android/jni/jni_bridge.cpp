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
//
//  Per-module binding logic lives in each module's
//  platforms/android/<Mod>JniBinding.{h,cpp}; this file only dispatches by
//  module id through the binding registry (subscribe_<mod>() etc.), so a
//  module can be added/removed without touching the bridge — the same
//  registry contract the Qt/iOS ViewEntry pattern follows.
// ────────────────────────────────────────────────────────────────────────────
#include <jni.h>
#include <android/log.h>

#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "AndroidShell.h"
#include "JniBind.h"
#include "app/AppCore.h"

#include "aria/async/executor.hpp"
#include "aria/scheduler.hpp"
#include "aria/subscription.hpp"

#include "platforms/android/CalendarJniBinding.h"
#include "platforms/android/DashboardJniBinding.h"
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

static constexpr const char* kTag = "WbJniBridge";
static constexpr const char* kBridgeClass = "com/dqsjqian/ariatools/JniBridge";

#define WB_LOG(fmt, ...) \
    __android_log_print(ANDROID_LOG_INFO, kTag, fmt, ##__VA_ARGS__)

// ── Global JNI state ────────────────────────────────────────────────────────
static JavaVM* g_jvm = nullptr;
static jclass g_bridgeClass = nullptr;
static jmethodID g_onModulesChanged = nullptr;
static jmethodID g_onPropertyChanged = nullptr;
static jmethodID g_postToMain = nullptr;

// Work posted from worker threads is parked here, then drained by Kotlin's
// Handler(Looper.getMainLooper()) through nativeRunMainTasks().
static std::mutex g_mainQueueMutex;
static std::vector<std::function<void()>> g_mainQueue;

class AndroidMainScheduler final : public aria::async::IExecutor,
                                   public aria::IDelayedScheduler {
public:
    void bind_main_thread() noexcept {
        mainThreadId_ = std::this_thread::get_id();
    }

    void schedule(std::function<void()> fn) override {
        post(std::move(fn));
    }

    void post(std::function<void()> fn) override {
        {
            std::lock_guard lock(g_mainQueueMutex);
            g_mainQueue.push_back(std::move(fn));
        }
        JNIEnv* env = nullptr;
        bool attached = false;
        if (g_jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6)
            == JNI_EDETACHED) {
            if (g_jvm->AttachCurrentThread(&env, nullptr) != JNI_OK) return;
            attached = true;
        }
        env->CallStaticVoidMethod(g_bridgeClass, g_postToMain);
        if (attached) g_jvm->DetachCurrentThread();
    }

    void post_after(std::chrono::milliseconds delay,
                    std::function<void()> fn) override {
        std::thread([this, delay, fn = std::move(fn)]() mutable {
            std::this_thread::sleep_for(delay);
            post(std::move(fn));
        }).detach();
    }

    [[nodiscard]] aria::SchedulerCaps caps() const noexcept override {
        return aria::SchedulerCaps::Post
             | aria::SchedulerCaps::Delay
             | aria::SchedulerCaps::GraphSafe
             | aria::SchedulerCaps::MainThread
             | aria::SchedulerCaps::Autonomous;
    }

    [[nodiscard]] bool is_main_thread() const noexcept override {
        return std::this_thread::get_id() == mainThreadId_;
    }

private:
    std::thread::id mainThreadId_{};
};

static AndroidMainScheduler g_mainScheduler;

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
/// Registered with wb::jni::push_property_fn() from JNI_OnLoad so the module
/// JniBinding files can push without knowing the Java side.
static void push_property_impl(const std::string& moduleId,
                               const std::string& name,
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

// ── Per-module binding registry ─────────────────────────────────────────────
//  Each module registers its JNI binding (subscribe / set_text / execute
//  command) by module id. subscribe_all() and the Android shell look up this
//  table, so neither side keeps any module-specific if-else.

static const wb::jni::BindingTable& module_bindings() {
    static const wb::jni::BindingTable table = [] {
        wb::jni::BindingTable t;
        wb::calendar::register_calendar_binding(t);
        wb::dashboard::register_dashboard_binding(t);
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

static void subscribe_all(wb::core::AppCore& core) {
    const auto& table = module_bindings();
    aria::runtime::EventBus& bus = core.services().bus();
    for (auto& entry : core.modules()) {
        auto it = table.find(entry.id);
        if (it == table.end() || !it->second.subscribe) {
            continue;
        }
        it->second.subscribe(bus, *entry.vm, g_propertySubs);
    }
}

// ── Native entry points (mirrored by Kotlin JniBridge) ──────────────────────
extern "C" {

JNIEXPORT void JNICALL
Java_com_dqsjqian_ariatools_JniBridge_nativeCreateShell(JNIEnv* env, jclass,
                                                      jstring i18nDir) {
    if (g_shell) {
        // Disconnect callbacks while their source VMs still exist.
        g_propertySubs.clear();
        g_shell.reset();
    }
    g_mainScheduler.bind_main_thread();

    const char* dir = env->GetStringUTFChars(i18nDir, nullptr);
    std::string i18n(dir);
    env->ReleaseStringUTFChars(i18nDir, dir);

    g_shell = std::make_unique<wb::android::AndroidShell>(
        i18n, g_mainScheduler, g_mainScheduler);

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
    std::lock_guard lock(g_mainQueueMutex);
    g_mainQueue.clear();
}

JNIEXPORT void JNICALL
Java_com_dqsjqian_ariatools_JniBridge_nativeRunMainTasks(JNIEnv*, jclass) {
    std::vector<std::function<void()>> tasks;
    {
        std::lock_guard lock(g_mainQueueMutex);
        tasks.swap(g_mainQueue);
    }
    for (auto& task : tasks) task();
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
    g_postToMain = env->GetStaticMethodID(
        g_bridgeClass, "postToMain", "()V");
    if (!g_onModulesChanged || !g_onPropertyChanged || !g_postToMain) {
        __android_log_print(ANDROID_LOG_ERROR, kTag,
                            "JNI_OnLoad: bridge method(s) not found");
        return JNI_ERR;
    }
    wb::jni::push_property_fn() = &push_property_impl;
    return JNI_VERSION_1_6;
}
