#pragma once
//
// ISyncService — Sync the data directory as a git repository with a remote.
// Skeleton stage: stub implementation (returns "not configured / simulated success").
// Production stage: implement with libgit2 for init/add/commit/pull/push + conflict-copy strategy + LFS.
//
#include "infra/settings/SyncTypes.h"
#include <functional>
#include <string>

namespace wb::services {

class ISyncService {
public:
    virtual ~ISyncService() = default;

    /// Progress / log callback (may be used for UI display).
    using ProgressFn = std::function<void(const std::string& line)>;

    /// Ensure dataDir is a git repository (init + link remote if absent).
    virtual SyncResult ensure_repo(const SyncSettings& s, const ProgressFn& log) = 0;

    /// Push: add -> commit -> push.
    virtual SyncResult push(const SyncSettings& s, const std::string& token,
                            const ProgressFn& log) = 0;

    /// Pull: pull (conflicts handled via copy strategy; no git conflict markers exposed to the user).
    virtual SyncResult pull(const SyncSettings& s, const std::string& token,
                            const ProgressFn& log) = 0;

    /// Full sync (pull then push), for the "sync" button / scheduled tasks.
    virtual SyncResult sync(const SyncSettings& s, const std::string& token,
                            const ProgressFn& log) = 0;
};

}  // namespace wb::services
