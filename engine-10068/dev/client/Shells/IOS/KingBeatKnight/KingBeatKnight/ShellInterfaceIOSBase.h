
#ifndef _SHELL_INTERFACE_IOS_BASE_H_
#define _SHELL_INTERFACE_IOS_BASE_H_

#import "ShellInterface.h"
#import "Game.h"

namespace GameClient {
    
    class ShellInterfaceIOSBase : public ShellInterface
    {
    public:
        ShellInterfaceIOSBase();
        
        ~ShellInterfaceIOSBase();

        virtual bool	isLoginFromSdk() { return false; }
        
        virtual void	login();
        
        virtual void	logout();
        
        virtual void	enterUserCenter();
        
        virtual void	enterAppBBS();
        
        virtual void	payWithInfo(const LORD::GUIString& payInfo);
        
        virtual void	onUpdatePushInfo(const LORD::GUIString& pushInfo);
        
        virtual void	onClearPushInfo();
        
        virtual LORD::GUIString getDeviceInfo();
        
    };
}

#endif // _SHELL_INTERFACE_IOS_BASE_H_

