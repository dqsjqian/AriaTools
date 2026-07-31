#import "AppDelegate.h"

@implementation AppDelegate

- (BOOL)application:(UIApplication*)application
    didFinishLaunchingWithOptions:(NSDictionary*)launchOptions {
    NSLog(@"[Workbench] didFinishLaunching");
    return YES;
}

- (UISceneConfiguration*)application:(UIApplication*)application
    configurationForConnectingSceneSession:(UISceneSession*)connectingSceneSession
                                   options:(UISceneConnectionOptions*)options {
    return [[UISceneConfiguration alloc]
            initWithName:@"Default Configuration"
             sessionRole:connectingSceneSession.role];
}

@end
