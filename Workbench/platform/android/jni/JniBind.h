// ────────────────────────────────────────────────────────────────────────────
//  JniBind.h — Shared JNI side-channel binding helpers for Workbench module
//  JniBinding translation units.
//
//  Each module's platforms/android/<Mod>JniBinding.{h,cpp} implements the
//  subscribe_<mod>() / set_<mod>_text() / exec_<mod>_command() entry points
//  for its VM. The common wire-format helpers (push_property + bind_*
//  projection templates) and the registry table types live here so the module
//  files stay self-contained.
//
//  The actual JNI globals (g_jvm / g_bridgeClass / g_onPropertyChanged) are
//  owned by jni_bridge.cpp. It installs the push_property callback right
//  after JNI_OnLoad, so module bindings can push values to Kotlin without
//  knowing the Java side or the thread-attachment details.
// ────────────────────────────────────────────────────────────────────────────
#pragma once

#include <jni.h>

#include <cstdio>
#include <string>
#include <unordered_map>
#include <vector>

#include "aria/binding/view_model.hpp"
#include "aria/runtime/event_bus.hpp"
#include "aria/subscription.hpp"

namespace wb::jni {

/// Push a property change to Kotlin (JniBridge.onPropertyChanged).
/// Installed by jni_bridge.cpp from JNI_OnLoad.
using PushPropertyFn = void (*)(const std::string& moduleId,
                                const std::string& name,
                                const std::string& value);

inline PushPropertyFn& push_property_fn() {
    static PushPropertyFn fn = nullptr;
    return fn;
}

inline void push_property(const std::string& moduleId, const std::string& name,
                          const std::string& value) {
    if (push_property_fn()) {
        push_property_fn()(moduleId, name, value);
    }
}

/// Subscribe any read-only reactive source and project it to the string wire
/// format used by the Compose side-channel. Capture route names by value:
/// bind_* receives temporary/local strings whose references do not outlive
/// the subscription callback.
template<class Source, class Project>
inline void bind_projected(std::vector<aria::Subscription>& subs,
                           const std::string& module, const std::string& name,
                           Source& source, Project project) {
    subs.push_back(source.on_changed(
        [module, name, project](const auto& value) {
            push_property(module, name, project(value));
        }));
    push_property(module, name, project(source.get()));
}

template<class Source>
inline void bind_str(std::vector<aria::Subscription>& subs,
                     const std::string& module, const std::string& name,
                     Source& source) {
    bind_projected(subs, module, name, source,
                   [](const std::string& value) { return value; });
}

template<class Source>
inline void bind_dbl(std::vector<aria::Subscription>& subs,
                     const std::string& module, const std::string& name,
                     Source& source) {
    bind_projected(subs, module, name, source, [](double value) {
        char buf[32];
        snprintf(buf, sizeof buf, "%.3f", value);
        return std::string(buf);
    });
}

template<class Source>
inline void bind_int(std::vector<aria::Subscription>& subs,
                     const std::string& module, const std::string& name,
                     Source& source) {
    bind_projected(subs, module, name, source,
                   [](int value) { return std::to_string(value); });
}

template<class Source>
inline void bind_bool(std::vector<aria::Subscription>& subs,
                      const std::string& module, const std::string& name,
                      Source& source) {
    bind_projected(subs, module, name, source,
                   [](bool value) { return value ? std::string{"1"} : std::string{"0"}; });
}

// ── Registry table types ────────────────────────────────────────────────────
// One entry per module, keyed by module id. The JNI bridge (subscribe_all)
// and the Android shell (set_text / execute_command) dispatch through these
// function pointers, so neither file needs to know module VM types.

struct ModuleBinding {
    using SubscribeFn = void (*)(aria::runtime::EventBus&,
                                 aria::binding::ViewModel&,
                                 std::vector<aria::Subscription>&);
    using SetTextFn = void (*)(aria::binding::ViewModel&,
                               const std::string&, const std::string&);
    using ExecCmdFn = void (*)(aria::binding::ViewModel&,
                               const std::string&);

    SubscribeFn subscribe    = nullptr;
    SetTextFn   set_text     = nullptr;
    ExecCmdFn   exec_command = nullptr;
};

using BindingTable = std::unordered_map<std::string, ModuleBinding>;

}  // namespace wb::jni
