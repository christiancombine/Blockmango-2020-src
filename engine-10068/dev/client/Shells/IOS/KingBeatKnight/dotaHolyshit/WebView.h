//
//  WebView.h
//  KingBeatKnight
//
//  Created by System Administrator on 4/2/16.
//
//

#ifndef KingBeatKnight_WebView_h
#define KingBeatKnight_WebView_h

extern "C" {
    void *_WebViewPlugin_Init(const char *gameObjectName);
    void _WebViewPlugin_Destroy(void *instance);
    void _WebViewPlugin_SetMargins(void *instance, int left, int top, int right, int bottom);
    void _WebViewPlugin_SetVisibility(void *instance, BOOL visibility);
    void _WebViewPlugin_LoadURL(void *instance, const char *url);
    void _WebViewPlugin_EvaluateJS(void *instance, const char *url);
}

#endif
