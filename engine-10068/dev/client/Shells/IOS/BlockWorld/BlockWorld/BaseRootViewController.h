//
//  RootViewController.h
//  UIDemo
//
//  Created by Perfect on 13-8-20.
//  Copyright (c) 2013年 zhangyi. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import "LoadingViewController.h"

#import "game.h"
#import "IOSSoundEngine.h"
#import "downloadFileEngine.h"

#define SYSTEM_GREATER_THAN_SIX ([[[UIDevice currentDevice] systemVersion] floatValue] >= 6.0)

@class GameLoadingView;
@interface BaseRootViewController : GLKViewController

@property (nonatomic, strong) UITextField* textField;
@property (nonatomic, strong)  downloadFileEngine* downloadEngine;
@property (nonatomic, strong)  NSString* downloadurl;

- (void)initialize;
- (void)destroy;

@end
