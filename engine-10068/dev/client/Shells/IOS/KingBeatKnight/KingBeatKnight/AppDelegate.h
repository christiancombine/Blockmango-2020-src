//
//  AppDelegate.h
//  KingBeatKnight
//
//  Created by zhangyi on 14-6-4.
//
//

#import <UIKit/UIKit.h>

@class BaseRootViewController;

@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow* window;
@property (retain, nonatomic) BaseRootViewController*  viewController;

@end
