//
//  BMFileManager.h
//  BlockWorld
//
//  Created by KiBen on 2017/11/30.
//

#import <Foundation/Foundation.h>

@interface BMFileManager : NSObject
+ (NSString *)cacheDirectory;

+ (NSString *)documentDirectory;

+ (NSString *)resourceSourceDirectory;

+ (NSString *)resourceCacheDirectory;

+ (BOOL)directoryExistsAtPath:(NSString *)path;
@end
