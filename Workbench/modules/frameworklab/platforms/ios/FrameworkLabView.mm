#include "FrameworkLabView.h"

#include "support/IosUi.h"
#include "viewmodels/FrameworkLabVm.h"

namespace wb::frameworklab::iosview {

FrameworkLabView::FrameworkLabView(FrameworkLabVm& vm,
                                   aria::binding::BindingEngine& be)
    : vc_(nil) {
    UILabel* title = wb::ios::ui::make_title(@"");
    UILabel* desc = wb::ios::ui::make_label(@"");
    UILabel* draftLabel = wb::ios::ui::make_label(@"");
    UITextField* draft = wb::ios::ui::make_field(@"");
    UIButton* add = wb::ios::ui::make_button(@"");
    UILabel* selected = wb::ios::ui::make_label(@"");
    UILabel* summary = wb::ios::ui::make_label(@"");
    UIButton* toggle = wb::ios::ui::make_button(@"");
    UIButton* remove = wb::ios::ui::make_button(@"");
    UIButton* clear = wb::ios::ui::make_button(@"");
    UILabel* graphTitle = wb::ios::ui::make_label(@"");
    UIButton* refresh = wb::ios::ui::make_button(@"");
    UILabel* graph = wb::ios::ui::make_label(@"");
    graph.numberOfLines = 0;
    graph.font = [UIFont monospacedSystemFontOfSize:11 weight:UIFontWeightRegular];

    vc_ = wb::ios::ui::make_stack_vc(@[
        title, desc, draftLabel, draft, add, selected, summary, toggle,
        remove, clear, graphTitle, refresh, graph
    ]);

    be.bind_text_oneway(vm.title, wb::ios::ui::view_for(title));
    be.bind_text_oneway(vm.desc, wb::ios::ui::view_for(desc));
    be.bind_text_oneway(vm.draftLabel, wb::ios::ui::view_for(draftLabel));
    be.bind_text(vm.draft, wb::ios::ui::view_for(draft));
    be.bind_text_oneway(vm.addLabel, wb::ios::ui::view_for(add));
    be.bind_text_oneway(vm.toggleLabel, wb::ios::ui::view_for(toggle));
    be.bind_text_oneway(vm.removeLabel, wb::ios::ui::view_for(remove));
    be.bind_text_oneway(vm.clearLabel, wb::ios::ui::view_for(clear));
    be.bind_text_oneway(vm.graphLabel, wb::ios::ui::view_for(graphTitle));
    be.bind_text_oneway(vm.refreshLabel, wb::ios::ui::view_for(refresh));
    be.bind_command(vm.addTask, wb::ios::ui::view_for(add));
    be.bind_command(vm.toggleSelected, wb::ios::ui::view_for(toggle));
    be.bind_command(vm.removeSelected, wb::ios::ui::view_for(remove));
    be.bind_command(vm.clearCompleted, wb::ios::ui::view_for(clear));
    be.bind_command(vm.refreshGraph, wb::ios::ui::view_for(refresh));

    auto sync = [](UILabel* label, const std::string& value) {
        label.text = [NSString stringWithUTF8String:value.c_str()];
    };
    sync(selected, vm.selectedText.get());
    subscriptions_.push_back(vm.selectedText.on_changed(
        [selected, sync](const std::string& value) { sync(selected, value); }));
    sync(summary, vm.summaryText.get());
    subscriptions_.push_back(vm.summaryText.on_changed(
        [summary, sync](const std::string& value) { sync(summary, value); }));
    sync(graph, vm.graphSnapshot.get());
    subscriptions_.push_back(vm.graphSnapshot.on_changed(
        [graph, sync](const std::string& value) { sync(graph, value); }));
}

}  // namespace wb::frameworklab::iosview
