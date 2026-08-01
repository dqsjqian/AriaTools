#pragma once
//
// Common data types for sync / encryption (pure data, cross-platform shared).
//
#include <string>
#include <cstdint>

namespace wb::services {

/// Encryption scope (opt-in; user chooses in settings).
enum class EncryptScope {
    None,        ///< No encryption (default)
    VaultOnly,   ///< Encrypt only files under vault/
    NotesToo,    ///< Also encrypt notes (sacrifices git text merge capability)
    All          ///< Encrypt all data
};

/// Sync settings — all are runtime user config, never hardcoded.
/// For multi-user distribution: each user fills in their own private Gitee repository.
struct SyncSettings {
    std::string dataDir;         ///< Data root directory (default ~/WorkbenchData); the whole directory is one git repository
    std::string remoteUrl;       ///< git remote URL, e.g. https://gitee.com/<user>/<repo>.git
    std::string branch = "main"; ///< Branch
    std::string username;        ///< git username (commit author / authentication)
    // Note: Token is not persisted in plaintext here; goes through ISecretStore for secure storage.

    bool   autoSync   = true;    ///< Full auto-sync (scheduled + condition-triggered)
    int    autoSyncIntervalSec = 300;  ///< Scheduled sync interval (seconds)
    bool   useLfs     = true;    ///< Large files go through Git LFS

    EncryptScope encryptScope = EncryptScope::None; ///< Encryption scope (opt-in)
};

/// Sync result.
struct SyncResult {
    bool        ok = false;
    std::string message;   ///< Human-readable message for the user
};

}  // namespace wb::services
