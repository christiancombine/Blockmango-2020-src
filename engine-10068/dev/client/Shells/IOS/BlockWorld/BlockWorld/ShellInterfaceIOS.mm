#import "ShellInterfaceIOS.h"
#import "BMConstString.h"

namespace GameClient {
 
    ShellInterfaceIOS::ShellInterfaceIOS()
    {
        
    }
    
    ShellInterfaceIOS::~ShellInterfaceIOS()
    {
        
    }

    void ShellInterfaceIOS::gameExit(LORD::String params) {
        [[NSNotificationCenter defaultCenter] postNotificationName:BMStringDidGameExitNotification object:nil];
    }
    
    void ShellInterfaceIOS::onUpdatePushInfo(const LORD::GUIString& pushInfo)
    {
        // 取消所有的本地推送
        [[UIApplication sharedApplication] cancelAllLocalNotifications];
        [UIApplication sharedApplication].applicationIconBadgeNumber = 0;
        
        NSString* jsonString = [NSString stringWithCString:pushInfo.c_str() encoding:NSUTF8StringEncoding];
        NSData* jsonData = [jsonString dataUsingEncoding:NSUTF8StringEncoding];
        
        NSDictionary* jsonPushInfo = [NSJSONSerialization JSONObjectWithData:jsonData options:NSJSONReadingMutableLeaves error:nil];
        
        NSEnumerator *enumerator = [jsonPushInfo objectEnumerator];
        id pushData;
        
        while ((pushData = [enumerator nextObject])) {
            /* code that acts on the dictionary’s values */
            NSString* pushID = [pushData objectForKey:@"id"];
            //NSString* timeType = [pushData objectForKey:@"timeType"];
            NSString* repeatType = [pushData objectForKey:@"repeatType"];
            NSString* time = [pushData objectForKey:@"time"];
            NSString* desc = [pushData objectForKey:@"desc"];
            
            // 创建一个本地推送
            UILocalNotification *notification = [[UILocalNotification alloc] init];
            NSDate *pushDate = nil;
            
            pushDate = [NSDate dateWithTimeIntervalSinceNow:[time intValue]];
            
            if ([repeatType isEqualToString:@"day"]) {
                // 设置重复间隔
                notification.repeatInterval = kCFCalendarUnitDay;
            } else {
            }
            
            if (notification != nil && pushDate != nil) {
                // 设置推送时间
                notification.fireDate = pushDate;
                // 设置时区
                notification.timeZone = [NSTimeZone defaultTimeZone];
                // 推送声音
                notification.soundName = UILocalNotificationDefaultSoundName;
                // 推送内容
                notification.alertBody = desc;
                
                //显示在icon上的红色圈中的数子
                notification.applicationIconBadgeNumber = 1;
                
                //设置userinfo 方便在之后需要撤销的时候使用
                NSDictionary *info = [NSDictionary dictionaryWithObject:pushID forKey:@"pushID"];
                notification.userInfo = info;
                //添加推送到UIApplication
                [[UIApplication sharedApplication] scheduleLocalNotification:notification];
            }

            
        }
        
    }
    
    void ShellInterfaceIOS::onClearPushInfo()
    {
        // 取消所有的本地推送
        [[UIApplication sharedApplication] cancelAllLocalNotifications];
        [UIApplication sharedApplication].applicationIconBadgeNumber = 0;
    }
    
}

