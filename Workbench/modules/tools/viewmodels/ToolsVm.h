#pragma once
//
// ToolsVm — tools module view model. Only maintains reactive UI state and Commands;
// JSON/file business is delegated to ToolsService; the View only binds to the VM.
//
#include "aria/aria.hpp"
#include "module_api/BaseVm.h"
#include "models/ToolsModel.h"

#include <memory>

namespace wb::tools {

class ToolsVm final : public wb::core::BaseVm {
public:
    explicit ToolsVm(std::shared_ptr<ToolsModel> model);

    // Base64
    aria::Property<std::string> base64Input;
    aria::Property<std::string> base64Output;
    aria::Command<> base64Encode;
    aria::Command<> base64Decode;

    // Random string
    // Random-string length bounds — business constraint, owned by the VM.
    static constexpr int kRandomLengthMin = 1;
    static constexpr int kRandomLengthMax = 256;
    aria::Property<int>         randomLength;
    aria::Property<std::string> randomOutput;
    aria::Command<> genRandom;

    // JSON
    aria::Property<std::string> jsonInput;
    aria::Property<std::string> jsonOutput;
    aria::Property<std::string> jsonStatus;
    aria::Command<> jsonFormat;
    aria::Command<> jsonMinify;

    // File encryption/decryption
    aria::Property<std::string> filePath;
    aria::Property<std::string> passphrase;
    aria::Property<std::string> fileStatus;
    aria::Command<> encryptFile;
    aria::Command<> decryptFile;

    // UI text (updates on language change)
    aria::Property<std::string> title;
    aria::Property<std::string> base64Group;
    aria::Property<std::string> inputLabel;
    aria::Property<std::string> outputLabel;
    aria::Property<std::string> encodeLabel;
    aria::Property<std::string> decodeLabel;
    aria::Property<std::string> randomGroup;
    aria::Property<std::string> lengthLabel;
    aria::Property<std::string> generateLabel;
    aria::Property<std::string> jsonGroup;
    aria::Property<std::string> formatLabel;
    aria::Property<std::string> minifyLabel;
    aria::Property<std::string> fileGroup;
    aria::Property<std::string> chooseFileLabel;
    aria::Property<std::string> passphraseLabel;
    aria::Property<std::string> encryptFileLabel;
    aria::Property<std::string> decryptFileLabel;

    void on_activate() override;
    void on_deactivate() override;

private:
    std::shared_ptr<ToolsModel> model_;
    void apply_json_result_(const ToolResult& result);
    void apply_file_result_(const ToolResult& result, std::string_view successKey);
};

}  // namespace wb::tools
