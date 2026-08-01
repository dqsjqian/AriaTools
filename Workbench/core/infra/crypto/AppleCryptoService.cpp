//
// AppleCryptoService — Native Apple platform crypto implementation (macOS/iOS).
//
// Algorithm: PBKDF2-HMAC-SHA256 derives 64B (32B AES + 32B MAC);
// AES-256-CBC(PKCS7) encryption followed by HMAC-SHA256 (Encrypt-then-MAC).
// Format: "WBCR2" + salt(16) + iv(16) + ciphertext + mac(32).
// Depends only on system CommonCrypto + Security, no third-party dynamic
// libraries, suitable for App distribution.
//
#include "infra/crypto/ICryptoService.h"

#include <CommonCrypto/CommonCryptor.h>
#include <CommonCrypto/CommonHMAC.h>
#include <CommonCrypto/CommonKeyDerivation.h>
#include <Security/Security.h>

#include <array>
#include <cstring>
#include <string>
#include <vector>

namespace wb::services {

namespace {
constexpr char kMagic[] = "WBCR2";
constexpr std::size_t kMagicSize = sizeof(kMagic) - 1;
constexpr std::size_t kSaltSize = 16;
constexpr std::size_t kIvSize = kCCBlockSizeAES128;
constexpr std::size_t kMacSize = CC_SHA256_DIGEST_LENGTH;
constexpr unsigned kRounds = 200000;

bool random_bytes(unsigned char* data, std::size_t size) {
    return SecRandomCopyBytes(kSecRandomDefault, size, data) == errSecSuccess;
}

bool derive_keys(const std::string& passphrase, const unsigned char* salt,
                 std::array<unsigned char, 64>& keys) {
    return CCKeyDerivationPBKDF(kCCPBKDF2,
                                passphrase.data(), passphrase.size(),
                                salt, kSaltSize,
                                kCCPRFHmacAlgSHA256,
                                kRounds,
                                keys.data(), keys.size()) == kCCSuccess;
}

void secure_clear(std::string& value) {
    if (!value.empty()) std::memset(value.data(), 0, value.size());
    value.clear();
}
}  // namespace

class AppleCryptoService final : public ICryptoService {
public:
    ~AppleCryptoService() override { lock(); }

    bool is_unlocked() const override { return !passphrase_.empty(); }

    bool unlock(const std::string& passphrase) override {
        lock();
        if (passphrase.empty()) return false;
        passphrase_ = passphrase;
        return true;
    }

    void lock() override { secure_clear(passphrase_); }

    std::optional<std::string> encrypt(const std::string& plain) override {
        if (!is_unlocked()) return std::nullopt;
        std::array<unsigned char, kSaltSize> salt{};
        std::array<unsigned char, kIvSize> iv{};
        std::array<unsigned char, 64> keys{};
        if (!random_bytes(salt.data(), salt.size()) ||
            !random_bytes(iv.data(), iv.size()) ||
            !derive_keys(passphrase_, salt.data(), keys)) {
            return std::nullopt;
        }

        std::vector<unsigned char> cipher(plain.size() + kCCBlockSizeAES128);
        std::size_t cipherSize = 0;
        const CCCryptorStatus rc = CCCrypt(
            kCCEncrypt, kCCAlgorithmAES, kCCOptionPKCS7Padding,
            keys.data(), 32, iv.data(), plain.data(), plain.size(),
            cipher.data(), cipher.size(), &cipherSize);
        if (rc != kCCSuccess) return std::nullopt;
        cipher.resize(cipherSize);

        std::string out(kMagicSize + salt.size() + iv.size() + cipher.size() + kMacSize, '\0');
        std::size_t off = 0;
        std::memcpy(out.data() + off, kMagic, kMagicSize); off += kMagicSize;
        std::memcpy(out.data() + off, salt.data(), salt.size()); off += salt.size();
        std::memcpy(out.data() + off, iv.data(), iv.size()); off += iv.size();
        std::memcpy(out.data() + off, cipher.data(), cipher.size()); off += cipher.size();
        CCHmac(kCCHmacAlgSHA256, keys.data() + 32, 32,
               out.data(), off, out.data() + off);
        std::memset(keys.data(), 0, keys.size());
        return out;
    }

    std::optional<std::string> decrypt(const std::string& input) override {
        if (!is_unlocked()) return std::nullopt;
        const std::size_t header = kMagicSize + kSaltSize + kIvSize;
        if (input.size() < header + kMacSize ||
            std::memcmp(input.data(), kMagic, kMagicSize) != 0) return std::nullopt;

        const auto* salt = reinterpret_cast<const unsigned char*>(input.data() + kMagicSize);
        const auto* iv = salt + kSaltSize;
        const std::size_t cipherSize = input.size() - header - kMacSize;
        const auto* cipher = iv + kIvSize;
        const auto* storedMac = cipher + cipherSize;

        std::array<unsigned char, 64> keys{};
        std::array<unsigned char, kMacSize> computedMac{};
        if (!derive_keys(passphrase_, salt, keys)) return std::nullopt;
        CCHmac(kCCHmacAlgSHA256, keys.data() + 32, 32,
               input.data(), input.size() - kMacSize, computedMac.data());
        if (!constant_time_equal_(storedMac, computedMac.data(), kMacSize)) {
            std::memset(keys.data(), 0, keys.size());
            return std::nullopt;
        }

        std::string plain(cipherSize + kCCBlockSizeAES128, '\0');
        std::size_t plainSize = 0;
        const CCCryptorStatus rc = CCCrypt(
            kCCDecrypt, kCCAlgorithmAES, kCCOptionPKCS7Padding,
            keys.data(), 32, iv, cipher, cipherSize,
            plain.data(), plain.size(), &plainSize);
        std::memset(keys.data(), 0, keys.size());
        if (rc != kCCSuccess) return std::nullopt;
        plain.resize(plainSize);
        return plain;
    }

private:
    static bool constant_time_equal_(const unsigned char* a,
                                     const unsigned char* b,
                                     std::size_t size) {
        unsigned char diff = 0;
        for (std::size_t i = 0; i < size; ++i) diff |= a[i] ^ b[i];
        return diff == 0;
    }

    std::string passphrase_;
};

ICryptoService* make_apple_crypto_service() {
    return new AppleCryptoService();
}

}  // namespace wb::services
