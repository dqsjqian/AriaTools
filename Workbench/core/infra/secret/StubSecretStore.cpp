//
// StubSecretStore — Skeleton-stage token secure storage stub (in-memory map).
// Replace with platform secure storage in the production phase (Keychain / Credential Manager / Keystore).
//
#include "infra/secret/ISecretStore.h"

#include <unordered_map>

namespace wb::services {

class StubSecretStore final : public ISecretStore {
public:
    bool store(const std::string& key, const std::string& secret) override {
        map_[key] = secret;
        return true;
    }
    std::optional<std::string> retrieve(const std::string& key) override {
        auto it = map_.find(key);
        if (it == map_.end()) return std::nullopt;
        return it->second;
    }
    bool erase(const std::string& key) override {
        return map_.erase(key) > 0;
    }

private:
    std::unordered_map<std::string, std::string> map_;
};

ISecretStore* make_stub_secret_store() {
    return new StubSecretStore();
}

}  // namespace wb::services
