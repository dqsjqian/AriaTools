#import "App/IosShell.h"
#import "App/IosViewManifest.h"
#import "support/UIViewFactory.h"

#include <string>

namespace wb::ios {

namespace {
// i18n resources are distributed with the app bundle: <Bundle>/i18n/.
std::string bundle_i18n_dir() {
    NSString* res = [[NSBundle mainBundle] resourcePath];
    if (!res) return "./i18n";
    return std::string([[res stringByAppendingPathComponent:@"i18n"] UTF8String]);
}

// Initial language follows the device language (the XmlI18nService maps
// "en" -> strings_en.xml; anything else falls back to strings.xml, the
// default Chinese bundle). Previously hard-coded "zh-CN" made the whole
// UI stay Chinese even on an English device.
std::string initial_lang() {
    NSString* lang = [NSLocale preferredLanguages].firstObject;
    if (!lang) return "zh-CN";
    NSString* tag = [lang componentsSeparatedByString:@"-"].firstObject;
    if ([tag isEqualToString:@"en"]) return "en";
    if ([tag isEqualToString:@"zh"]) return "zh-CN";
    return "en";
}
}  // namespace

IosShell::IosShell()
    : adapter_(std::make_shared<aria::adapters::uikit::UIKitAdapter>()),
      // UIKit callbacks are already on the main thread, but AsyncCommand still
      // needs a MainQueueExecutor to schedule the final co_await schedule_on(ui)
      // — otherwise the InlineExecutor fallback trips the graph thread-affinity
      // check the moment any module creates an AsyncCommand (e.g. login).
      be_(adapter_),
      core_(bundle_i18n_dir(), initial_lang())
{
    // Inject the main-queue executor + delay BEFORE load_modules(): see
    // AppCore::AppCore() comment — "load_modules() is called by the platform
    // shell AFTER set_ui_executor / set_timer". Without these, login/login
    // AsyncCommand throws std::invalid_argument at construction time and the
    // app crashes to a blank screen.
    core_.set_ui_executor(&ui_exec_);
    core_.set_timer(&delay_);
    core_.load_modules();
    register_all_views();
}

IosShell::~IosShell() = default;

UIViewController* IosShell::build_root() {
    auto* tab = [[UITabBarController alloc] init];
    NSMutableArray<UIViewController*>* vcs = [NSMutableArray array];

    const auto& mods = core_.modules();
    for (const auto& m : mods) {
        UIViewController* vc =
            UIViewFactory::instance().build(m.id, *m.vm, be_);
        NSString* t = [NSString stringWithUTF8String:core_.nav_title(m.navKey).c_str()];
        vc.title = t;
        vc.tabBarItem = [[UITabBarItem alloc] initWithTitle:t image:nil tag:0];
        [vcs addObject:vc];
    }
    tab.viewControllers = vcs;

    if (!mods.empty() && mods.front().vm) mods.front().vm->activate();
    return tab;
}

}  // namespace wb::ios
