#include "EditBoxImplIOS.h"

#define kLabelZOrder  9999

#include "UI/GUIEditBox.h"
#import "BMConstString.h"

namespace LORD
{
    
    GUIEditBoxImpl* __createSystemEditBox(GUIEditBox* pEditBox) {
        return new EditBoxImplIOS(pEditBox);
    }
    
    void __destroySystemEditBox(GUIEditBoxImpl* pEditBoxImpl) {
        if (pEditBoxImpl) {
            delete pEditBoxImpl;
        }
    }
    
    EditBoxImplIOS::EditBoxImplIOS(GUIEditBox* pEditText)
    : GUIEditBoxImpl(pEditText)
    , _anchorPoint(Vector2(0.5f, 0.5f))
    , _maxTextLength(-1)
    {
        _bmEditBox = [[BMEditBox alloc] initWithFrame:CGRectMake(0, 3, [UIScreen mainScreen].bounds.size.width - 80, 30)];
        _bmEditBox.editBoxImpl = this;
    }
    
    EditBoxImplIOS::~EditBoxImplIOS() {
        
    }
    
    void EditBoxImplIOS::doAnimationWhenKeyboardMove(float duration, float distance) {
        
    }
    
    bool EditBoxImplIOS::initWithSize(const Vector2& size) {
        return true;
    }
    
    void EditBoxImplIOS::setFont(const char* pFontName, int fontSize) {
        
    }
    
    void EditBoxImplIOS::setFontColor(const Color& color) {
        
    }
    
    void EditBoxImplIOS::setPlaceholderFont(const char* pFontName, int fontSize) {
        // TODO need to be implemented.
    }
    
    void EditBoxImplIOS::setPlaceholderFontColor(const Color& color) {
        
    }
    
    void EditBoxImplIOS::setInputMode(InputMode inputMode) {
    
    }
    
    void EditBoxImplIOS::setMaxLength(int maxLength) {
        _bmEditBox.editTextMaxLength = maxLength;
    }
    
    int EditBoxImplIOS::getMaxLength() {
        return _bmEditBox.editTextMaxLength;
    }
    
    void EditBoxImplIOS::setInputFlag(InputFlag inputFlag) {
        
    }
    
    void EditBoxImplIOS::setReturnType(KeyboardReturnType returnType) {

    }
    
    bool EditBoxImplIOS::isEditing() {
        return [_bmEditBox isEditing];
    }
    
    void EditBoxImplIOS::refreshInactiveText() {
        
    }
    
    void EditBoxImplIOS::setText(const char* text) {
        
    }
    
    NSString* _removeSiriString(NSString* str) {
        NSString* siriString = @"\xef\xbf\xbc";
        return [str stringByReplacingOccurrencesOfString:siriString withString:@""];
    }
    
    const char*  EditBoxImplIOS::getText(void) {
        return [_removeSiriString(_bmEditBox.editText) UTF8String];
    }
    
    void EditBoxImplIOS::setPlaceHolder(const char* pText) {
        
    }
    
    void EditBoxImplIOS::setPosition(const Vector2& pos) {

    }
    
    void EditBoxImplIOS::setVisible(bool visible) {
        
    }
    
    void EditBoxImplIOS::setContentSize(const Vector2& size) {
        
    }
    
    void EditBoxImplIOS::setAnchorPoint(const Vector2& anchorPoint) {
        
    }
    
    void EditBoxImplIOS::visit(void) {
        
    }
    
    void EditBoxImplIOS::onEnter(void) {
        
    }
    
    void EditBoxImplIOS::updatePosition(float dt) {
        
    }
    
    void EditBoxImplIOS::openKeyboard() {
        [[NSNotificationCenter defaultCenter] postNotificationName:BMStringShowEditBoxNotification object:_bmEditBox];
    }
    
    void EditBoxImplIOS::closeKeyboard() {
        [[NSNotificationCenter defaultCenter] postNotificationName:BMStringDismissEditBoxNotification object:_bmEditBox];
    }
    
    void EditBoxImplIOS::onEndEditing() {
        _editBox->OnInputText(getText());
    }
    
}


