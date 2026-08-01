#import "SceneDelegate.h"
#import "App/IosShell.h"

#include <memory>

@implementation SceneDelegate {
    std::unique_ptr<wb::ios::IosShell> _shell;
}

- (void)scene:(UIScene*)scene
    willConnectToSession:(UISceneSession*)session
                 options:(UISceneConnectionOptions*)connectionOptions {
    UIWindowScene* windowScene = (UIWindowScene*)scene;
    if (![windowScene isKindOfClass:[UIWindowScene class]]) return;

    self.window = [[UIWindow alloc] initWithWindowScene:windowScene];

    _shell = std::make_unique<wb::ios::IosShell>();
    self.window.rootViewController = _shell->build_root();

    [self.window makeKeyAndVisible];
    NSLog(@"[Workbench][Scene] willConnect — window attached");
}

- (void)sceneDidDisconnect:(UIScene*)scene {
    // Releases window -> RootVC -> IosShell (including AppCore / VM) in destruction order.
    _shell.reset();
    self.window = nil;
}

@end
