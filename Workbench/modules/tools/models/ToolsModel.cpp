#include "models/ToolsModel.h"

#include "utils/Base64.h"

#include <random>

namespace wb::tools {

std::string ToolsModel::encode_base64(const std::string& input) const {
    return wb::utils::base64_encode(input);
}

std::string ToolsModel::decode_base64(const std::string& input) const {
    return wb::utils::base64_decode(input);
}

std::string ToolsModel::generate_random(int length) const {
    static constexpr char kAlnum[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    if (length <= 0) length = 16;

    std::random_device device;
    std::mt19937 generator(device());
    std::uniform_int_distribution<> distribution(
        0, static_cast<int>(sizeof(kAlnum) - 2));

    std::string result;
    result.reserve(static_cast<std::size_t>(length));
    for (int i = 0; i < length; ++i) {
        result.push_back(kAlnum[distribution(generator)]);
    }
    return result;
}

ToolResult ToolsModel::format_json(const std::string& input) const {
    return service_->format_json(input);
}

ToolResult ToolsModel::minify_json(const std::string& input) const {
    return service_->minify_json(input);
}

ToolResult ToolsModel::encrypt_file(const std::string& inputPath,
                                    const std::string& passphrase) {
    return service_->encrypt_file(inputPath, passphrase);
}

ToolResult ToolsModel::decrypt_file(const std::string& inputPath,
                                    const std::string& passphrase) {
    return service_->decrypt_file(inputPath, passphrase);
}

}  // namespace wb::tools
