//
//  BMGameViewController.m
//  BlockModsGameKit
//
//  Created by KiBen on 2017/12/4.
//  Copyright © 2017年 SandboxOL. All rights reserved.
//

#import "BMGameViewController.h"
#import "GameLoadingView.h"
#import "Core.h"
#import "BMFileManager.h"
#import <OpenGLES/ES1/glext.h>
#import "ShellInterfaceIOS.h"
#import "BMCacheResourceManager.h"
#import "game.h"
#import "IOSSoundEngine.h"
#import "BMConstString.h"
#import "BMEditBox.h"
#import "BMEnum.h"

@interface BMGameViewController ()
@property (nonatomic, strong) EAGLContext* context;
@property (nonatomic, weak) GameLoadingView *loadingView;
@property (nonatomic, weak) BMEditBox *editBox;
@end


@implementation BMGameViewController {
    CGFloat                 m_touchScale;
    BOOL                    m_bInitGame;
    BOOL                    m_bPause;
    GameClient::CGame*      m_pGame;
    LORD::IOSSoundEngine*   m_pSoundEngine;
    
}

using namespace LORD;

#pragma mark - view 相关重载接口
- (BOOL)shouldAutorotate {
    return YES;
}

- (NSUInteger)supportedInterfaceOrientations {
    return UIInterfaceOrientationMaskLandscape;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.view.backgroundColor = [UIColor whiteColor];
    self.view.multipleTouchEnabled = YES;
    // 初始化流程
    m_bInitGame = false;
    m_bPause = false;
    
    _context = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES2];
    self.preferredFramesPerSecond = 40;
    if(!self.context)
        return;
    
    if (![EAGLContext setCurrentContext:_context])
        return;
    
    GLKView *view = (GLKView *)self.view;
    view.context = _context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
    //loading View
    GameLoadingView *loadingView = [[GameLoadingView alloc] initWithFrame:self.view.bounds];
    [self.view addSubview:loadingView];
    _loadingView = loadingView;
    
    [self _initializeGameAndSoundEngine];
    [self _initializeShellInterface];
    [self _checkResource];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_willEnterForegroundHandler) name:UIApplicationWillEnterForegroundNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_didEnterBackgroundHandler) name:UIApplicationDidEnterBackgroundNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_willTerminateHandler) name:UIApplicationWillTerminateNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_gameDidExitHandler) name:BMStringDidGameExitNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_showEditBox:) name:BMStringShowEditBoxNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_dismissEditBox:) name:BMStringDismissEditBoxNotification object:nil];
}

- (void)viewDidLayoutSubviews {
    [super viewDidLayoutSubviews];

    _loadingView.frame = self.view.bounds;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    
    [self _destroy];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    
    if ([self isViewLoaded] && [self view].window == nil) {
        self.view = nil;
        
        if ([EAGLContext currentContext] == self.context) {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }
}

#pragma mark -GLKViewDelegate
- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect {
    if (m_bInitGame && !m_bPause) {
        // 游戏主循环
        m_pGame->MainTick();
    }
}

#pragma mark -touches event
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    [super touchesBegan:touches withEvent:event];
    
    [self _handleTouches:touches withState:BMTouchStateBegan];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    [super touchesMoved:touches withEvent:event];
    
    [self _handleTouches:touches withState:BMTouchStateMoved];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    [super touchesEnded:touches withEvent:event];
    NSLog(@"%s", __FUNCTION__);
    [self _handleTouches:touches withState:BMTouchStateEnded];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
    [super touchesCancelled:touches withEvent:event];
    
    [self _handleTouches:touches withState:BMTouchStateCancelled];
}

- (void)motionBegan:(UIEventSubtype)motion withEvent:(UIEvent *)event {
    if (m_pGame) {
        m_pGame->handleMotionBegin();
    }
}

- (void)motionCancelled:(UIEventSubtype)motion withEvent:(UIEvent *)event {
    if (m_pGame) {
        m_pGame->handleMotionCancelled();
    }
}

