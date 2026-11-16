#pragma once

// Workbench's manifest convenience facade. Aria owns identity validation,
// ordering and registration lifetime; this facade retains the manifest tokens.
#include "module_api/IModule.h"
#include "aria/app/module_registry.hpp"

namespace wb::module_api {

class ModuleRegistry {
public:
    void add(std::shared_ptr<IModule> module) {
        if (!module) throw std::invalid_argument("null Workbench module");
        aria::app::ModuleDescriptor descriptor{module->id(), module->order()};
        registrations_ += modules_.add(std::move(descriptor), std::move(module));
    }
    [[nodiscard]] auto ordered() const { return modules_.ordered(); }
    [[nodiscard]] auto all() const { return modules_.all(); }
    [[nodiscard]] auto find(const std::string& id) const { return modules_.find(id); }
    void clear() { registrations_.clear(); modules_.clear(); }

private:
    aria::app::ModuleRegistry<IModule> modules_;
    aria::SubscriptionBag registrations_;
};

}  // namespace wb::module_api
