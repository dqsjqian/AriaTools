#pragma once
//
// ICryptoService — Data encryption/decryption (opt-in).
// Skeleton stage: stub implementation (returns input as-is / simple placeholder).
// Apple platform implementation: CommonCrypto (PBKDF2 + AES-256-CBC + HMAC-SHA256);
// other platforms may plug in system crypto libraries or vendored crypto libraries via the same interface.
//
#include <string>
#include <optional>

namespace wb::services {

class ICryptoService {
public:
    virtual ~ICryptoService() = default;

    /// Whether unlocked (user has entered passphrase / derived key).
    [[nodiscard]] virtual bool is_unlocked() const = 0;

    /// Unlock with a passphrase (derives the key).
    virtual bool unlock(const std::string& passphrase) = 0;
    virtual void lock() = 0;

    /// Encrypt / decrypt binary data (bytes carried as string).
    [[nodiscard]] virtual std::optional<std::string> encrypt(const std::string& plain) = 0;
    [[nodiscard]] virtual std::optional<std::string> decrypt(const std::string& cipher) = 0;
};

}  // namespace wb::services
