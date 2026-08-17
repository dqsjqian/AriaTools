// ────────────────────────────────────────────────────────────────────────────
//  CalendarJniBinding.cpp — see CalendarJniBinding.h.
// ────────────────────────────────────────────────────────────────────────────
#include "platforms/android/CalendarJniBinding.h"

namespace wb::calendar {

void subscribe_calendar(aria::runtime::EventBus& bus, CalendarVm& vm,
                        std::vector<aria::Subscription>& subs) {
    (void)bus;
    using wb::jni::bind_str;
    using wb::jni::push_property;

    bind_str(subs, "calendar", "title",      vm.title);
    bind_str(subs, "calendar", "hint",       vm.hint);
    bind_str(subs, "calendar", "monthTitle",  vm.monthTitle);
    bind_str(subs, "calendar", "status",      vm.status);
    bind_str(subs, "calendar", "subscribeUrl",vm.subscribeUrl);
    // Labels for nav buttons (i18n).
    bind_str(subs, "calendar", "prev",      vm.prevLabel);
    bind_str(subs, "calendar", "next",      vm.nextLabel);
    bind_str(subs, "calendar", "today",     vm.todayLabel);
    bind_str(subs, "calendar", "refresh",   vm.refreshLabel);
    bind_str(subs, "calendar", "subscribe", vm.subscribeLabel);
    bind_str(subs, "calendar", "url_placeholder", vm.urlPlaceholder);
    // Events list: join the current month's event titles into a
    // newline-joined string so the Compose LazyColumn can render.
    auto sync_events = [&vm]() {
        std::string joined;
        for (const auto& day : vm.days.snapshot()) {
            if (!day || day->eventTitles.empty()) continue;
            for (const auto& t : day->eventTitles) {
                if (!joined.empty()) joined += "\n";
                joined += day->label + "  " + t;
            }
        }
        push_property("calendar", "events", joined);
    };
    sync_events();
    subs.push_back(vm.days.on_any_change(
        [sync_events]() { sync_events(); }));
}

void set_calendar_text(CalendarVm& vm, const std::string& propName,
                       const std::string& value) {
    if (propName == "subscribeUrl") vm.subscribeUrl.set(value);
}

void exec_calendar_command(CalendarVm& vm, const std::string& cmdName) {
    if (cmdName == "prevMonth")       vm.prevMonth.execute();
    else if (cmdName == "nextMonth")  vm.nextMonth.execute();
    else if (cmdName == "today")      vm.today.execute();
    else if (cmdName == "refresh")     vm.refresh.execute();
    else if (cmdName == "addSubscription") vm.addSubscription.execute();
}

void register_calendar_binding(wb::jni::BindingTable& table) {
    wb::jni::ModuleBinding b;
    b.subscribe = [](aria::runtime::EventBus& bus, aria::binding::ViewModel& vm,
                     std::vector<aria::Subscription>& subs) {
        subscribe_calendar(bus, static_cast<CalendarVm&>(vm), subs);
    };
    b.set_text = [](aria::binding::ViewModel& vm, const std::string& propName,
                    const std::string& value) {
        set_calendar_text(static_cast<CalendarVm&>(vm), propName, value);
    };
    b.exec_command = [](aria::binding::ViewModel& vm, const std::string& cmdName) {
        exec_calendar_command(static_cast<CalendarVm&>(vm), cmdName);
    };
    table.emplace("calendar", b);
}

}  // namespace wb::calendar
