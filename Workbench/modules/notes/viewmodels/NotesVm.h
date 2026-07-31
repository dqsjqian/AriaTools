#pragma once
//
// NotesVm — 记事本。文案经 i18n 派发；status 随语言与列表变化重算。
//
#include "aria/aria.hpp"
#include "module_api/LocalizedVm.h"
#include "entities/NoteModel.h"

#include <memory>

namespace wb::notes {

class NotesVm final : public wb::core::LocalizedVm {
public:
    explicit NotesVm(wb::services::II18nService& i18n);

    aria::ObservableList<NoteModel> notes;

    // 界面文案（随语言更新）
    aria::Property<std::string> title;
    aria::Property<std::string> hint;
    aria::Property<std::string> addLabel;
    aria::Property<std::string> deleteLabel;
    aria::Property<std::string> status;   ///< 动态：随语言 + 列表数量重算

    aria::Command<> addNote;
    aria::Command<> deleteSelected;

    void on_activate() override;
    void on_deactivate() override;

private:
    int counter_ = 0;
    void refresh_status_();
};

}  // namespace wb::notes
