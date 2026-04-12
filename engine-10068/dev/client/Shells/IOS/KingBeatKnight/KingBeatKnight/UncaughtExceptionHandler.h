	//
//  UncaughtExceptionHandler.h
//  KingBeatKnight
//
//  Created by zhangyi on 15-4-18.
//
//

void InstallUncaughtExceptionHandler(void);

#ifndef KingBeatKnight_UncaughtExceptionHandler_h
#define KingBeatKnight_UncaughtExceptionHandler_h

@interface UncaughtExceptionHandler : NSObject{
    BOOL dismissed;
}

@end

void HandleException(NSException *exception);
void SignalHandler(int signal);

#endif
