//
//  RootViewController.m
//  UIDemo
//
//  Created by Perfect on 13-8-20.
//  Copyright (c) 2013年 zhangyi. All rights reserved.
//

#import "AppDelegate.h"
#import "RootViewController.h"
#import "ShellInterfaceIOS.h"

using namespace LORD;

@implementation RootViewController

- (void) initShellInterface
{
    GameClient::ShellInterfaceIOS* shellInterface = LordNew GameClient::ShellInterfaceIOS;
    m_pGame->setShellInterface(shellInterface);
    m_pGame->setPlatformInfo("test");
}

- (void) destroyShellInterface
{
    GameClient::ShellInterface* shellInterface =  m_pGame->getShellInterface();
    LordSafeDelete(shellInterface);
    
    m_pGame->setShellInterface(NULL);
}

- (void) registerSDK
{
    // regester notification provided by WanMeiUniversalPlatform
}

#pragma mark -

- (void)login
{
    
}

// show user center view
- (void)showInfo
{
   
}

// show discuss view
- (void)showDiscussPage
{
   
}

- (void)showCustomerService
{
    
}

// buy some thing
- (void)buyAK
{
    
}

- (void)logout
{
   
}


- (void)didReceiveInitSucceedNotification:(NSNotification *)notification
{
//    self.loginView.hidden = NO;
//    self.gameView.hidden = YES;
    
    [super doGameInit];
}

- (void)didReceiveInitFailedNotification:(NSNotification *)notification
{
    
}

- (void)didReceiveLoginSucceedNotification:(NSNotification *)notification
{
   
}

- (void)didReceiveLoginFailedNotification:(NSNotification *)notification
{
    
}

- (void)didReceivePaymentSucceedNotification:(NSNotification *)notification
{
   
}

- (void)didReceivePaymentFailedNotification:(NSNotification *)notification
{
  
}

- (void)didReceiveLogoutSucceedNotification:(NSNotification *)notification
{

}

- (void)didReceiveClosePageNotification:(NSNotification *)notification
{
    
}

- (void)didReceiveLeaveGameHelpNotification:(NSNotification *)notification
{
    NSLog(@"+++++++++++++++ leave Game Help");
}

- (void)didReceiveLeaveSettingNotification:(NSNotification *)notification
{
    NSLog(@"+++++++++++++++ leave Setting Page");
}

- (void)didReceiveUserPortraitChangedNotification:(NSNotification *)notification
{
    NSLog(@"+++++++++++++++ Upload User Portrait");
}

- (void)didReceiveUserInfoChangedNotification:(NSNotification *)notification
{
    NSLog(@"+++++++++++++++ Change User Info");
}

- (void)didReceiveUserImproveAccountNotification:(NSNotification *)notification
{
    NSLog(@"+++++++++++++++ Improve User Account");
}

- (void)didReceiveKeyboardNeedDismissNotification:(NSNotification *)notification
{
    NSLog(@"+++++++++++++++ keyboard need hide");
}


@end
