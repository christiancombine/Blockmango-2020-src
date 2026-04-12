//
//  BMFileManager.m
//  BlockWorld
//
//  Created by KiBen on 2017/11/30.
//

#import "BMFileManager.h"

NSString *const kResourceDirectory = @"BlockMod";

@implementation BMFileManager

+ (NSString *)cacheDirectory {
    return NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES).firstObject;
}

+ (NSString *)documentDirectory {
    return NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).firstObject;
}

+ (NSString *)resourceCacheDirectory {
    return [[BMFileManager cacheDirectory] stringByAppendingPathComponent:kResourceDirectory];
}

+ (NSString *)resourceSourceDirectory {
    return [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:kResourceDirectory];
}

+ (BOOL)directoryExistsAtPath:(NSString *)path {
    BOOL isDir;
    BOOL isExists = [[NSFileManager defaultManager] fileExistsAtPath:path isDirectory:&isDir];
    return isExists && isDir;
}
@end
