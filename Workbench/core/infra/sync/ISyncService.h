#pragma once
//
// ISyncService — 把数据目录作为 git 仓库与远端同步。
// 骨架期：桩实现（返回"未配置/模拟成功"）。
// 业务阶段：用 libgit2 实现 init/add/commit/pull/push + 冲突副本策略 + LFS。
//
#include "infra/settings/SyncTypes.h"
#include <functional>
#include <string>

namespace wb::services {

class ISyncService {
public:
    virtual ~ISyncService() = default;

    /// 进度 / 日志回调（可用于 UI 展示）。
    using ProgressFn = std::function<void(const std::string& line)>;

    /// 确保 dataDir 是一个 git 仓库（不存在则 init + 关联 remote）。
    virtual SyncResult ensure_repo(const SyncSettings& s, const ProgressFn& log) = 0;

    /// 推送：add -> commit -> push。
    virtual SyncResult push(const SyncSettings& s, const std::string& token,
                            const ProgressFn& log) = 0;

    /// 拉取：pull（冲突用副本策略，不抛 git 冲突标记给用户）。
    virtual SyncResult pull(const SyncSettings& s, const std::string& token,
                            const ProgressFn& log) = 0;

    /// 一次完整同步（pull 再 push），供"同步"按钮 / 定时任务调用。
    virtual SyncResult sync(const SyncSettings& s, const std::string& token,
                            const ProgressFn& log) = 0;
};

}  // namespace wb::services
