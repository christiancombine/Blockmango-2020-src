//
//  BMCacheResourceManager.m
//  BlockWorld
//
//  Created by KiBen on 2017/11/30.
//

#import "BMCacheResourceManager.h"
#import "BMFileManager.h"

@implementation BMCacheResourceManager

+ (BOOL)isResourceExists {
    // src path
    NSString *srcResourcePath = [BMFileManager resourceSourceDirectory];
    
    // dest path
    NSString *desResoucePath = [BMFileManager resourceCacheDirectory];
    
    return [BMFileManager directoryExistsAtPath:desResoucePath];
}

+ (void)copyResourceToCacheDirectory {
    
    NSString *destPath = [BMFileManager resourceCacheDirectory];
    NSString *srcPath = [BMFileManager resourceSourceDirectory];
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSError *error = nil;
    [fileManager createDirectoryAtPath:destPath withIntermediateDirectories:NO attributes:nil error:&error];
    NSArray *fileList = [fileManager contentsOfDirectoryAtPath:srcPath error:&error];
    
    for(NSString *srcFileName in fileList) {
        NSString *destFile = [destPath stringByAppendingPathComponent:srcFileName];
        NSString *srcFile = [srcPath stringByAppendingPathComponent:srcFileName];
        [fileManager copyItemAtPath:srcFile toPath:destFile error:&error];
        NSLog(@"file copy success: %@",srcFile);
    }
}

+ (void)clearResouceCache {
    NSString *cacheResourceDir = [BMFileManager resourceCacheDirectory];
    NSFileManager *fileManager = [NSFileManager defaultManager];
    [fileManager removeItemAtPath:cacheResourceDir error:nil];
    NSLog(@"clear cache: %@", cacheResourceDir);
}
@end
