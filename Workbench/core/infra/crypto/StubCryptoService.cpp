//
// StubCryptoService — Skeleton-stage encrypt/decrypt stub (no real encryption, returns input as-is).
// Temporary fallback for non-Apple platforms; replace with a real platform crypto implementation before release.
//
#include "infra/crypto/ICryptoService.h"

namespace wb::services {

class StubCryptoService final : public ICryptoService {
public:
    bool is_unlocked() const override { return unlocked_; }

    bool unlock(const std::string& passphrase) override {
        unlocked_ = !passphrase.empty();
        return unlocked_;
    }
    void lock() override { unlocked_ = false; }

    std::optional<std::string> encrypt(const std::string& plain) override {
        if (!unlocked_) return std::nullopt;
        return plain;  // Skeleton stage: placeholder, no real encryption
    }
    std::optional<std::string> decrypt(const std::string& cipher) override {
        if (!unlocked_) return std::nullopt;
        return cipher;
    }

private:
    bool unlocked_ = false;
};

ICryptoService* make_stub_crypto_service() {
    return new StubCryptoService();
}

}  // namespace wb::services
