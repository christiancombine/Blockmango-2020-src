//
//  BMCacheResourceManager.h
//  KingBeatKnight
//
//  Created by KiBen on 2017/11/30.
//

#import <Foundation/Foundation.h>

@interface BMCacheResourceManager : NSObject

+ (BOOL)isResourceExists;

+ (void)copyResourceToCacheDirectory;

+ (void)clearResouceCache;
@end
