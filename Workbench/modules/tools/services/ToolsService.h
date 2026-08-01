#pragma once
//
// ToolsService — tools module private business service.
// Handles JSON conversion and file encryption/decryption flows; the VM only maintains UI state and calls this service.
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

    /// Encrypts to <dataDir>/vault/<original file name>.wbenc.
    [[nodiscard]] ToolResult encrypt_file(const std::string& inputPath,
                                          const std::string& passphrase);
    /// Decrypts to <dataDir>/vault/<original file name>.decrypted.
    [[nodiscard]] ToolResult decrypt_file(const std::string& inputPath,
                                          const std::string& passphrase);

private:
    wb::services::ICryptoService& crypto_;
    wb::services::IStorageService& storage_;
};

}  // namespace wb::tools
