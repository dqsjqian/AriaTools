#include "viewmodels/ToolsVm.h"

#include "infra/i18n/I18n.h"

#include <string>

namespace wb::tools {

ToolsVm::ToolsVm(std::shared_ptr<ToolsModel> model)
    : base64Input(""),
      base64Output(""),
      base64Encode([this] {
          base64Output.set(model_->encode_base64(base64Input.get()));
      }),
      base64Decode([this] {
          base64Output.set(model_->decode_base64(base64Input.get()));
      }),
      randomLength(16),
      randomOutput(""),
      genRandom([this] { randomOutput.set(model_->generate_random(randomLength.get())); }),
      jsonInput(""),
      jsonOutput(""),
      jsonStatus(""),
      jsonFormat([this] { apply_json_result_(model_->format_json(jsonInput.get())); }),
      jsonMinify([this] { apply_json_result_(model_->minify_json(jsonInput.get())); }),
      filePath(""),
      passphrase(""),
      fileStatus(""),
      encryptFile([this] {
          apply_file_result_(model_->encrypt_file(filePath.get(), passphrase.get()),
                             "encrypt_success");
      }),
      decryptFile([this] {
          apply_file_result_(model_->decrypt_file(filePath.get(), passphrase.get()),
                             "decrypt_success");
      }),
      model_(std::move(model))
{
    text(title,            "title");
    text(base64Group,      "base64_group");
    text(inputLabel,       "input");
    text(outputLabel,      "output");
    text(encodeLabel,      "encode");
    text(decodeLabel,      "decode");
    text(randomGroup,      "random_group");
    text(lengthLabel,      "length");
    text(generateLabel,    "generate");
    text(jsonGroup,        "json_group");
    text(formatLabel,      "format");
    text(minifyLabel,      "minify");
    text(fileGroup,        "file_group");
    text(chooseFileLabel,  "choose_file");
    text(passphraseLabel,  "passphrase");
    text(encryptFileLabel, "encrypt_file");
    text(decryptFileLabel, "decrypt_file");
}

void ToolsVm::apply_json_result_(const ToolResult& result) {
    if (result.ok) {
        jsonOutput.set(result.value);
        jsonStatus.set(wb::i18n::str("json_valid"));
    } else {
        jsonOutput.set("");
        jsonStatus.set(wb::i18n::str("json_error") + ": " + result.error);
    }
}

void ToolsVm::apply_file_result_(const ToolResult& result, std::string_view successKey) {
    if (result.ok) {
        fileStatus.set(wb::i18n::str(successKey) + ": " + result.value);
    } else {
        fileStatus.set(wb::i18n::str("file_error") + ": " + result.error);
    }
}

void ToolsVm::on_activate() {}
void ToolsVm::on_deactivate() { bag().clear(); }

}  // namespace wb::tools
