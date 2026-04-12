#ifndef __EditBoxIMPLIOS_H__
#define __EditBoxIMPLIOS_H__

#include "UI/GUIEditBoxImpl.h"
#import "BMEditBox.h"

namespace LORD
{
    class GUIEditBox;
    
    class EditBoxImplIOS : public GUIEditBoxImpl
    {
    public:
        /**
         * @js NA
         */
        EditBoxImplIOS(GUIEditBox* pEditText);
        /**
         * @js NA
         * @lua NA
         */
        virtual ~EditBoxImplIOS();
        
        virtual bool initWithSize(const Vector2& size);
        virtual void setFont(const char* pFontName, int fontSize);
        virtual void setFontColor(const Color& color);
        virtual void setPlaceholderFont(const char* pFontName, int fontSize);
        virtual void setPlaceholderFontColor(const Color& color);
        virtual void setInputMode(InputMode inputMode);
        virtual void setInputFlag(InputFlag inputFlag);
        virtual void setMaxLength(int maxLength);
        virtual int  getMaxLength();
        virtual void setReturnType(KeyboardReturnType returnType);
        virtual bool isEditing();
        
        virtual void setText(const char* pText);
        virtual const char* getText(void);
        virtual void refreshInactiveText();
        virtual void setPlaceHolder(const char* pText);
        virtual void setPosition(const Vector2& pos);
        virtual void setVisible(bool visible);
        virtual void setContentSize(const Vector2& size);
        virtual void setAnchorPoint(const Vector2& anchorPoint);
        virtual void updatePosition(float dt) override;
        /**
         * @js NA
         * @lua NA
         */
        virtual void visit(void);
        /**
         * @js NA
         * @lua NA
         */
        virtual void onEnter(void);
        virtual void doAnimationWhenKeyboardMove(float duration, float distance);
        virtual void openKeyboard();
        virtual void closeKeyboard();
        
        virtual void onEndEditing();
        
    private:
        Vector2          _contentSize;
        Vector2         _position;
        Vector2         _anchorPoint;
        BMEditBox   *_bmEditBox;
        int             _maxTextLength;
        bool            _inRetinaMode;
    };

}

#endif /* __CCEditBoxIMPLIOS_H__ */

