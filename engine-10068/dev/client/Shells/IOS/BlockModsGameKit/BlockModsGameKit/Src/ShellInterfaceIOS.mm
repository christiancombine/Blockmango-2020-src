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
}