- (void)motionEnded:(UIEventSubtype)motion withEvent:(UIEvent *)event {
    if (m_pGame) {
        m_pGame->handleMotionEnded();
    }
}

#pragma mark - Private Methods

#pragma mark - gameclient init
- (void)_initializeGameAndSoundEngine {
    m_pGame = LordNew GameClient::CGame;
    m_pSoundEngine = LordNew IOSSoundEngine;
}

- (void)_initializeShellInterface {
    GameClient::ShellInterfaceIOS *shellInterface = LordNew GameClient::ShellInterfaceIOS;
    m_pGame->setShellInterface(shellInterface);
    m_pGame->setPlatformInfo("test");
}

- (void)_destroy {
//    m_pSoundEngine->end();
    LordSafeDelete(m_pSoundEngine);
    
    GameClient::ShellInterface* shellInterface =  m_pGame->getShellInterface();
    LordSafeDelete(shellInterface);
    m_pGame->setShellInterface(NULL);
    
    LordSafeDelete(m_pGame);
}

#pragma mark - Start Game
- (void)_startGame {
    // 启动游戏
    [self.loadingView updateProgressText:@"Starting....."];
    
    dispatch_time_t time = dispatch_time(DISPATCH_TIME_NOW, (int64_t)100 * NSEC_PER_MSEC);
    
    dispatch_after(time, dispatch_get_main_queue(), ^{
        
        CGRect screenBounds = [[UIScreen mainScreen] bounds];
        
        // 初始化游戏
        NSString *cachesDir = [BMFileManager cacheDirectory];
        NSString *docDir = [BMFileManager documentDirectory];
        
        ////新的路径可读写。目录为cache/BBZS目录
        const char* strRootPath = [[cachesDir stringByAppendingString:@"/BlockMod/"] UTF8String];
        const char* strDocPath = [[docDir stringByAppendingString:@"/"] UTF8String];
        
        float boundWidth = screenBounds.size.width;
        float boundHeight = screenBounds.size.height;
        if (boundWidth < boundHeight) {
            Math::Swap(boundWidth, boundHeight);
        }
        
        int width = 0;
        int height = 0;
        glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &width);
        glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &height);
        
        if (width < height) {
            Math::Swap(width, height);
        }
        
        m_touchScale = height / boundHeight;
        NSArray *addrComponents = [_gameAddr componentsSeparatedByString:@":"];
        const char *gameIP = [addrComponents.firstObject UTF8String];
        int port = [addrComponents.lastObject intValue];
        
//        m_pGame->InitGame(strRootPath, strDocPath, width, height, 0, m_pSoundEngine);
        m_pGame->InitGame([_nickName UTF8String], _userID.longLongValue, _userToken.UTF8String, gameIP, port, [_gameTimestamp longLongValue], _language.UTF8String, _gameType.UTF8String, _mapName.UTF8String, _mapUrl.UTF8String, strRootPath, strDocPath, width, height, 0, 0, m_pSoundEngine);
        
        [_loadingView removeFromSuperview];
        
        // animation
        CATransition *animation = [CATransition animation];
        animation.duration = 2.5;
        animation.timingFunction = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseIn];;
        animation.type = @"rippleEffect";
        [[self.view layer] addAnimation:animation forKey:@"animation"];
        // animation end
        
//        [_loadingView removeFromSuperview];
        
//        [UIView beginAnimations:nil context:nil];
//        [UIView setAnimationDuration:1];
//        [UIView setAnimationTransition:UIViewAnimationTransitionFlipFromLeft forView:self.view cache:FALSE];
//        [UIView commitAnimations];

        m_bInitGame = true;
    });
}

