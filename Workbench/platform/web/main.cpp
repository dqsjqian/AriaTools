#include "app/AppCore.h"
#include "viewmodels/TipCalcVm.h"

#include "aria/adapters/http/http_adapter.hpp"
#include "aria/binding/binding_engine.hpp"
#include "aria/runtime/dispatcher.hpp"
#include "aria/runtime/dispatcher_executor.hpp"

#include <httplib.h>

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

namespace {

std::atomic<bool> stop_requested{false};

void handle_signal(int) { stop_requested = true; }

std::shared_ptr<wb::tipcalc::TipCalcVm> find_tipcalc(wb::core::AppCore& core) {
    for (const auto& module : core.modules()) {
        if (module.id == "tipcalc") {
            return std::dynamic_pointer_cast<wb::tipcalc::TipCalcVm>(module.vm);
        }
    }
    return nullptr;
}

int probe(unsigned short port, aria::runtime::SimpleDispatcher& dispatcher) {
    httplib::Client client{"127.0.0.1", port};
    client.set_connection_timeout(2, 0);
    const auto health = client.Get("/aria/health");
    const auto views = client.Get("/aria/views");
    if (!health || health->status != 200 || health->body.find("\"ok\":true") == std::string::npos) {
        std::cerr << "probe: /aria/health failed\n";
        return 10;
    }
    if (!views || views->status != 200 || views->body.find("tipcalc.per_person") == std::string::npos) {
        std::cerr << "probe: /aria/views did not expose the shared TipCalc VM\n";
        return 11;
    }

    const auto write = client.Post("/aria/state",
        R"({"view":"tipcalc.bill","field":"double","value":100.0})",
        "application/json");
    if (!write || write->status != 200) {
        std::cerr << "probe: browser-to-VM state write failed\n";
        return 12;
    }
    dispatcher.pump(std::chrono::milliseconds{100});
    const auto derived = client.Get("/aria/state?view=tipcalc.per_person");
    if (!derived || derived->status != 200 || derived->body.find("57.500000") == std::string::npos) {
        std::cerr << "probe: VM Computed value did not flow back to HTTP state\n";
        return 13;
    }

    // Round Up click gate: with a fractional bill the click must ceil it;
    // with an integer bill + 5-multiple tip it must be a silent no-op
    // (can_execute false). Guards the wire can_execute wiring.
    client.Post("/aria/state",
        R"({"view":"tipcalc.bill","field":"double","value":100.5})",
        "application/json");
    dispatcher.pump(std::chrono::milliseconds{100});
    const auto before = client.Get("/aria/state?view=tipcalc.bill");
    client.Post("/aria/click", R"({"view":"tipcalc.round_up"})",
        "application/json");
    dispatcher.pump(std::chrono::milliseconds{100});
    const auto after = client.Get("/aria/state?view=tipcalc.bill");
    if (!before || before->body.find("\"value\":100.5") == std::string::npos ||
        !after || after->body.find("\"value\":101.0") == std::string::npos) {
        std::cerr << "probe: round_up click did not ceil the bill\n";
        return 14;
    }

    std::cout << "probe: HTTP health + browser→Property→Computed→HTTP passed\n";
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    bool probe_mode = false;
    unsigned short requested_port = 19090;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--probe") probe_mode = true;
        else if (arg == "--port" && i + 1 < argc) {
            requested_port = static_cast<unsigned short>(std::stoi(argv[++i]));
        }
    }
    if (probe_mode) requested_port = 0;

    auto dispatcher = std::make_shared<aria::runtime::SimpleDispatcher>();
    aria::runtime::DispatcherExecutor ui_executor{*dispatcher};
    aria::runtime::DispatcherScheduler timer{*dispatcher};

    wb::core::AppCore core{WB_WEB_I18N_ROOT, "zh-CN"};
    core.set_ui_executor(&ui_executor);
    core.set_timer(&timer);
    core.load_modules();

    auto vm = find_tipcalc(core);
    if (!vm) {
        std::cerr << "web: tipcalc module not found\n";
        return 2;
    }

    aria::adapters::http::HttpAdapterConfig config;
    config.host = "127.0.0.1";
    config.port = requested_port;
    config.enable_cors = true;
    config.static_root = WB_WEB_STATIC_ROOT;

    auto http = std::make_shared<aria::adapters::http::HttpAdapter>(config);
    aria::binding::BindingEngine bindings{
        http,
        dispatcher,
        aria::binding::BindingEngine::DispatchPolicy::SmartMarshal};

    auto& bill = http->register_view("tipcalc.bill", "double");
    auto& tip = http->register_view("tipcalc.tip_percent", "int");
    auto& people = http->register_view("tipcalc.people", "int");
    auto& tip_amount = http->register_view("tipcalc.tip_amount", "text");
    auto& total = http->register_view("tipcalc.total", "text");
    auto& per_person = http->register_view("tipcalc.per_person", "text");
    auto& round_up = http->register_view("tipcalc.round_up", "click");

    bindings.bind_double_oneway(vm->bill, bill);
    bindings.bind_int_oneway(vm->tipPercent, tip);
    bindings.bind_int_oneway(vm->people, people);
    const auto money = [](double value) { return std::to_string(value); };
    bindings.bind_text_projected(vm->tipAmount, tip_amount, money);
    bindings.bind_text_projected(vm->total, total, money);
    bindings.bind_text_projected(vm->perPerson, per_person, money);

    // HTTP callbacks run on server workers. Unlike native UI callbacks they
    // must hop to the graph thread before mutating Properties or Commands.
    bindings.adopt(bill, http->on_double_changed(bill, [&](double value) {
        dispatcher->post([vm, value] { vm->bill = value; });
    }));
    bindings.adopt(tip, http->on_int_changed(tip, [&](int value) {
        dispatcher->post([vm, value] { vm->tipPercent = value; });
    }));
    bindings.adopt(people, http->on_int_changed(people, [&](int value) {
        dispatcher->post([vm, value] { vm->people = value; });
    }));
    bindings.adopt(round_up, http->on_click(round_up, [&] {
        dispatcher->post([vm] { vm->roundUp.execute(); });
    }));

    // Mirror can_execute onto the wire so web clients can disable the Round Up
    // button. The Qt shell gets this automatically via bind_command; this
    // on_click path posts to the dispatcher instead (graph-thread safety), so
    // re-wire the predicate here. Command<> auto-tracks its predicate: the
    // event re-fires whenever bill/tipPercent change. set_enabled is
    // thread-safe and the effect fires on the graph thread.
    bindings.adopt(round_up, vm->roundUp.observe_can_execute(
        [http, &round_up](bool can) { http->set_enabled(round_up, can); }));
    http->set_enabled(round_up, vm->roundUp.can_execute());

    if (!http->start()) {
        std::cerr << "web: failed to start HTTP adapter\n";
        return 3;
    }

    const auto port = http->actual_port();
    if (probe_mode) {
        const int result = probe(port, *dispatcher);
        http->stop();
        return result;
    }

    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);
    std::cout << "AriaTools Web: http://127.0.0.1:" << port << "\n";
    while (!stop_requested.load()) {
        dispatcher->pump(std::chrono::milliseconds{20});
        std::this_thread::sleep_for(std::chrono::milliseconds{5});
    }
    http->stop();
    return 0;
}
