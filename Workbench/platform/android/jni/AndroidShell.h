#pragma once
//
// AndroidShell — Android-side shell. Owns the pure-C++ wb::core::AppCore and
// exposes module metadata + activation + bidirectional property/command
// bridge to the JNI layer.
//
// The view layer is native Kotlin/Compose talking over the JNI side-channel:
//   C++ → Kotlin : Property::on_changed → JNI onPropertyChanged → StateFlow
//   Kotlin → C++ : nativeSetProperty / nativeExecuteCommand → this
//
#include "app/AppCore.h"

#include <memory>
#include <string>
#include <vector>

namespace wb::android {

class AndroidShell {
public:
    struct ModuleInfo {
        std::string id;
        std::string navKey;
        std::string title;   // resolved navigation title (current language)
    };

    explicit AndroidShell(std::string i18nBaseDir, std::string initialLang = "zh-CN");
    ~AndroidShell();

    AndroidShell(const AndroidShell&) = delete;
    AndroidShell& operator=(const AndroidShell&) = delete;

    wb::core::AppCore& core() { return core_; }

    /// Module metadata (id + current-language title), built once from AppCore.
    [[nodiscard]] const std::vector<ModuleInfo>& modules() const { return modules_; }

    /// Activate the VM of the given module id (deactivating the previous one).
    void activate_module(const std::string& id);

    /// Kotlin→C++: set a string Property on the given module's VM.
    /// Routes by (moduleId, propName) to the underlying Property<string>::set.
    void set_text(const std::string& moduleId, const std::string& propName,
                  const std::string& value);

    /// Kotlin→C++: execute a parameterless Command on the given module's VM.
    /// Routes by (moduleId, cmdName) to the underlying Command<>::execute.
    void execute_command(const std::string& moduleId, const std::string& cmdName);

private:
    wb::core::AppCore              core_;
    std::vector<ModuleInfo>        modules_;
    std::shared_ptr<aria::binding::ViewModel> active_;
};

}  // namespace wb::android
