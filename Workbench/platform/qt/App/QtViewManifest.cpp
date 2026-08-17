#include "App/QtViewManifest.h"

// QtViewManifest.cpp — auto-generated View registration.
//
// GeneratedModuleList.h (produced by CMake's auto-discovery scan) contains
// one WB_MODULE_ENTRY(mod, Mod) line per discovered module. We expand each
// into a forward-declaration of register_<mod>_view() + a call.
//
// To add/remove a module's Qt View, just create/delete its platforms/qt/
// sources — no changes to this file.

// ── Phase 1: forward-declare each View register function (global scope) ──
#define WB_MODULE_ENTRY(mod, Mod) \
    namespace wb::mod { void register_##mod##_view(); }
#include "app/GeneratedModuleList.h"
#undef WB_MODULE_ENTRY

namespace wb::qt {

void register_all_views() {
// ── Phase 2: call each register function ────────────────────────────────
#define WB_MODULE_ENTRY(mod, Mod) \
    wb::mod::register_##mod##_view();
#include "app/GeneratedModuleList.h"
#undef WB_MODULE_ENTRY
}

}  // namespace wb::qt
