// notes 模块业务测试：直接驱动 MarkdownNotesService + NotesModel，
// 用临时目录做真实文件持久化，不依赖主 app / 其它业务模块。
#include "models/NotesModel.h"
#include "services/MarkdownNotesService.h"
#include "module_api/AppEvents.h"

#include "infra/storage/IStorageService.h"
#include "infra/log/Log.h"
#include "aria/runtime/event_bus.hpp"

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

namespace fs = std::filesystem;

namespace wb::services {
// 存储桩工厂（LocalFileStorageService.cpp 提供）。
IStorageService* make_local_file_storage_service(std::string dataDir);
}

namespace {

int g_failures = 0;

void check(bool cond, const char* what) {
    if (!cond) {
        std::printf("[notes-tests] FAIL: %s\n", what);
        ++g_failures;
    } else {
        std::printf("[notes-tests] ok: %s\n", what);
    }
}

fs::path make_temp_dir() {
    const auto stamp = std::chrono::high_resolution_clock::now()
                           .time_since_epoch()
                           .count();
    auto base = fs::temp_directory_path() /
        ("wb-notes-" + std::to_string(stamp));
    std::error_code ec;
    fs::remove_all(base, ec);
    fs::create_directories(base, ec);
    return base;
}

}  // namespace

int main() {
    using namespace wb::notes;

    wb::log::init_default_sink();  // 让 NotesModel 的流式日志在测试输出里可见

    const fs::path dataDir = make_temp_dir();
    std::unique_ptr<wb::services::IStorageService> storage(
        wb::services::make_local_file_storage_service(dataDir.string()));

    auto service = std::make_shared<MarkdownNotesService>(*storage);
    aria::runtime::EventBus bus;

    // 订阅 NoteSaved，验证保存事件确实发布。
    int savedEvents = 0;
    std::string lastSavedId;
    auto sub = bus.subscribe<wb::events::NoteSaved>(
        [&](const wb::events::NoteSaved& ev) { ++savedEvents; lastSavedId = ev.id; });

    NotesModel model(service, bus);

    // 1) 初始为空。
    check(model.reload(), "reload empty");
    check(model.notes.size() == 0, "empty list after reload");
    check(!model.hasSelection.get(), "no selection initially");

    // 2) 新建 → 落盘 + 选中 + 发布事件。
    check(model.create_note(), "create note");
    check(model.notes.size() == 1, "one note after create");
    check(model.hasSelection.get(), "selected after create");
    check(savedEvents == 1, "NoteSaved published on create");
    const std::string id = model.selectedId.get();
    check(!id.empty(), "created note has id");

    // 3) 编辑草稿 + 保存 → 文件内容更新 + 再次发布事件。
    model.set_title("Hello");
    model.set_body("# Body\n正文图文");
    check(model.dirty.get(), "dirty after edit");
    check(model.save_current(), "save current");
    check(!model.dirty.get(), "clean after save");
    check(savedEvents == 2, "NoteSaved published on save");
    check(lastSavedId == id, "saved event carries correct id");

    // 文件真的存在且含标题。
    const fs::path notePath = dataDir / "notes" / (id + ".md");
    check(fs::exists(notePath), "note file exists on disk");
    {
        std::ifstream in(notePath);
        std::string content((std::istreambuf_iterator<char>(in)),
                            std::istreambuf_iterator<char>());
        check(content.find("title: \"Hello\"") != std::string::npos,
              "front matter contains title");
        check(content.find("# Body") != std::string::npos,
              "body persisted");
    }

    // 4) 重新加载后内容一致（往返）。
    check(model.reload(), "reload after save");
    check(model.notes.size() == 1, "one note after reload");
    model.select(id);
    check(model.hasSelection.get(), "reselect after reload");
    check(model.draftTitle.get() == "Hello", "title round-trips");
    check(model.draftBody.get() == "# Body\n正文图文", "body round-trips");

    // 5) 附件导入 → 生成相对路径并复制文件。
    const fs::path srcFile = dataDir / "src.png";
    { std::ofstream(srcFile) << "PNGDATA"; }
    std::string rel;
    check(model.import_attachment(srcFile.string(), rel), "import attachment");
    check(rel == ("assets/" + id + "/src.png"), "attachment relative path");
    check(fs::exists(dataDir / "notes" / "assets" / id / "src.png"),
          "attachment copied to disk");

    // 6) 删除 → 文件与附件目录被清除 + 取消选中。
    model.select(id);
    check(model.delete_current(), "delete current");
    check(model.notes.size() == 0, "empty after delete");
    check(!model.hasSelection.get(), "no selection after delete");
    check(!fs::exists(notePath), "note file removed");
    check(!fs::exists(dataDir / "notes" / "assets" / id),
          "attachment dir removed");

    std::error_code ec;
    fs::remove_all(dataDir, ec);

    std::puts(g_failures == 0 ? "[notes-tests] PASS" : "[notes-tests] FAIL");
    return g_failures == 0 ? 0 : 1;
}
