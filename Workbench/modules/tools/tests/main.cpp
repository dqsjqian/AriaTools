#include "infra/ServiceHub.h"
#include "models/ToolsModel.h"
#include "services/ToolsService.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

int main() {
    wb::infra::ServiceHub hub{"./i18n", "zh-CN"};
    auto service = std::make_shared<wb::tools::ToolsService>(hub.crypto(), hub.storage());
    wb::tools::ToolsModel model{std::move(service)};

    if (model.decode_base64(model.encode_base64("Aria")) != "Aria" ||
        model.generate_random(24).size() != 24) {
        std::cerr << "[tools-tests] Base64/random FAIL\n";
        return 1;
    }

    const auto formatted = model.format_json("{\"b\":2,\"a\":[1,true]}");
    const auto minified = model.minify_json(formatted.value);
    if (!formatted.ok || !minified.ok || minified.value != "{\"a\":[1,true],\"b\":2}") {
        std::cerr << "[tools-tests] JSON FAIL\n";
        return 1;
    }

    const fs::path input = fs::temp_directory_path() / "wb-tools-harness.txt";
    const std::string content = "Aria Workbench crypto roundtrip\n";
    { std::ofstream out(input, std::ios::binary); out << content; }

    const auto encrypted = model.encrypt_file(input.string(), "strong-passphrase");
    if (!encrypted.ok) {
        std::cerr << "[tools-tests] encrypt FAIL: " << encrypted.error << '\n';
        return 2;
    }
    const auto decrypted = model.decrypt_file(encrypted.value, "strong-passphrase");
    if (!decrypted.ok) {
        std::cerr << "[tools-tests] decrypt FAIL: " << decrypted.error << '\n';
        return 3;
    }

    std::ifstream in(decrypted.value, std::ios::binary);
    const std::string restored((std::istreambuf_iterator<char>(in)),
                               std::istreambuf_iterator<char>());
    if (restored != content) {
        std::cerr << "[tools-tests] roundtrip mismatch\n";
        return 4;
    }

    std::cout << "[tools-tests] Base64/random PASS\n"
              << "[tools-tests] JSON format/minify PASS\n"
              << "[tools-tests] Apple native encrypt/decrypt PASS\n";
    return 0;
}
