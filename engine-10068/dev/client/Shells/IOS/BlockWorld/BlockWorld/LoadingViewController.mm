//
//  RootViewController.m
//  UIDemo
//
//  Created by Perfect on 14-4-11.
//  Copyright (c) 2014年 zhangyi. All rights reserved.
//

#import "LoadingViewController.h"

NSString * const LoadingViewUpdateVersionNotification = @"LoadingViewUpdateVersion";
NSString * const LoadingViewUpdateCopyProgressNotification = @"LoadingViewUpdateCopyProgress";
NSString * const LoadingViewUpdateDownloadProgressNotification = @"LoadingViewUpdateDownloadProgress";
NSString * const LoadingViewUpdatePakProgressNotification = @"LoadingViewUpdatePakProgress";

@implementation LoadingViewController

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
	    
	UIImage *image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"loading" ofType:@"png"]];
	UIImageView *imgView = [[UIImageView alloc] initWithFrame:self.view.bounds];
    imgView.autoresizingMask = UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleWidth;
	imgView.image = image;
    imgView.tag = 1;
	[self.view addSubview:imgView];
    

//    image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"loading2" ofType:@"png"]];
//	imgView = [[UIImageView alloc] initWithFrame:self.view.bounds];
//    imgView.autoresizingMask = UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleWidth;
//	imgView.image = image;
//    imgView.tag = 2;
//	[self.view addSubview:imgView];
//    
//    [imgView release];
//    
//    image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"loading3" ofType:@"png"]];
//	imgView = [[UIImageView alloc] initWithFrame:self.view.bounds];
//    imgView.autoresizingMask = UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleWidth;
//	imgView.image = image;
//    imgView.tag = 3;
//	[self.view addSubview:imgView];
//    
//    [imgView release];
//    
//    image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"loading4" ofType:@"png"]];
//	imgView = [[UIImageView alloc] initWithFrame:self.view.bounds];
//    imgView.autoresizingMask = UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleWidth;
//	imgView.image = image;
//    imgView.tag = 4;
//	[self.view addSubview:imgView];
//    
//    [imgView release];
    
	_progressTextView = [[UILabel alloc] init];
    
	_progressTextView.text = @"准备资源(不消耗流量)0.00%";
	_progressTextView.font = [UIFont systemFontOfSize:15];
	_progressTextView.backgroundColor = [UIColor clearColor];
	_progressTextView.numberOfLines = 2;
    _progressTextView.textColor = [UIColor whiteColor];
    _progressTextView.tag = 5;
    
    [_progressTextView setTextAlignment:NSTextAlignmentRight];
	[self.view addSubview:_progressTextView];
    
    // version view
    _versionTextView = [[UILabel alloc] init];
    _versionTextView.text = @"";
    _versionTextView.font = [UIFont systemFontOfSize:15];
    _versionTextView.backgroundColor = [UIColor clearColor];
    _versionTextView.numberOfLines = 2;
    _versionTextView.textColor = [UIColor whiteColor];
    _versionTextView.tag = 6;
    
    [self.view addSubview:_versionTextView];
    
    [self setVersionText:@"当前版本: \n最新版本:"];
    
    m_copyProgress = 0.0f;
    
    _progressTimer = [NSTimer scheduledTimerWithTimeInterval: 0.3
                                             target: self
                                           selector: @selector(handleCopyProgressTimer:)
                                           userInfo: nil
                                            repeats: YES];
    
    
    _activityIndicatorView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];

    CGRect indicatorFrame =  _activityIndicatorView.frame;
    CGSize boundSize = self.view.bounds.size;
    
    if (boundSize.height > boundSize.width) {
        CGFloat temp = boundSize.height;
        boundSize.height = boundSize.width;
        boundSize.width = temp;
    }
    
    _activityIndicatorView.frame = CGRectMake((boundSize.width - indicatorFrame.size.width)*0.5f, (boundSize.height - indicatorFrame.size.height)*0.5f, indicatorFrame.size.width, indicatorFrame.size.height);
    
    [self.view addSubview:_activityIndicatorView];
    
    [_activityIndicatorView startAnimating];
}

- (void) setVersionText:(NSString *)text
{
    _versionTextView.text = text;
    
    CGSize size =  [_versionTextView.text sizeWithFont:_versionTextView.font constrainedToSize:CGSizeMake(600, 2000)];
    _versionTextView.frame = CGRectMake(10, 10, size.width, size.height);
}

- (void) setProgressText:(NSString *)text
{
    _progressTextView.text = text;
    
    CGSize size =  [_progressTextView.text sizeWithFont:_progressTextView.font constrainedToSize:CGSizeMake(320, 2000)];
    CGSize boundSize = self.view.bounds.size;
    
    if (boundSize.height > boundSize.width) {
        CGFloat temp = boundSize.height;
        boundSize.height = boundSize.width;
        boundSize.width = temp;
    }
    _progressTextView.frame = CGRectMake(boundSize.width - size.width - 10, boundSize.height - size.height - 10, size.width, size.height);
    
}

- (void) handleCopyProgressTimer: (NSTimer *) timer
{
    int value = arc4random() % 8;
    
    m_copyProgress += value;
    
    if (m_copyProgress >= 97) {
        m_copyProgress = 97;
    }
    
    NSString* tips = [NSString stringWithFormat:@"准备资源中(不消耗流量)%.2f%%", m_copyProgress];
    
    [self setProgressText:tips];
}

//- (void) handleTimer: (NSTimer *) timer
//{
//    //在这里进行处理
//    
//    static int nIndex = 1;
//    
//    CATransition *animation = [CATransition animation];
//    animation.delegate = self;
//    animation.duration = 1.0;
//    animation.timingFunction = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseIn];;
//    animation.type = kCATransitionFade;
//    
//    UIView* imageView = [self.view viewWithTag:nIndex];
//    [self.view bringSubviewToFront:imageView];
//    UIView* label = [self.view viewWithTag:5];
//    [self.view bringSubviewToFront:label];
//    [UIView setAnimationTransition:(UIViewAnimationTransitionFlipFromLeft) forView:self.view cache:FALSE];
//    
//    [[self.view layer] addAnimation:animation forKey:@"animation"];
//    
//    ++nIndex;
//    if (nIndex>4) {
//        nIndex = 1;
//    }
//}

// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return UIInterfaceOrientationIsLandscape( interfaceOrientation );
}

#ifdef __IPHONE_6_0
- (BOOL)shouldAutorotate {
    return YES;
}

- (NSUInteger)supportedInterfaceOrientations {
    return UIInterfaceOrientationMaskLandscape;
}
#endif


- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
    

    _progressTextView = nil;
    

    _versionTextView = nil;
    
}


- (void)dealloc {

    
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

@end
