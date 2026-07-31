#import <UIKit/UIKit.h>
#import "AppDelegate.h"

// iOS 入口。UIApplicationMain 创建 UIApplication、AppDelegate，
// 读取 Info.plist 的 SceneManifest，进入事件循环。
int main(int argc, char* argv[]) {
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil,
                                 NSStringFromClass([AppDelegate class]));
    }
}
