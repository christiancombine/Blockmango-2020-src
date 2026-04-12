//
//  BMEnum.h
//  BlockModsGameKit
//
//  Created by KiBen on 2018/1/2.
//  Copyright © 2018年 SandboxOL. All rights reserved.
//

#ifndef BMEnum_h
#define BMEnum_h

#define IOS_MAX_TOUCHES_COUNT 10

typedef NS_ENUM(NSUInteger, BMTouchState) {
    BMTouchStateBegan,
    BMTouchStateMoved,
    BMTouchStateEnded,
    BMTouchStateCancelled
};

#endif /* BMEnum_h */
