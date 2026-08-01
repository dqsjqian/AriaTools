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
}  // namespace

IosShell::IosShell()
    : adapter_(std::make_shared<aria::adapters::uikit::UIKitAdapter>()),
      // UIKit callbacks are already on the main thread, so the Direct strategy suffices (no dispatcher needed).
      be_(adapter_),
      core_(bundle_i18n_dir(), "zh-CN")
{
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
