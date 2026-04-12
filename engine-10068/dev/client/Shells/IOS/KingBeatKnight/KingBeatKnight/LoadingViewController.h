//
//  RootViewController.h
//  UIDemo
//
//  Created by Perfect on 14-4-11.
//  Copyright (c) 2014年 zhangyi. All rights reserved.
//

#import <UIKit/UIKit.h>

extern NSString * const LoadingViewUpdateVersionNotification;
extern NSString * const LoadingViewUpdateCopyProgressNotification;
extern NSString * const LoadingViewUpdateDownloadProgressNotification;
extern NSString * const LoadingViewUpdatePakProgressNotification;

@interface LoadingViewController : UIViewController {
    
    float m_copyProgress;
}

@property(nonatomic, strong) UILabel* progressTextView;
@property(nonatomic, strong) UILabel* versionTextView;
@property(nonatomic, strong) UIActivityIndicatorView* activityIndicatorView;
@property(nonatomic, strong) NSTimer* progressTimer;

- (void) setProgressText:(NSString*) text;

- (void) setVersionText:(NSString*) text;

@end

