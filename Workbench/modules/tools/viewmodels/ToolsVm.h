#pragma once
//
// ToolsVm — tools 模块 ViewModel。只维护响应式 UI 状态与 Command，
// JSON/文件业务委托给 ToolsService；View 只绑定 VM。
//
#include "aria/aria.hpp"
#include "module_api/LocalizedVm.h"
#include "models/ToolsModel.h"

#include <memory>

namespace wb::tools {

class ToolsVm final : public wb::core::LocalizedVm {
public:
    ToolsVm(wb::services::II18nService& i18n,
            std::shared_ptr<ToolsModel> model);

    // Base64
    aria::Property<std::string> base64Input;
    aria::Property<std::string> base64Output;
    aria::Command<> base64Encode;
    aria::Command<> base64Decode;

    // 随机字符串
    aria::Property<int>         randomLength;
    aria::Property<std::string> randomOutput;
    aria::Command<> genRandom;

    // JSON
    aria::Property<std::string> jsonInput;
    aria::Property<std::string> jsonOutput;
    aria::Property<std::string> jsonStatus;
    aria::Command<> jsonFormat;
    aria::Command<> jsonMinify;

    // 文件加解密
    aria::Property<std::string> filePath;
    aria::Property<std::string> passphrase;
    aria::Property<std::string> fileStatus;
    aria::Command<> encryptFile;
    aria::Command<> decryptFile;

    // 界面文案（随语言更新）
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
