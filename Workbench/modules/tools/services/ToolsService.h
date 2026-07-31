#pragma once
//
// ToolsService — tools 模块私有业务服务。
// 负责 JSON 转换与文件加解密流程；VM 只维护 UI 状态并调用本服务。
//
#include "infra/crypto/ICryptoService.h"
#include "infra/storage/IStorageService.h"

#include <string>

namespace wb::tools {

struct ToolResult {
    bool ok = false;
    std::string value;
    std::string error;
};

class ToolsService {
public:
    ToolsService(wb::services::ICryptoService& crypto,
                 wb::services::IStorageService& storage)
        : crypto_(crypto), storage_(storage) {}

    [[nodiscard]] ToolResult format_json(const std::string& input) const;
    [[nodiscard]] ToolResult minify_json(const std::string& input) const;

    /// 加密到 <dataDir>/vault/<原文件名>.wbenc。
    [[nodiscard]] ToolResult encrypt_file(const std::string& inputPath,
                                          const std::string& passphrase);
    /// 解密到 <dataDir>/vault/<原文件名>.decrypted。
    [[nodiscard]] ToolResult decrypt_file(const std::string& inputPath,
                                          const std::string& passphrase);

private:
    wb::services::ICryptoService& crypto_;
    wb::services::IStorageService& storage_;
};

}  // namespace wb::tools
