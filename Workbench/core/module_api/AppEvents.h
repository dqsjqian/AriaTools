#pragma once
//
// AppEvents — Event types for cross-module communication (published/subscribed via aria EventBus).
// Modules never depend on each other directly; they depend only on these event contracts.
//
#include <string>

namespace wb::events {

/// A note was saved (notes module publishes -> sync module may auto-sync accordingly).
struct NoteSaved {
    std::string id;
};

/// Request a data sync (any module may publish -> sync module executes it).
struct SyncRequested {
    std::string reason;
};

/// Language has changed (settings module publishes -> interested modules/shells may react).
struct LanguageChanged {
    std::string lang;
};

}  // namespace wb::events
