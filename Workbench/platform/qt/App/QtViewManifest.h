#pragma once
// Qt View registration manifest: explicitly calls each module's register_<mod>_view().
// Adding a module = add one line here (symmetric with core's ModulesManifest).
namespace wb::qt { void register_all_views(); }
