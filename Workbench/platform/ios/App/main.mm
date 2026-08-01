#import <UIKit/UIKit.h>
#import "AppDelegate.h"

// iOS entry point. UIApplicationMain creates the UIApplication and AppDelegate,
// reads the SceneManifest from Info.plist, and enters the event loop.
int main(int argc, char* argv[]) {
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil,
                                 NSStringFromClass([AppDelegate class]));
    }
}
