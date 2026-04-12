
#ifndef _SHELL_INTERFACE_IOS_H_
#define _SHELL_INTERFACE_IOS_H_

#import "ShellInterface.h"
#import "ShellInterfaceIOSBase.h"
#import "Game.h"

namespace GameClient {
    
    class ShellInterfaceIOS : public ShellInterfaceIOSBase
    {
    public:
        ShellInterfaceIOS();
        
        ~ShellInterfaceIOS();
        
        virtual void	payWithInfo(const LORD::GUIString& payInfo);
        
        virtual LORD::GUIString getPlatformID() {return "test";}
        
        virtual void    onShareToWeiXin(const LORD::GUIString& info);
        
        void* m_WebViwer;
    };
}

#endif // _SHELL_INTERFACE_IOS_H_

