#pragma once
//
// NotesVm — 记事本 ViewModel。只维护页面响应式状态与 Command，
// 列表/选中/编辑/保存/删除等业务一律委托 NotesModel；View 只绑定 VM。
//
#include "aria/aria.hpp"
#include "module_api/BaseVm.h"
#include "models/NotesModel.h"

#include <memory>

namespace wb::notes {

class NotesVm final : public wb::core::BaseVm {
public:
    explicit NotesVm(std::shared_ptr<NotesModel> model);

    // 列表与编辑状态（转发自 Model，供 View 绑定）。
    aria::ObservableList<Note>& notes;
    aria::Property<std::string>& selectedId;
    aria::Property<std::string>  editTitle;
    aria::Property<std::string>  editBody;
    aria::Property<bool>&        hasSelection;
    aria::Property<bool>&        dirty;
    aria::Property<std::string>  status;   ///< 动态：随语言 + 列表数量重算

    // 命令
    aria::Command<>            addNote;
    aria::Command<std::string> selectNote;   ///< 参数：笔记 id
    aria::Command<>            saveNote;
    aria::Command<>            deleteSelected;

    // 界面文案（随语言更新）
    aria::Property<std::string> title;
    aria::Property<std::string> hint;
    aria::Property<std::string> addLabel;
    aria::Property<std::string> saveLabel;
    aria::Property<std::string> deleteLabel;
    aria::Property<std::string> titlePlaceholder;
    aria::Property<std::string> bodyPlaceholder;

    void on_activate() override;
    void on_deactivate() override;

private:
    void refresh_status_();
    void load_selection_into_editor_();

    std::shared_ptr<NotesModel> model_;
};

}  // namespace wb::notes
