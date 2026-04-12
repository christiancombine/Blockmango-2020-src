
@interface downloadFileEngine : NSObject <NSURLConnectionDataDelegate> {
    long long mFileSize;
}

@property (nonatomic, strong) NSMutableData* data;

-(void) downloadFile:(NSString*) srcpath;

@end
