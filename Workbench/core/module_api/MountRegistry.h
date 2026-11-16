#pragma once

// Workbench compatibility surface: JSON payloads and moduleId naming are app
// policy. Provider storage, lifetime, toggles and typed resolution live in Aria.
#include "aria/app/mount_registry.hpp"
#include <nlohmann/json.hpp>

namespace wb::module_api {
class ModuleContext;

struct Mounted {
    std::string moduleId;
    std::shared_ptr<aria::binding::ViewModel> vm;
};

class IMountArgs {
public:
    virtual ~IMountArgs() = default;
    virtual bool on_mount(const nlohmann::json& args) = 0;
};

class MountRegistry {
public:
    explicit MountRegistry(ModuleContext& context) : mounts_(context) {}
    using Factory = aria::app::MountRegistry<ModuleContext>::Factory;

    // The facade owns registrations for the assembled application's lifetime.
    // Duplicate/invalid contributions are rejected without replacing a provider.
    bool Provide(std::string slot, std::string moduleId, Factory factory) {
        try {
            registrations_ += mounts_.provide(std::move(slot), std::move(moduleId), std::move(factory));
            return true;
        } catch (const std::invalid_argument&) {
            return false;
        }
    }
    [[nodiscard]] std::optional<Mounted> Resolve(
        const std::string& slot, const nlohmann::json& args = nlohmann::json::object()) {
        auto result = mounts_.resolve(slot);
        if (!result) return std::nullopt;
        // JSON is optional for legacy Workbench providers. Typed requests below
        // require their declared interface and fail if it is not implemented.
        if (auto* target = dynamic_cast<IMountArgs*>(result->vm.get());
            target && !target->on_mount(args)) return std::nullopt;
        return Mounted{std::move(result->provider_id), std::move(result->vm)};
    }
    template<class Interface, class Payload>
    [[nodiscard]] std::optional<Mounted> Resolve(const std::string& slot, const Payload& payload) {
        auto result = mounts_.template resolve<Interface>(slot, payload);
        if (!result) return std::nullopt;
        return Mounted{std::move(result->provider_id), std::move(result->vm)};
    }
    [[nodiscard]] auto module_of(const std::string& slot) const { return mounts_.module_of(slot); }
    [[nodiscard]] bool provided(const std::string& slot) const { return mounts_.provided(slot); }
    bool SetEnabled(const std::string& slot, bool enabled) { return mounts_.set_enabled(slot, enabled); }
    bool Unprovide(const std::string& slot) { return mounts_.remove(slot); }
    [[nodiscard]] aria::Subscription on_changed(std::function<void()> callback) {
        return mounts_.on_changed(std::move(callback));
    }
    void clear() { registrations_.clear(); mounts_.clear(); }

private:
    aria::app::MountRegistry<ModuleContext> mounts_;
    aria::SubscriptionBag registrations_;
};
}  // namespace wb::module_api
