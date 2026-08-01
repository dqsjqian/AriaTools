//
// StubSyncService — Skeleton-stage sync stub implementation.
// Does not actually operate on git: based on whether config is complete,
// returns "not configured" / "simulated success".
// Replace with a libgit2 implementation in the production phase.
//
#include "infra/sync/ISyncService.h"

namespace wb::services {

class StubSyncService final : public ISyncService {
public:
    SyncResult ensure_repo(const SyncSettings& s, const ProgressFn& log) override {
        if (log) log("[stub] ensure_repo: " + s.dataDir);
        if (s.remoteUrl.empty())
            return {false, "Remote repository URL not configured (set a private Gitee repo in settings)"};
        if (log) log("[stub] repository ready (simulated)");
        return {true, "Repository ready (simulated)"};
    }

    SyncResult push(const SyncSettings& s, const std::string& token,
                    const ProgressFn& log) override {
        if (auto r = precheck_(s, token); !r.ok) return r;
        if (log) log("[stub] add -> commit -> push (simulated)");
        return {true, "Push succeeded (simulated)"};
    }

    SyncResult pull(const SyncSettings& s, const std::string& token,
                    const ProgressFn& log) override {
        if (auto r = precheck_(s, token); !r.ok) return r;
        if (log) log("[stub] pull (simulated)");
        return {true, "Pull succeeded (simulated)"};
    }

    SyncResult sync(const SyncSettings& s, const std::string& token,
                    const ProgressFn& log) override {
        if (auto r = pull(s, token, log); !r.ok) return r;
        return push(s, token, log);
    }

private:
    static SyncResult precheck_(const SyncSettings& s, const std::string& token) {
        if (s.remoteUrl.empty())
            return {false, "Remote repository URL not configured"};
        if (token.empty())
            return {false, "Access token not configured (set it in settings)"};
        return {true, ""};
    }
};

ISyncService* make_stub_sync_service() {
    return new StubSyncService();
}

}  // namespace wb::services
