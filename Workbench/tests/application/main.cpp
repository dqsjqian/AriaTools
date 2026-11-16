#include "app/AppCore.h"
#include "viewmodels/CartVm.h"
#include "viewmodels/DashboardVm.h"
#include "viewmodels/LoginVm.h"
#include "viewmodels/SettingsVm.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
int checks = 0, failures = 0;
void check(bool pass, const char* label) {
    ++checks;
    if (!pass) { ++failures; std::cerr << "FAIL: " << label << '\n'; }
}
template<class Vm>
std::shared_ptr<Vm> primary(wb::core::AppCore& core, const char* id) {
    for (const auto& entry : core.modules())
        if (entry.id == id) return std::dynamic_pointer_cast<Vm>(entry.vm);
    return nullptr;
}
int main(int argc, char** argv) {
    if (argc != 3) return 2;
    const fs::path fixture(argv[2]);
    fs::create_directories(fixture);
    const auto resources = fixture / "i18n";
    fs::copy(argv[1], resources, fs::copy_options::recursive | fs::copy_options::overwrite_existing);

    // Every shipped XML file is parsed, including keys not used at startup.
    auto locale = wb::i18n::make_context(resources.string());
    std::size_t resources_checked = 0;
    for (const auto& entry : fs::recursive_directory_iterator(resources)) {
        if (!entry.is_regular_file() || entry.path().extension() != ".xml") continue;
        const auto stem = entry.path().stem().string();
        const auto language = stem == "strings" ? "zh-CN" : stem.substr(8);
        const auto result = locale->lookup_exact(language, entry.path().parent_path().filename().string(), "__parse_all__");
        if (!result) std::cerr << result.error().path << ": " << result.error().message << '\n';
        check(result.has_value(), "shipped XML parses with framework catalog");
        ++resources_checked;
    }
    check(resources_checked >= WB_EXPECTED_MODULES, "all module resources were staged");

    const auto broken_xml = resources / "notes/strings.xml";
    std::ifstream original_stream(broken_xml);
    const std::string original((std::istreambuf_iterator<char>(original_stream)), {});
    std::ofstream(broken_xml) << "<resources><string name=\"title\">broken";
    aria::async::MainThreadExecutor ui;
    ui.drain();
    {
        wb::core::AppCore core(resources.string());
        const auto data = (fixture / "data").string();
        core.services().storage().set_data_dir(data);
        core.services().settings().sync().dataDir = data;
        core.set_ui_executor(&ui);
        bool threw = false;
        try { core.load_modules(); } catch (const std::runtime_error&) { threw = true; }
        check(threw, "malformed resource fails startup with a diagnostic");
        check(core.modules().empty(), "failed startup rolls back page entries");
        check(!core.mounts().provided("dashboard.content"), "failed startup revokes slot contributions");
        check(!core.create_module_vm("cart"), "failed startup revokes page factories");
        std::ofstream(broken_xml) << original;
        check(core.i18n().reload().has_value(), "repaired resources reload");
        core.load_modules();
        check(core.modules().size() == WB_EXPECTED_MODULES, "startup can retry without duplicate modules");
        auto cart = primary<wb::cart::CartVm>(core, "cart");
        auto dashboard = primary<wb::dashboard::DashboardVm>(core, "dashboard");
        check(cart && dashboard, "manifest provides typed primary VMs");
        check(dashboard && dashboard->mountedVm.get() == cart,
              "dashboard startup mount shares cached primary cart");
        check(core.create_module_vm("cart") != cart, "fresh navigation VM stays independent of primary cache");
        core.load_modules();
        check(primary<wb::cart::CartVm>(core, "cart") == cart, "repeated startup preserves primary state");
        core.mounts().SetEnabled("dashboard.content", false);
        check(dashboard->mountedVm.get() == nullptr, "external slot disable refreshes host automatically");
        core.mounts().SetEnabled("dashboard.content", true);
        check(dashboard->mountedVm.get() == cart, "external slot enable restores primary VM");
        const auto chinese = dashboard->welcome.get();
        core.i18n().set_language("en");
        check(!chinese.empty() && dashboard->welcome.get() != chinese,
              "real inactive module text refreshes on language change");
        auto settings = primary<wb::settings::SettingsVm>(core, "settings");
        if (settings) settings->activate();
        check(settings && settings->language.get() == "en", "settings activation observes the injected locale");

        // Finish the worker operation on UI before translating its result.
        auto login = primary<wb::login::LoginVm>(core, "login");
        core.i18n().set_language("zh-CN");
        login->login.execute("alice", "");
        core.i18n().set_language("en");
        check(ui.pump_until([&] { return !login->login.is_executing.get(); }, std::chrono::seconds(6)),
              "login worker completes through UI executor");
        check(login->login.last_error_message.get() == core.i18n().tr("login", "err_password_empty"),
              "async error uses the completion-time locale");
    }
    ui.drain();
    std::cout << checks << " checks, " << failures << " failures; " << resources_checked << " XML resources\n";
    return failures ? 1 : 0;
}
