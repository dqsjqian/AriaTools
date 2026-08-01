#pragma once
//
// ModuleRegistry — Runtime registry of assembled modules (filled by the app layer via an explicit manifest).
// Does not rely on global constructor auto-registration (static libraries get stripped by the linker);
// instead ModulesManifest calls add() explicitly.
//
#include "module_api/IModule.h"

#include <memory>
#include <vector>

namespace wb::module_api {

class ModuleRegistry {
public:
    /// Add a module instance (called by the manifest).
    void add(std::shared_ptr<IModule> m) { modules_.push_back(std::move(m)); }

    /// Return all modules sorted by order ascending.
    [[nodiscard]] std::vector<std::shared_ptr<IModule>> ordered() const;

    [[nodiscard]] const std::vector<std::shared_ptr<IModule>>& all() const {
        return modules_;
    }

private:
    std::vector<std::shared_ptr<IModule>> modules_;
};

}  // namespace wb::module_api
