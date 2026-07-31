#include "services/ToolsService.h"

#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

namespace wb::tools {

namespace {
ToolResult json_transform(const std::string& input, int indent) {
    try {
        const auto json = nlohmann::json::parse(input);
        return {true, json.dump(indent), {}};
    } catch (const nlohmann::json::parse_error& e) {
        return {false, {}, e.what()};
    } catch (const std::exception& e) {
        return {false, {}, e.what()};
    }
}

ToolResult read_file(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) return {false, {}, "cannot open input file"};
    std::ostringstream ss;
    ss << in.rdbuf();
    if (!in.good() && !in.eof()) return {false, {}, "failed to read input file"};
    return {true, ss.str(), {}};
}

bool write_file(const fs::path& path, const std::string& data) {
    std::error_code ec;
    fs::create_directories(path.parent_path(), ec);
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out) return false;
    out.write(data.data(), static_cast<std::streamsize>(data.size()));
    return static_cast<bool>(out);
}
}  // namespace

ToolResult ToolsService::format_json(const std::string& input) const {
    return json_transform(input, 2);
}

ToolResult ToolsService::minify_json(const std::string& input) const {
    return json_transform(input, -1);
}

ToolResult ToolsService::encrypt_file(const std::string& inputPath,
                                      const std::string& passphrase) {
    auto input = read_file(inputPath);
    if (!input.ok) return input;
    if (!crypto_.unlock(passphrase)) return {false, {}, "invalid passphrase"};
    auto cipher = crypto_.encrypt(input.value);
    crypto_.lock();
    if (!cipher) return {false, {}, "encryption failed"};

    storage_.ensure_dir("vault");
    const fs::path src(inputPath);
    const std::string fileName = src.filename().string() + ".wbenc";
    const fs::path output = fs::path(storage_.data_dir()) / "vault" / fileName;
    if (!write_file(output, *cipher)) return {false, {}, "failed to write encrypted file"};
    return {true, output.string(), {}};
}

ToolResult ToolsService::decrypt_file(const std::string& inputPath,
                                      const std::string& passphrase) {
    auto input = read_file(inputPath);
    if (!input.ok) return input;
    if (!crypto_.unlock(passphrase)) return {false, {}, "invalid passphrase"};
    auto plain = crypto_.decrypt(input.value);
    crypto_.lock();
    if (!plain) return {false, {}, "decryption failed or password is incorrect"};

    storage_.ensure_dir("vault");
    fs::path src(inputPath);
    std::string fileName = src.filename().string();
    if (fileName.size() > 6 && fileName.ends_with(".wbenc"))
        fileName.resize(fileName.size() - 6);
    fileName += ".decrypted";
    const fs::path output = fs::path(storage_.data_dir()) / "vault" / fileName;
    if (!write_file(output, *plain)) return {false, {}, "failed to write decrypted file"};
    return {true, output.string(), {}};
}

}  // namespace wb::tools
