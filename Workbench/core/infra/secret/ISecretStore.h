#pragma once
//
// ISecretStore — Secure storage abstraction for sensitive data (git tokens).
// Skeleton stage: stub implementation (in-memory).
// Production stage: Mac=Keychain, Win=Credential Manager, iOS=Keychain, Android=Keystore.
//
#include <string>
#include <optional>

namespace wb::services {

class ISecretStore {
public:
    virtual ~ISecretStore() = default;

    /// Store/retrieve a secret named by key (e.g. "gitee.token").
    virtual bool store(const std::string& key, const std::string& secret) = 0;
    [[nodiscard]] virtual std::optional<std::string> retrieve(const std::string& key) = 0;
    virtual bool erase(const std::string& key) = 0;
};

}  // namespace wb::services
