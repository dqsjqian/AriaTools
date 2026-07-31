#include "module_api/ModuleRegistry.h"

#include <algorithm>

namespace wb::module_api {

std::vector<std::shared_ptr<IModule>> ModuleRegistry::ordered() const {
    auto v = modules_;
    std::stable_sort(v.begin(), v.end(),
                     [](const auto& a, const auto& b) { return a->order() < b->order(); });
    return v;
}

}  // namespace wb::module_api
