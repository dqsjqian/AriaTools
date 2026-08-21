#include "platforms/android/FrameworkLabJniBinding.h"

namespace wb::frameworklab {

void register_frameworklab_binding(wb::jni::BindingTable& table) {
    wb::jni::ModuleBinding binding;
    binding.subscribe = [](aria::runtime::EventBus&, aria::binding::ViewModel& base,
                           std::vector<aria::Subscription>& subs) {
        auto& vm = static_cast<FrameworkLabVm&>(base);
        using wb::jni::bind_str;
        using wb::jni::push_property;
        bind_str(subs, "frameworklab", "title", vm.title);
        bind_str(subs, "frameworklab", "desc", vm.desc);
        bind_str(subs, "frameworklab", "draft", vm.draft);
        bind_str(subs, "frameworklab", "draftLabel", vm.draftLabel);
        bind_str(subs, "frameworklab", "addLabel", vm.addLabel);
        bind_str(subs, "frameworklab", "toggleLabel", vm.toggleLabel);
        bind_str(subs, "frameworklab", "removeLabel", vm.removeLabel);
        bind_str(subs, "frameworklab", "clearLabel", vm.clearLabel);
        bind_str(subs, "frameworklab", "refreshLabel", vm.refreshLabel);
        bind_str(subs, "frameworklab", "graphLabel", vm.graphLabel);
        bind_str(subs, "frameworklab", "summary", vm.summaryText);
        bind_str(subs, "frameworklab", "selected", vm.selectedText);
        bind_str(subs, "frameworklab", "graph", vm.graphSnapshot);
        auto sync_tasks = [&vm] {
            std::string joined;
            for (const auto& task : vm.tasks->snapshot()) {
                if (!joined.empty()) joined += "\n";
                joined += task->completed ? "[x] " : "[ ] ";
                joined += task->title;
            }
            push_property("frameworklab", "tasks", joined);
        };
        sync_tasks();
        subs.push_back(vm.tasks->on_any_change(sync_tasks));
    };
    binding.set_text = [](aria::binding::ViewModel& base,
                          const std::string& name, const std::string& value) {
        auto& vm = static_cast<FrameworkLabVm&>(base);
        if (name == "draft") vm.draft.set(value);
        else if (name == "selectedIndex") {
            try { vm.select_index(static_cast<std::size_t>(std::stoul(value))); }
            catch (...) {}
        }
    };
    binding.exec_command = [](aria::binding::ViewModel& base,
                              const std::string& name) {
        auto& vm = static_cast<FrameworkLabVm&>(base);
        if (name == "addTask") vm.addTask.execute();
        else if (name == "toggleSelected") vm.toggleSelected.execute();
        else if (name == "removeSelected") vm.removeSelected.execute();
        else if (name == "clearCompleted") vm.clearCompleted.execute();
        else if (name == "refreshGraph") vm.refreshGraph.execute();
    };
    table.emplace("frameworklab", std::move(binding));
}

}  // namespace wb::frameworklab
