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
    // 释放 window → RootVC → IosShell（含 AppCore / VM）依次析构。
    _shell.reset();
    self.window = nil;
}

@end
