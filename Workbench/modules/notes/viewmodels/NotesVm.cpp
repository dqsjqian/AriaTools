#include "viewmodels/NotesVm.h"

#include "infra/i18n/I18n.h"

namespace wb::notes {

NotesVm::NotesVm(std::shared_ptr<NotesModel> model)
    : notes(model->notes),
      selectedId(model->selectedId),
      editTitle(""),
      editBody(""),
      hasSelection(model->hasSelection),
      dirty(model->dirty),
      status(""),
      addNote([this] {
          (void)model_->create_note();
          load_selection_into_editor_();
          refresh_status_();
      }),
      selectNote([this](const std::string& id) {
          model_->select(id);
          load_selection_into_editor_();
      }),
      saveNote([this] {
          model_->set_title(editTitle.get());
          model_->set_body(editBody.get());
          (void)model_->save_current();
      }),
      deleteSelected([this] {
          (void)model_->delete_current();
          load_selection_into_editor_();
          refresh_status_();
      }),
      model_(std::move(model))
{
    // 编辑器 → Model 草稿（保存前不落盘，仅标记 dirty）。
    track(editTitle.on_changed([this](const std::string& v) { model_->set_title(v); }));
    track(editBody.on_changed([this](const std::string& v) { model_->set_body(v); }));

    // 静态文案：就近书写，切语言自动刷新。
    text(title,            "title");
    text(hint,             "hint");
    text(addLabel,         "add");
    text(saveLabel,        "save");
    text(deleteLabel,      "delete");
    text(titlePlaceholder, "title_placeholder");
    text(bodyPlaceholder,  "body_placeholder");

    // 动态状态：既随列表数量变化，也随语言变化刷新。
    localize([this] { refresh_status_(); });
    track(notes.on_any_change([this]() { refresh_status_(); }));

    load_selection_into_editor_();
}

void NotesVm::on_activate() {
    (void)model_->reload();
    load_selection_into_editor_();
    refresh_status_();
}

void NotesVm::on_deactivate() { bag().clear(); }

void NotesVm::load_selection_into_editor_() {
    // 从 Model 草稿同步到编辑器 Property；此处仅镜像，不回写 Model。
    editTitle.set(model_->draftTitle.get());
    editBody.set(model_->draftBody.get());
}

void NotesVm::refresh_status_() {
    const auto n = notes.size();
    if (n == 0) {
        status.set(wb::i18n::str("empty"));
    } else {
        status.set(wb::i18n::str("count_prefix") + std::to_string(n) + wb::i18n::str("count_suffix"));
    }
}

}  // namespace wb::notes
