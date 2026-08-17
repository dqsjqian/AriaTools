#include "app/ModulesManifest.h"

// ModulesManifest.cpp — auto-generated registration.
//
// The CMake configure step scans modules/ for subdirectories and generates
// GeneratedModuleList.h with one WB_MODULE_ENTRY(mod, Mod) per discovered
// module. This file expands each entry into a forward-declaration of
// make_<mod>_module() + a registry.add() call.
//
// To add a module: create modules/<name>/CMakeLists.txt using wb_add_module.
// To remove a module: delete its directory. No changes to this file or any
// core file are needed — CMake re-scans on the next configure.

// ── Phase 1: forward-declare each module factory (global scope) ────────
#define WB_MODULE_ENTRY(mod, Mod) \
    namespace wb::mod { \
        std::shared_ptr<wb::module_api::IModule> make_##mod##_module(); \
    }
#include "app/GeneratedModuleList.h"
#undef WB_MODULE_ENTRY

namespace wb::app {

void populate_modules(wb::module_api::ModuleRegistry& registry) {
// ── Phase 2: register each module ───────────────────────────────────────
#define WB_MODULE_ENTRY(mod, Mod) \
    registry.add(wb::mod::make_##mod##_module());
#include "app/GeneratedModuleList.h"
#undef WB_MODULE_ENTRY
}

}  // namespace wb::app
