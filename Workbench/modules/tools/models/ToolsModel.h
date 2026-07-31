#pragma once

#include "services/ToolsService.h"

#include <memory>
#include <string>

namespace wb::tools {

// ToolsModel — tools 模块共享的强类型业务入口。
// VM 只维护页面状态；工具算法和数据访问统一经本 Model 调用。
class ToolsModel {
public:
    explicit ToolsModel(std::shared_ptr<ToolsService> service)
        : service_(std::move(service)) {}

    [[nodiscard]] std::string encode_base64(const std::string& input) const;
    [[nodiscard]] std::string decode_base64(const std::string& input) const;
    [[nodiscard]] std::string generate_random(int length) const;

    [[nodiscard]] ToolResult format_json(const std::string& input) const;
    [[nodiscard]] ToolResult minify_json(const std::string& input) const;
    [[nodiscard]] ToolResult encrypt_file(const std::string& inputPath,
                                          const std::string& passphrase);
    [[nodiscard]] ToolResult decrypt_file(const std::string& inputPath,
                                          const std::string& passphrase);

private:
    std::shared_ptr<ToolsService> service_;
};

}  // namespace wb::tools
