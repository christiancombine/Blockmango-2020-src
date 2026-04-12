#import "ShellInterfaceIOS.h"
#import "WebView.h"

namespace GameClient {
 
    ShellInterfaceIOS::ShellInterfaceIOS()
    {
        m_WebViwer = NULL;
    }
    
    ShellInterfaceIOS::~ShellInterfaceIOS()
    {
        
    }
     
    void ShellInterfaceIOS::payWithInfo(const LORD::GUIString& payInfo)
    {
        if(m_WebViwer != NULL)
        {
            NSLog(@"webViwer is opend close it");
            _WebViewPlugin_Destroy(m_WebViwer);
            m_WebViwer = NULL;
        }
        m_WebViwer = _WebViewPlugin_Init("ShellInterfaceIOS");
        //_WebViewPlugin_LoadURL(m_WebViwer, "http://www.baidu.com/");
        LORD::GUIString payUrl = "http://www.baidu.com/";
        payUrl += payInfo;
        _WebViewPlugin_LoadURL(m_WebViwer, payUrl.c_str());
        _WebViewPlugin_SetVisibility(m_WebViwer, true);
    }
    
    void ShellInterfaceIOS::onShareToWeiXin(const LORD::GUIString& info)
    {
    }
}

