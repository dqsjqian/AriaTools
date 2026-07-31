#pragma once
//
// ISecretStore — 敏感信息（git Token）安全存储抽象。
// 骨架期：桩实现（内存）。
// 业务阶段：Mac=Keychain, Win=Credential Manager, iOS=Keychain, Android=Keystore。
//
#include <string>
#include <optional>

namespace wb::services {

class ISecretStore {
public:
    virtual ~ISecretStore() = default;

    /// 存取以 key 命名的密文（如 "gitee.token"）。
    virtual bool store(const std::string& key, const std::string& secret) = 0;
    [[nodiscard]] virtual std::optional<std::string> retrieve(const std::string& key) = 0;
    virtual bool erase(const std::string& key) = 0;
};

}  // namespace wb::services
