//
// StubSyncService — 骨架期同步桩实现。
// 不真正操作 git：根据配置是否完整，返回"未配置" / "模拟成功"。
// 业务阶段替换为 libgit2 实现。
//
#include "infra/sync/ISyncService.h"

namespace wb::services {

class StubSyncService final : public ISyncService {
public:
    SyncResult ensure_repo(const SyncSettings& s, const ProgressFn& log) override {
        if (log) log("[stub] ensure_repo: " + s.dataDir);
        if (s.remoteUrl.empty())
            return {false, "尚未配置远程仓库地址（请在设置中填写 Gitee 私有仓库）"};
        if (log) log("[stub] 仓库已就绪（模拟）");
        return {true, "仓库已就绪（模拟）"};
    }

    SyncResult push(const SyncSettings& s, const std::string& token,
                    const ProgressFn& log) override {
        if (auto r = precheck_(s, token); !r.ok) return r;
        if (log) log("[stub] add -> commit -> push（模拟）");
        return {true, "推送成功（模拟）"};
    }

    SyncResult pull(const SyncSettings& s, const std::string& token,
                    const ProgressFn& log) override {
        if (auto r = precheck_(s, token); !r.ok) return r;
        if (log) log("[stub] pull（模拟）");
        return {true, "拉取成功（模拟）"};
    }

    SyncResult sync(const SyncSettings& s, const std::string& token,
                    const ProgressFn& log) override {
        if (auto r = pull(s, token, log); !r.ok) return r;
        return push(s, token, log);
    }

private:
    static SyncResult precheck_(const SyncSettings& s, const std::string& token) {
        if (s.remoteUrl.empty())
            return {false, "尚未配置远程仓库地址"};
        if (token.empty())
            return {false, "尚未配置访问 Token（请在设置中填写）"};
        return {true, ""};
    }
};

ISyncService* make_stub_sync_service() {
    return new StubSyncService();
}

}  // namespace wb::services
