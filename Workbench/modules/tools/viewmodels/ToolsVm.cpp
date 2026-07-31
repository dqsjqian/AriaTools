#include "viewmodels/ToolsVm.h"

#include <string>

namespace wb::tools {

ToolsVm::ToolsVm(wb::services::II18nService& i18n,
                 std::shared_ptr<ToolsModel> model)
    : wb::core::LocalizedVm(i18n, "tools"),
      base64Input(""),
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
    bind_text(title, "title");
    bind_text(base64Group, "base64_group");
    bind_text(inputLabel, "input");
    bind_text(outputLabel, "output");
    bind_text(encodeLabel, "encode");
    bind_text(decodeLabel, "decode");
    bind_text(randomGroup, "random_group");
    bind_text(lengthLabel, "length");
    bind_text(generateLabel, "generate");
    bind_text(jsonGroup, "json_group");
    bind_text(formatLabel, "format");
    bind_text(minifyLabel, "minify");
    bind_text(fileGroup, "file_group");
    bind_text(chooseFileLabel, "choose_file");
    bind_text(passphraseLabel, "passphrase");
    bind_text(encryptFileLabel, "encrypt_file");
    bind_text(decryptFileLabel, "decrypt_file");
}

void ToolsVm::apply_json_result_(const ToolResult& result) {
    if (result.ok) {
        jsonOutput.set(result.value);
        jsonStatus.set(tr("json_valid"));
    } else {
        jsonOutput.set("");
        jsonStatus.set(tr("json_error") + ": " + result.error);
    }
}

void ToolsVm::apply_file_result_(const ToolResult& result, std::string_view successKey) {
    if (result.ok) {
        fileStatus.set(tr(successKey) + ": " + result.value);
    } else {
        fileStatus.set(tr("file_error") + ": " + result.error);
    }
}

void ToolsVm::on_activate() {}
void ToolsVm::on_deactivate() { bag().clear(); }

}  // namespace wb::tools
