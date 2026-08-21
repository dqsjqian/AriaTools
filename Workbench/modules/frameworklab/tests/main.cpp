#include "viewmodels/FrameworkLabVm.h"

#include <cstdio>
#include <string>

namespace {
int failures = 0;
void check(bool condition, const char* name) {
    std::printf("[frameworklab-tests] %s: %s\n", condition ? "ok" : "FAIL", name);
    if (!condition) ++failures;
}
}  // namespace

int main() {
    wb::frameworklab::FrameworkLabVm vm;

    check(vm.tasks->size() == 3, "seeded source list");
    check(vm.active.size() == 2, "active FilteredList derives source");
    check(vm.completed.size() == 1, "completed FilteredList derives source");

    vm.select_index(1);
    vm.toggleSelected.execute();
    check(vm.active.size() == 1, "toggle updates active filter");
    check(vm.completed.size() == 2, "toggle updates completed filter");
    check(vm.activeCount.get() == 1, "Computed active count follows revision");

    vm.select_index(0);
    check(vm.selection.has_value(), "selection contains source item");
    vm.removeSelected.execute();
    check(!vm.selection.has_value(), "removing selected item auto-clears Selection");

    vm.refreshGraph.execute();
    const std::string graph = vm.graphSnapshot.get();
    check(graph.find("[Source] frameworklab.tasks_revision") != std::string::npos,
          "inspector contains real Property source");
    check(graph.find("[Derivation] frameworklab.summary_text") != std::string::npos,
          "inspector contains real Computed derivation");
    check(graph.find("depth=") != std::string::npos,
          "inspector output is textual graph snapshot");

    vm.clearCompleted.execute();
    check(vm.completed.empty(), "clear command removes completed items");
    check(vm.tasks->size() == 0, "only completed items remained and were cleared");

    std::puts(failures == 0 ? "[frameworklab-tests] PASS" : "[frameworklab-tests] FAIL");
    return failures == 0 ? 0 : 1;
}
