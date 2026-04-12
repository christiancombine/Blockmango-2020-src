//
//  BMGameViewController.h
//  BlockModsGameKit
//
//  Created by KiBen on 2017/12/4.
//  Copyright © 2017年 SandboxOL. All rights reserved.
//

#import <GLKit/GLKit.h>

@class BMGameViewController;

@protocol BMGameViewControllerDelegate <NSObject>
@optional
- (void)gameViewControllerdidDismissed:(BMGameViewController *)controller;
@end

@interface BMGameViewController : GLKViewController

@property (nonatomic, copy) NSNumber *userID;
@property (nonatomic, copy) NSString *nickName;
@property (nonatomic, copy) NSString *userToken;
@property (nonatomic, copy) NSString *gameAddr;
@property (nonatomic, copy) NSString *gameType;
@property (nonatomic, assign) NSNumber *gameTimestamp;
@property (nonatomic, copy) NSString *language;
@property (nonatomic, copy) NSString *mapName;
@property (nonatomic, copy) NSString *mapUrl;

@property (nonatomic, weak) id<BMGameViewControllerDelegate> bmDelegate;

- (instancetype)initWithCoder:(NSCoder *)aDecoder NS_UNAVAILABLE;

- (instancetype)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil NS_UNAVAILABLE;
@end


