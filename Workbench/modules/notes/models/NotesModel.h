#pragma once
//
// NotesModel — notes 模块的模块级共享业务状态中心（不是单个 View 的 VM）。
//
// 职责：
//   * 持有 notes 列表（ObservableList）与「当前选中笔记」的可编辑草稿状态；
//   * 经 INotesService 完成 Markdown 文件的加载/保存/删除/附件导入；
//   * 保存成功后经 EventBus 发布强类型 wb::events::NoteSaved（sync 等模块可订阅）。
//
// 模块内多个 VM 可共享同一个 NotesModel；依赖均由构造函数注入。
//
#include "models/Note.h"
#include "services/INotesService.h"

#include "aria/aria.hpp"
#include "aria/runtime/event_bus.hpp"

#include <memory>
#include <string>

namespace wb::notes {

class NotesModel {
public:
    NotesModel(std::shared_ptr<INotesService> service,
               aria::runtime::EventBus& bus)
        : service_(std::move(service)), bus_(bus) {}

    // 列表与选中/编辑草稿状态：VM 直接绑定这些响应式成员。
    aria::ObservableList<Note> notes;
    aria::Property<std::string> selectedId{""};
    aria::Property<std::string> draftTitle{""};
    aria::Property<std::string> draftBody{""};
    aria::Property<bool>        hasSelection{false};
    aria::Property<bool>        dirty{false};
    aria::Property<std::string> lastError{""};

    // 从磁盘重新加载全部笔记，清空当前选中。
    [[nodiscard]] bool reload();

    // 新建一条空笔记（立即落盘，成为选中项）。
    [[nodiscard]] bool create_note();

    // 按 id 选中一条笔记，把内容载入草稿；空 id 表示取消选中。
    void select(const NoteId& id);

    // 编辑草稿（仅内存，标记 dirty；保存前不落盘）。
    void set_title(const std::string& title);
    void set_body(const std::string& body);

    // 把当前草稿保存到磁盘并发布 NoteSaved。
    [[nodiscard]] bool save_current();

    // 删除当前选中笔记（连带附件目录），并清空选中。
    [[nodiscard]] bool delete_current();

    // 为当前选中笔记导入附件，返回可插入正文的相对 Markdown 路径。
    [[nodiscard]] bool import_attachment(const std::string& sourcePath,
                                         std::string& outRelativePath);

private:
    void apply_selection_(const Note* note);
    [[nodiscard]] std::shared_ptr<Note> find_(const NoteId& id) const;
    void set_error_(const NotesError error, std::string message);

    std::shared_ptr<INotesService> service_;
    aria::runtime::EventBus& bus_;
};

}  // namespace wb::notes
