//
//  GameLoadingView.m
//  BlockWorld
//
//  Created by KiBen on 2013/11/30.
//

#import "GameLoadingView.h"

@interface GameLoadingView ()
@property (nonatomic, assign) CGFloat loadingProgress;

@property (nonatomic, weak) UILabel *progressLabel;
@property (nonatomic, weak) UILabel *versionLabel;
@property (nonatomic, weak) UIActivityIndicatorView* activityIndicatorView;
@property (nonatomic, weak) NSTimer *progressTimer;
@end

@implementation GameLoadingView

- (instancetype)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        UIImage *loadingImage = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"loading" ofType:@"png"]];
        UIImageView *loadingImageView = [[UIImageView alloc] initWithFrame:self.bounds];
        loadingImageView.autoresizingMask = UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleWidth;
        loadingImageView.image = loadingImage;
        [self addSubview:loadingImageView];
        
        UILabel *progressLabel = [[UILabel alloc] init];
//        progressLabel.text = @"准备资源(不消耗流量)0.00%";
        progressLabel.font = [UIFont systemFontOfSize:15];
        progressLabel.backgroundColor = [UIColor clearColor];
        progressLabel.numberOfLines = 2;
        progressLabel.textColor = [UIColor whiteColor];
        progressLabel.textAlignment = NSTextAlignmentRight;
        [self addSubview:progressLabel];
        _progressLabel = progressLabel;
        
        // version view
        UILabel *versionLabel = [[UILabel alloc] init];
        versionLabel = [[UILabel alloc] init];
        versionLabel.text = @"";
        versionLabel.font = [UIFont systemFontOfSize:15];
        versionLabel.backgroundColor = [UIColor clearColor];
        versionLabel.numberOfLines = 2;
        versionLabel.textColor = [UIColor whiteColor];
//        [self addSubview:versionLabel];
        _versionLabel = versionLabel;
        
        UIActivityIndicatorView *activityView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
        activityView.center = self.center;
        [self addSubview:activityView];
        _activityIndicatorView = activityView;
        [_activityIndicatorView startAnimating];
        
        _loadingProgress = 0.0f;
        
    }
    return self;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return UIInterfaceOrientationIsLandscape( interfaceOrientation );
}

- (BOOL)shouldAutorotate {
    return YES;
}

- (NSUInteger)supportedInterfaceOrientations {
    return UIInterfaceOrientationMaskLandscape;
}

- (void)updateProgressText:(NSString *)progress
{
    _progressLabel.text = progress;
    
    CGSize boundSize = self.bounds.size;
    CGSize textSize = [progress boundingRectWithSize:CGSizeMake(boundSize.width, CGFLOAT_MAX) options:NSStringDrawingUsesFontLeading | NSStringDrawingUsesLineFragmentOrigin attributes:@{NSFontAttributeName : _progressLabel.font} context:nil].size;
    
    if (boundSize.height > boundSize.width) {
        CGFloat temp = boundSize.height;
        boundSize.height = boundSize.width;
        boundSize.width = temp;
    }
    _progressLabel.frame = CGRectMake(boundSize.width - textSize.width - 10, boundSize.height - textSize.height - 10, textSize.width, textSize.height);
}

- (void)setVersionText:(NSString *)text {
    
}

- (void)startLoading {
    _progressTimer = [NSTimer scheduledTimerWithTimeInterval: 0.3
                                                      target: self
                                                    selector: @selector(handleProgressTimer:)
                                                    userInfo: nil
                                                     repeats: YES];
}

- (void)handleProgressTimer:(NSTimer *)timer {
    int value = arc4random() % 8;
    
    _loadingProgress += value;
    if (_loadingProgress >= 97) {
        _loadingProgress = 97;
        [_progressTimer invalidate];
        _progressTimer = nil;
    }
    NSString *tips = [NSString stringWithFormat:@"准备资源中(不消耗流量)%.2f%%", _loadingProgress];
    [self updateProgressText:tips];
}
@end
