#include "infra/i18n/I18n.h"
#include "module_api/BaseVm.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
namespace fs = std::filesystem;
int failures=0, checks=0;
void check(bool ok, const char* label) {
    ++checks;
    if (!ok) { ++failures; std::cerr << "FAIL " << label << '\n'; }
}
void write(const fs::path& path, const std::string& data) {
    fs::create_directories(path.parent_path());
    std::ofstream(path) << "<resources>" << data << "</resources>";
}
struct TestVm : wb::core::BaseVm {
    aria::Property<std::string> title;
    explicit TestVm(wb::i18n::Context context, int& updates) : BaseVm(std::move(context)) {
        localize([this, &updates] {
            ++updates;
            title.set(wb::i18n::str_in(i18n(), "alpha", "title"));
        });
    }
};
int main(int argc, char** argv) {
    if (argc != 2) return 2;
    const fs::path root(argv[1]);
    fs::create_directories(root);
    write(root/"alpha/strings.xml", R"(<string name="title">Alpha 中文</string><string name="priority">模块默认</string><string name="empty"></string><string name="entities">A &amp; B &lt; C &gt; D &quot;Q&quot; &apos;S&apos;</string>)");
    write(root/"alpha/strings_en.xml", R"(<string name="title">Alpha English</string>)");
    write(root/"common/strings.xml", R"(<string name="shared">公共中文</string><string name="common_default">默认公共</string>)");
    write(root/"common/strings_en.xml", R"(<string name="shared">Shared English</string><string name="priority">Common English</string>)");
    write(root/"other/strings_fr.xml", R"(<string name="foreign">Forbidden</string>)");
    auto context = wb::i18n::make_context(root.string());
    auto& service = *context;
    check(wb::i18n::str_in(service, "alpha", "title") == "Alpha 中文", "default module string");
    check(wb::i18n::str_in(service, "alpha", "shared") == "公共中文", "default common fallback");
    check(wb::i18n::str_in(service, "alpha", "missing") == "[alpha/missing]", "missing placeholder");
    check(wb::i18n::str_in(service, "alpha", "foreign") == "[alpha/foreign]", "other modules are not searched");
    check(wb::i18n::str_in(service, "alpha", "empty").empty(), "empty text is a valid hit");
    check(service.tr("alpha", "entities") == "A & B < C > D \"Q\" 'S'", "XML named entities");
    check(wb::i18n::detail::module_of("/work/modules/alpha/viewmodels/Vm.cpp") == "alpha", "POSIX module inference");
    check(wb::i18n::detail::module_of(R"(C:\work\modules\alpha\Vm.cpp)") == "alpha", "Windows module inference");
    auto langs=service.available_languages();
    check(std::find(langs.begin(), langs.end(), "zh-CN") != langs.end(), "default language listed");
    check(std::find(langs.begin(), langs.end(), "en") != langs.end(), "English language discovered");
    check(std::find(langs.begin(), langs.end(), "fr") != langs.end(), "new language discovered");
    int updates=0, events=0;
    auto sub=service.on_language_changed([&](const auto&) { ++events; });
    {
        TestVm vm(context, updates);
        check(vm.title.get() == "Alpha 中文", "initial localization");
        service.set_language("en");
        check(vm.title.get() == "Alpha English", "inactive VM refreshes on language change");
        check(wb::i18n::str_in(service, "alpha", "priority") == "模块默认", "module default precedes common current");
        check(wb::i18n::str_in(service, "alpha", "shared") == "Shared English", "common current-language fallback");
        check(wb::i18n::str_in(service, "alpha", "common_default") == "默认公共", "common default-language fallback");
        check(wb::i18n::str_in(service, "alpha", "title", wb::i18n::Lang::ZhCN) == "Alpha 中文", "explicit language lookup");
        check(service.language() == "en", "explicit lookup does not switch locale");
        service.set_language("en");
        check(updates == 2 && events == 1, "same language does not notify");
    }
    service.set_language("zh-CN");
    check(updates == 2 && events == 2, "VM destruction releases localization listener");
    check(wb::i18n::detail::module_of("/work/notmodules/alpha/Vm.cpp").empty(),
          "module inference requires a complete path component");
    auto second = wb::i18n::make_context(root.string(), "en");
    int second_updates = 0;
    auto second_vm = std::make_unique<TestVm>(second, second_updates);
    check(service.language() == "zh-CN" && second_vm->title.get() == "Alpha English",
          "two contexts retain independent locales");
    context.reset();
    second->set_language("zh-CN");
    check(second_vm->title.get() == "Alpha 中文" && second_updates == 2,
          "destroying one context leaves the other context usable");
    write(root/"alpha/strings.xml", R"(<string name="title">Changed 中文</string>)");
    check(second->reload().has_value(), "catalog reload succeeds");
    check(second_vm->title.get() == "Changed 中文", "VM refreshes after catalog reload");
    second_vm.reset();
    second->set_language("en");
    check(second_updates == 3, "destroyed VM releases catalog and locale bindings");
    std::cout << checks << " checks, " << failures << " failures\n";
    return failures ? 1 : 0;
}
