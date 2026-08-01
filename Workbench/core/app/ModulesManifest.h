#pragma once
//
// ModulesManifest — Explicit list of module factories to assemble into the app.
//
// Why explicit (not global self-registration): modules are independent static
// libraries; global constructor self-registration gets stripped by the linker,
// requiring -force_load/whole-archive and is fragile across platforms. An
// explicit manifest is more reliable, and "add a module = add one line here";
// each module remains self-contained and independently buildable.
//
#include "module_api/ModuleRegistry.h"

namespace wb::app {

/// Register all enabled modules into the registry (used after sorting by each module's order).
void populate_modules(wb::module_api::ModuleRegistry& registry);

}  // namespace wb::app
