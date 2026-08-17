// ────────────────────────────────────────────────────────────────────────────
//  SearchJniBinding.cpp — see SearchJniBinding.h.
// ────────────────────────────────────────────────────────────────────────────
#include "platforms/android/SearchJniBinding.h"

namespace wb::search {

void subscribe_search(aria::runtime::EventBus& bus, SearchVm& vm,
                      std::vector<aria::Subscription>& subs) {
    (void)bus;
    using wb::jni::bind_str;
    using wb::jni::push_property;

    bind_str(subs, "search", "title", vm.title);
    bind_str(subs, "search", "desc",  vm.desc);
    bind_str(subs, "search", "query", vm.query);
    bind_str(subs, "search", "debounced", *vm.debounced);
    bind_str(subs, "search", "distinct",  *vm.distinct);
    bind_str(subs, "search", "placeholder", vm.placeholder);
    bind_str(subs, "search", "searches",   vm.searchesLabel);
    // Hits list: push as newline-joined strings.
    auto sync_hits = [&vm]() {
        std::string joined;
        for (const auto& h : vm.hits.snapshot()) {
            if (!joined.empty()) joined += "\n";
            joined += "#" + std::to_string(h->seq) + " " + h->q;
        }
        push_property("search", "hits", joined);
    };
    sync_hits();
    subs.push_back(vm.hits.on_any_change(
        [sync_hits]() { sync_hits(); }));
}

void set_search_text(SearchVm& vm, const std::string& propName,
                     const std::string& value) {
    if (propName == "query") vm.query.set(value);
}

void exec_search_command(SearchVm& vm, const std::string& cmdName) {
    (void)vm;
    (void)cmdName;  // search exposes no parameterless commands via the shell
}

void register_search_binding(wb::jni::BindingTable& table) {
    wb::jni::ModuleBinding b;
    b.subscribe = [](aria::runtime::EventBus& bus, aria::binding::ViewModel& vm,
                     std::vector<aria::Subscription>& subs) {
        subscribe_search(bus, static_cast<SearchVm&>(vm), subs);
    };
    b.set_text = [](aria::binding::ViewModel& vm, const std::string& propName,
                    const std::string& value) {
        set_search_text(static_cast<SearchVm&>(vm), propName, value);
    };
    b.exec_command = [](aria::binding::ViewModel& vm, const std::string& cmdName) {
        exec_search_command(static_cast<SearchVm&>(vm), cmdName);
    };
    table.emplace("search", b);
}

}  // namespace wb::search