#pragma mark - Check Resource
- (void)_checkResource {
    // 1. 检查是否需要拷贝资源
    BOOL isExsits = [BMCacheResourceManager isResourceExists];
    BOOL isGreater = false;
    NSString *cacheVersion = [[NSUserDefaults standardUserDefaults] objectForKey:BMStringGameResourceVersion];
    NSData *data = [[NSData alloc] initWithContentsOfFile:[BMCacheResourceManager resourceVersionFilePath]];
    NSDictionary *resVersionDict = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingAllowFragments error:nil];
    if (!cacheVersion) {
        isGreater = YES;
    }else {
        isGreater = [cacheVersion compare:resVersionDict[@"version"] options:NSNumericSearch] == NSOrderedAscending;
    }
    [[NSUserDefaults standardUserDefaults] setObject:resVersionDict[@"version"] forKey:BMStringGameResourceVersion];
    [[NSUserDefaults standardUserDefaults] synchronize];
    
    if (isExsits && !isGreater) {
        [self _startGame];
        return;
    }
    
    [_loadingView startLoading];
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        [BMCacheResourceManager clearResouceCache]; // 清掉原先资源
        [BMCacheResourceManager copyResourceToCacheDirectory]; // 拷贝所需资源
        dispatch_async(dispatch_get_main_queue(), ^{
            [self _startGame];
        });
    });
}

#pragma mark - Touches Handler
- (void)_handleTouches:(NSSet *)touches withState:(BMTouchState)state {
    if (!m_bInitGame || !m_pGame) {
        return;
    }
    
    UITouch *ids[IOS_MAX_TOUCHES_COUNT] = {0};
    float xs[IOS_MAX_TOUCHES_COUNT] = {0.0f};
    float ys[IOS_MAX_TOUCHES_COUNT] = {0.0f};
    
    int i = 0;
    for (UITouch *touch in touches) {
        ids[i] = touch;
        xs[i] = [touch locationInView: [touch view]].x * m_touchScale;
        ys[i] = [touch locationInView: [touch view]].y * m_touchScale;
        ++i;
    }
    
    switch (state) {
        case BMTouchStateBegan:
            m_pGame->handleTouchesBegin(i,(intptr_t*)(void *)ids, xs, ys);
            break;
        case BMTouchStateMoved:
            m_pGame->handleTouchesMove(i,(intptr_t*)(void *)ids, xs, ys);
            break;
        case BMTouchStateEnded:
            m_pGame->handleTouchesEnd(i,(intptr_t*)(void *)ids, xs, ys);
            break;
        case BMTouchStateCancelled:
            m_pGame->handleTouchesCancel(i,(intptr_t*)(void *)ids, xs, ys);
            break;
    }
}

#pragma mark - Application Period Notification Handler
- (void)_willEnterForegroundHandler {
    self.paused = false;
    m_bPause = false;
    if (m_bInitGame && m_pGame) {
        m_pGame->OnResume();
    }
}

- (void)_didEnterBackgroundHandler {
    self.paused = YES;
    m_bPause = YES;
    if (m_bInitGame && m_pGame) {
        m_pGame->OnPause();
    }
}

- (void)_willTerminateHandler {
    self.paused = YES;
    m_bPause = YES;
    if (m_bInitGame && m_pGame) {
        m_pGame->OnPause();
    }
}

- (void)_gameDidExitHandler {
    self.paused = YES;
    m_bPause = YES;
    [self dismissViewControllerAnimated:YES completion:^{
        if (_bmDelegate && [_bmDelegate respondsToSelector:@selector(gameViewControllerdidDismissed:)]) {
            [_bmDelegate gameViewControllerdidDismissed:self];
        }
        m_pGame->DestroyGame();
    }];
}

- (void)_showEditBox:(NSNotification *)notification {
    if (!_editBox) {
        [self.view addSubview:notification.object];
        _editBox = notification.object;
    }
    _editBox.hidden = NO;
    [_editBox becameResponder];
}

- (void)_dismissEditBox:(NSNotification *)notification {
    if (_editBox) {
        _editBox.hidden = YES;
        [_editBox resignResponder];
    }
}
@end
