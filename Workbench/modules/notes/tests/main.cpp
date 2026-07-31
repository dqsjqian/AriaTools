// notes 模块业务测试：不依赖主 app / 其它业务模块。
#include "infra/ServiceHub.h"
#include "module_api/ModuleContext.h"
#include "module/NotesModule.h"
#include "viewmodels/NotesVm.h"

#include <cstdio>

int main() {
    // 最小 ServiceHub（i18n 目录指向本模块 i18n；缺 common 时导航文案回退 [.]，不影响本测）。
    wb::infra::ServiceHub hub{"./i18n", "zh-CN"};
    wb::module_api::ModuleContext ctx{hub};

    auto mod = wb::notes::make_notes_module();
    auto vmBase = mod->create_view_model(ctx);
    auto* vm = dynamic_cast<wb::notes::NotesVm*>(vmBase.get());
    if (!vm) { std::puts("[notes-tests] FAIL: VM type"); return 1; }

    vm->activate();
    std::printf("[notes-tests] module id=%s, initial notes=%zu, status='%s'\n",
                mod->id().c_str(), vm->notes.size(), vm->status.get().c_str());

    vm->addNote.execute();
    vm->addNote.execute();
    std::printf("[notes-tests] after 2 adds: notes=%zu, status='%s'\n",
                vm->notes.size(), vm->status.get().c_str());

    vm->deleteSelected.execute();
    std::printf("[notes-tests] after delete: notes=%zu\n", vm->notes.size());

    std::puts(vm->notes.size() == 1 ? "[notes-tests] PASS" : "[notes-tests] FAIL");
    return vm->notes.size() == 1 ? 0 : 1;
}
