#pragma once
//
// AndroidShell — Android-side shell. Owns the pure-C++ wb::core::AppCore and
// exposes module metadata + activation to the JNI bridge. The view layer is
// native Kotlin/Compose talking over the JNI side-channel (Property → on_changed
// → JNI → StateFlow → recomposition), symmetric with the Qt/iOS shells.
//
// No BindingEngine here: Compose has no traditional View objects to bind, so
// the bridge pushes property values instead (same pattern as Aria demo5).
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

private:
    wb::core::AppCore              core_;
    std::vector<ModuleInfo>        modules_;
    std::shared_ptr<aria::binding::ViewModel> active_;
};

}  // namespace wb::android
