//
//  BMDecorationViewController.m
//  BlockModsGameKit
//
//  Created by KiBen on 2018/1/2.
//  Copyright © 2018年 SandboxOL. All rights reserved.
//

#import "BMDecorationViewController.h"
#import "Core.h"
#import "Src/game.h"
#import "DecorationShellInterfaceIOS.h"
#import "BMFileManager.h"
#import "BMCacheResourceManager.h"
#import "BMEnum.h"
#import "BMConstString.h"
#import <OpenGLES/ES1/glext.h>

@interface BMDecorationViewController () {
    CGFloat                 m_touchScale;
    BOOL                    m_bInitGame;
    BOOL                    m_bPause;
    ClothesPlug::CGame *m_pGame;
    
}

@property (nonatomic, strong) EAGLContext *context;
@property (nonatomic, copy) NSArray<NSString *> *defaultRequiredDecorationIDs;
@end

@implementation BMDecorationViewController

using namespace LORD;

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.view.backgroundColor = [UIColor whiteColor];
    self.view.multipleTouchEnabled = YES;
    self.preferredFramesPerSecond = 60;
    // 初始化流程
    m_bInitGame = false;
    m_bPause = false;
    self.defaultRequiredDecorationIDs = @[@"clothes_tops", @"clothes_pants", @"custom_shoes", @"custom_hair", @"custom_face"];
    
    _context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    if(!_context)
        return;
    
    if (![EAGLContext setCurrentContext:_context])
        return;
    
    GLKView *view = (GLKView *)self.view;
    view.context = _context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
    [self _initializeGameAndShellInterface];
    [self _checkResource];
}

- (void)dealloc {
    
    m_bPause = YES;
    m_pGame->DestroyGame();
    
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

- (void)changeGender:(int)gender {
    if (m_pGame && m_bInitGame && !m_bPause) {
        m_pGame->changeSex(gender);
    }
}

- (void)useDecorationWithResourceID:(NSString *)resourceID {
    if (m_pGame && m_bInitGame && !m_bPause && resourceID) {
        NSArray *compoments = [resourceID componentsSeparatedByString:@"."];
        m_pGame->changeParts([compoments.firstObject UTF8String], [compoments.lastObject UTF8String]);
    }
}

- (void)unuseDecorationWithResourceID:(NSString *)resourceID {
    if (m_pGame && m_bInitGame && !m_bPause && resourceID) {
        NSString *ID = [resourceID componentsSeparatedByString:@"."].firstObject;
        if ([self.defaultRequiredDecorationIDs containsObject:ID]) {
            m_pGame->changeParts([ID UTF8String], "1");
        }else {
            m_pGame->changeParts([ID UTF8String], "0");
        }
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

#pragma mark -GLKViewDelegate
- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect {
    if (m_bInitGame && !m_bPause) {
        // 游戏主循环
        m_pGame->MainTick();
    }
}

#pragma mark - Start Game
- (void)_startGame {
    
    dispatch_time_t time = dispatch_time(DISPATCH_TIME_NOW, (int64_t)100 * NSEC_PER_MSEC);
    
    dispatch_after(time, dispatch_get_main_queue(), ^{
        
        CGRect screenBounds = [[UIScreen mainScreen] bounds];
        
        const char* strRootPath = [[[BMFileManager resourceCacheDirectory] stringByAppendingString:@"/"] UTF8String];
        const char* strDocPath = [[[BMFileManager documentDirectory] stringByAppendingString:@"/"] UTF8String];
        
        float boundHeight = screenBounds.size.height;
        
        int width = 0;
        int height = 0;
        glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &width);
        glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &height);
        
        m_touchScale = MIN(2, boundHeight / height);
        m_pGame->initEngine(strRootPath, strDocPath, width, height, 0);
        
        m_bInitGame = true;
        
        __weak typeof(self) weakSelf = self;
        if (self.generateFinished) {
            self.generateFinished(weakSelf);
        }
    });
}

- (void)_destroy {
    //    m_pSoundEngine->end();
    
    ClothesPlug::ShellInterface* shellInterface =  m_pGame->getShellInterface();
    LordSafeDelete(shellInterface);
    m_pGame->setShellInterface(NULL);
    
    LordSafeDelete(m_pGame);
}

- (void)_initializeGameAndShellInterface {
    m_pGame = LordNew ClothesPlug::CGame;
    
    ClothesPlug::DecorationShellInterfaceIOS *shellInterface = LordNew ClothesPlug::DecorationShellInterfaceIOS;
    m_pGame->setShellInterface(shellInterface);
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
@end

