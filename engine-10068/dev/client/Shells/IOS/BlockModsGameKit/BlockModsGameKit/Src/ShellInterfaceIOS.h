
#ifndef _SHELL_INTERFACE_IOS_H_
#define _SHELL_INTERFACE_IOS_H_

#import <Foundation/Foundation.h>
#import "ShellInterface.h"

namespace GameClient {
    
    class ShellInterfaceIOS : public ShellInterface
    {
    public:
        ShellInterfaceIOS();
        
        ~ShellInterfaceIOS();
        
        virtual void gameExit(LORD::String params);
    };
}


#endif // _SHELL_INTERFACE_IOS_H_

