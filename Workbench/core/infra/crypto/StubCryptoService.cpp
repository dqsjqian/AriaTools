//
// StubCryptoService — 骨架期加解密桩（不做真实加密，原样返回）。
// 非 Apple 平台的临时回退实现；发布前应替换为该平台的真实加密实现。
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
        return plain;  // 骨架期：占位，不真正加密
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
