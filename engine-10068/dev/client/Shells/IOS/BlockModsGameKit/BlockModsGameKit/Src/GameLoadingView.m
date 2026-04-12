//
//  GameLoadingView.m
//  KingBeatKnight
//
//  Created by KiBen on 2017/11/30.
//

#import "GameLoadingView.h"

@interface GameLoadingView ()
@property (nonatomic, assign) CGFloat loadingProgress;

@property (nonatomic, weak) UIImageView *loadingImageView;
@property (nonatomic, weak) UILabel *progressLabel;
@property (nonatomic, weak) UILabel *versionLabel;
@property (nonatomic, weak) CAShapeLayer *firstRectLayer;
@property (nonatomic, weak) CAShapeLayer *secondRectLayer;
@property (nonatomic, weak) NSTimer *progressTimer;
@end

@implementation GameLoadingView

- (instancetype)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        
        self.backgroundColor = [UIColor colorWithRed:206 / 255.0 green:191 / 255.0 blue:158 / 255.0 alpha:1.0];
        
        CAShapeLayer *firstRectLayer = [self _rectLayerWithWidth:self.bounds.size.width * 0.5 color:[UIColor colorWithRed:179 / 255.0 green:164 / 255.0 blue:135 / 255.0 alpha:1.0]];
        [self.layer addSublayer:firstRectLayer];
        _firstRectLayer = firstRectLayer;
        
        CAShapeLayer *secondRectLayer = [self _rectLayerWithWidth:self.bounds.size.width * 0.45 color:[UIColor colorWithRed:88 / 255.0 green:73 / 255.0 blue:54 / 255.0 alpha:1.0]];
        [self.layer addSublayer:secondRectLayer];
        _secondRectLayer = secondRectLayer;
        
        UIImageView *loadingImageView = [[UIImageView alloc] initWithFrame:CGRectMake(0, 0, 29, 29)];
        loadingImageView.center = self.center;
        loadingImageView.image = [UIImage imageNamed:@"BlockModsGameBundle.bundle/loading_1"];
        loadingImageView.animationImages = @[[UIImage imageNamed:@"BlockModsGameBundle.bundle/loading_1"],
                                                                       [UIImage imageNamed:@"BlockModsGameBundle.bundle/loading_2"],
                                                                       [UIImage imageNamed:@"BlockModsGameBundle.bundle/loading_3"],
                                                                       [UIImage imageNamed:@"BlockModsGameBundle.bundle/loading_4"]];
        [self addSubview:loadingImageView];
        loadingImageView.animationDuration = 0.65;
        [loadingImageView startAnimating];
        _loadingImageView = loadingImageView;
        
        UILabel *progressLabel = [[UILabel alloc] init];
        progressLabel.font = [UIFont systemFontOfSize:15];
        progressLabel.backgroundColor = [UIColor clearColor];
        progressLabel.numberOfLines = 2;
        progressLabel.textColor = [UIColor whiteColor];
        progressLabel.textAlignment = NSTextAlignmentRight;
        [self addSubview:progressLabel];
        _progressLabel = progressLabel;
    }
    return self;
}

- (void)layoutSubviews {
    [super layoutSubviews];
    
    NSLog(@"----------------%@", self);
    _loadingImageView.center = self.center;
    _firstRectLayer.position = self.center;
    _secondRectLayer.position = self.center;
    _progressLabel.center = CGPointMake(self.center.x, self.center.y + 40);
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
    [_progressLabel sizeToFit];
    
//    _progressLabel.frame = CGRectMake(boundSize.width - textSize.width - 10, boundSize.height - textSize.height - 10, textSize.width, textSize.height);
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
    NSString *tips = [NSString stringWithFormat:@"Loading...", _loadingProgress];
    [self updateProgressText:tips];
}

- (CAShapeLayer *)_rectLayerWithWidth:(CGFloat)width color:(UIColor *)color {
    UIBezierPath *rectPath = [UIBezierPath bezierPathWithRect:CGRectMake(0, 0, width, width)];
    CAShapeLayer *rectLayer = [CAShapeLayer layer];
    rectLayer.frame = CGRectMake(0, 0, width, width);
    rectLayer.position = self.center;
    rectLayer.path = rectPath.CGPath;
    rectLayer.fillColor = color.CGColor;
    rectLayer.transform = CATransform3DMakeRotation(M_PI_4, 0, 0, 1);
    return rectLayer;
}
@end
