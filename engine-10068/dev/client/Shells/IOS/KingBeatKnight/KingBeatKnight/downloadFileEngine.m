#import "downloadFileEngine.h"

@implementation downloadFileEngine

- (id)init {
    self = [super init];
    if (self) {
        // Initialize self.
    
    }
    
    return self;
}

-(void) downloadFile:(NSString*) srcpath
{
    NSURL *url = [NSURL URLWithString: srcpath];
    
    NSURLRequest *request = [[NSURLRequest alloc] initWithURL:url cachePolicy:NSURLRequestReloadIgnoringCacheData timeoutInterval: 100.0];//设置缓存和超时
    
    NSURLConnection *connection = nil;
    
    connection = [[NSURLConnection alloc] initWithRequest:request delegate:self startImmediately:YES];
    
    //初始化connection对象并马上执行，有的初始化方法并不是马上开始执行，而需要调用start方法。
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response

{//该方法在响应connection时调用
    
    NSLog(@"response");
    
    self.data = [[NSMutableData alloc] init];
    
    NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse *)response;
    
    if(httpResponse && [httpResponse respondsToSelector:@selector(allHeaderFields)]){
        
        NSDictionary *httpResponseHeaderFields = [httpResponse allHeaderFields];
        
        mFileSize = [[httpResponseHeaderFields objectForKey:@"Content-Length"] longLongValue];
        
    }//获取文件文件的大小
    
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error

{//出错时调用
    
    NSLog(@"error");
    
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data

{//接受数据，在接受完成之前，该方法重复调用
    
    NSLog(@"receive");
    
    [_data appendData: data];
    
    //_displayLabel.text = [NSString stringWithFormat:(@"%6.1fkb/%6.1fkb"),[_data length]/1024.0,mFileSize/1024.0];
    
    //[_progressView setProgress: [_data length]/(float)mFileSize];
    
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection

{//完成时调用
    
    NSLog(@"Finish");
    
    NSString *filePath = [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0] stringByAppendingPathComponent:@"downloadFile.txt"];
    
    [_data writeToFile:filePath atomically:NO];//将数据写入Documents目录。
    
    NSLog(@"%@",filePath);

    [[NSNotificationCenter defaultCenter]
     postNotificationName:@"FbzzDownloadUpdateUrl" object:nil];
}

@end 
