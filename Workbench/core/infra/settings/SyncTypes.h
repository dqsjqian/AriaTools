#pragma once
//
// 同步 / 加密 相关的公共数据类型（纯数据，跨端共享）。
//
#include <string>
#include <cstdint>

namespace wb::services {

/// 加密范围（opt-in，用户在设置里选择）。
enum class EncryptScope {
    None,        ///< 不加密（默认）
    VaultOnly,   ///< 仅加密 vault/ 下的文件加解密产物
    NotesToo,    ///< 连笔记也加密（会牺牲 git 文本合并能力）
    All          ///< 全部数据加密
};

/// 同步设置 —— 全部为运行时用户配置，绝不写死。
/// 面向多用户分发：每个用户填自己的私有 Gitee 仓库。
struct SyncSettings {
    std::string dataDir;         ///< 数据根目录（默认 ~/WorkbenchData），整体是一个 git 仓库
    std::string remoteUrl;       ///< git 远程地址，例如 https://gitee.com/<user>/<repo>.git
    std::string branch = "main"; ///< 分支
    std::string username;        ///< git 用户名（提交署名 / 认证）
    // 注意：Token 不放这里明文持久化，走 ISecretStore 安全存储。

    bool   autoSync   = true;    ///< 全自动同步（定时 + 条件触发）
    int    autoSyncIntervalSec = 300;  ///< 定时同步间隔（秒）
    bool   useLfs     = true;    ///< 大文件走 Git LFS

    EncryptScope encryptScope = EncryptScope::None; ///< 加密范围（opt-in）
};

/// 同步结果。
struct SyncResult {
    bool        ok = false;
    std::string message;   ///< 面向用户的可读信息
};

}  // namespace wb::services
