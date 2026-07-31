#pragma once
//
// ICryptoService — 数据加解密（opt-in）。
// 骨架期：桩实现（原样返回 / 简单占位）。
// Apple 平台实现：CommonCrypto（PBKDF2 + AES-256-CBC + HMAC-SHA256）；
// 其他平台可按同一接口接入系统密码库或 vendored 加密库。
//
#include <string>
#include <optional>

namespace wb::services {

class ICryptoService {
public:
    virtual ~ICryptoService() = default;

    /// 是否已解锁（用户已输入口令 / 派生出密钥）。
    [[nodiscard]] virtual bool is_unlocked() const = 0;

    /// 用口令解锁（派生密钥）。
    virtual bool unlock(const std::string& passphrase) = 0;
    virtual void lock() = 0;

    /// 加密 / 解密二进制数据（以字符串承载字节）。
    [[nodiscard]] virtual std::optional<std::string> encrypt(const std::string& plain) = 0;
    [[nodiscard]] virtual std::optional<std::string> decrypt(const std::string& cipher) = 0;
};

}  // namespace wb::services
