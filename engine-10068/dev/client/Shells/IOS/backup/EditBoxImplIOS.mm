#include "EditBoxImplIOS.h"

#define kLabelZOrder  9999

#include "UI/GUIEditBox.h"
#import "AppDelegate.h"
#import "BaseRootViewController.h"

#define getEditBoxImplIOS() ((LORD::EditBoxImplIOS*)editBox_)

@implementation EditBoxImplIOS_objc

@synthesize textField = textField_;
@synthesize editState = editState_;
@synthesize editBox = editBox_;

- (void)dealloc
{
    [textField_ resignFirstResponder];
    [textField_ removeFromSuperview];
    self.textField = NULL;
}

-(id) initWithEditBox:(void *)editBox
{
    self = [super init];
    if (self)
    {
        CGRect screenBounds = [[UIScreen mainScreen] bounds];
        
        self.textField = [[UITextField alloc]initWithFrame:CGRectMake(40, 0, screenBounds.size.height -80, 35)];
        
        textField_.backgroundColor = [UIColor whiteColor];
        //textField_.clearsOnBeginEditing = YES;
        textField_.delegate = self;
        [textField_ addTarget:self action:@selector(textChanged) forControlEvents:UIControlEventEditingChanged];
        textField_.autocapitalizationType = UITextAutocapitalizationTypeNone;
        textField_.autocorrectionType = UITextAutocorrectionTypeYes;
        textField_.contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
        textField_.borderStyle = UITextBorderStyleRoundedRect;
        textField_.clearButtonMode = UITextFieldViewModeAlways;
        
        self.editBox = editBox;
    }
    
    return self;
}

//-(id) initWithFrame: (CGRect) frameRect editBox: (void*) editBox
//{
//    self = [super init];
//    
//    if (self)
//    {
//        editState_ = NO;
//        self.textField = [[[UITextField alloc] initWithFrame: frameRect] autorelease];
//
//        [textField_ setTextColor:[UIColor whiteColor]];
//        textField_.font = [UIFont systemFontOfSize:frameRect.size.height*2/3]; //TODO need to delete hard code here.
//		textField_.contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
//        textField_.backgroundColor = [UIColor clearColor];
//        textField_.borderStyle = UITextBorderStyleNone;
//        textField_.delegate = self;
//        textField_.hidden = true;
//		textField_.returnKeyType = UIReturnKeyDefault;
//        [textField_ addTarget:self action:@selector(textChanged) forControlEvents:UIControlEventEditingChanged];
//        self.editBox = editBox;
//    }
//    
//    return self;
//}
//
//-(void) setPosition:(CGPoint) pos
//{
//    CGRect frame = [textField_ frame];
//    frame.origin = pos;
//    [textField_ setFrame:frame];
//}
//
//-(void) setContentSize:(CGSize) size
//{
//    CGRect frame = [textField_ frame];
//    frame.size = size;
//    [textField_ setFrame:frame];
//}

-(void) visit
{
    
}

-(void) openKeyboard
{
    BaseRootViewController* rootViewController = ((AppDelegate*)[UIApplication sharedApplication].delegate).viewController;

    [rootViewController.view addSubview:textField_];
    [textField_ becomeFirstResponder];
}

-(void) closeKeyboard
{
    [textField_ resignFirstResponder];
    [textField_ removeFromSuperview];
}

- (BOOL)textFieldShouldReturn:(UITextField *)sender
{
    if (sender == textField_) {
        [sender resignFirstResponder];
    }
    return NO;
}

-(void)animationSelector
{
//    auto view = cocos2d::Director::getInstance()->getOpenGLView();
//    CCEAGLView *eaglview = (CCEAGLView *) view->getEAGLView();
//
//    [eaglview doAnimationWhenAnotherEditBeClicked];
}

- (BOOL)textFieldShouldBeginEditing:(UITextField *)sender        // return NO to disallow editing.
{
//    CCLOG("textFieldShouldBeginEditing...");
    editState_ = YES;

//    auto view = cocos2d::Director::getInstance()->getOpenGLView();
//    CCEAGLView *eaglview = (CCEAGLView *) view->getEAGLView();

//    RootViewController* rootViewController = ((AppDelegate*)[UIApplication sharedApplication].delegate).viewController;
    
//    if ([eaglview isKeyboardShown])
//    {
//        [self performSelector:@selector(animationSelector) withObject:nil afterDelay:0.0f];
//    }
//    cocos2d::extension::EditBoxDelegate* pDelegate = getEditBoxImplIOS()->getDelegate();
//    if (pDelegate != NULL)
//    {
//        pDelegate->editBoxEditingDidBegin(getEditBoxImplIOS()->getEditBox());
//    }
    
//#if CC_ENABLE_SCRIPT_BINDING
//    cocos2d::extension::EditBox*  pEditBox= getEditBoxImplIOS()->getEditBox();
//    if (NULL != pEditBox && 0 != pEditBox->getScriptEditBoxHandler())
//    {        
//        cocos2d::CommonScriptData data(pEditBox->getScriptEditBoxHandler(), "began",pEditBox);
//        cocos2d::ScriptEvent event(cocos2d::kCommonEvent,(void*)&data);
//        cocos2d::ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(&event);
//    }
//#endif
    return YES;
}

- (BOOL)textFieldShouldEndEditing:(UITextField *)sender
{
//    CCLOG("textFieldShouldEndEditing...");
    editState_ = NO;
    getEditBoxImplIOS()->refreshInactiveText();
    
//    cocos2d::extension::EditBoxDelegate* pDelegate = getEditBoxImplIOS()->getDelegate();
//    if (pDelegate != NULL)
//    {
//        pDelegate->editBoxEditingDidEnd(getEditBoxImplIOS()->getEditBox());
//        pDelegate->editBoxReturn(getEditBoxImplIOS()->getEditBox());
//    }
//    
//#if CC_ENABLE_SCRIPT_BINDING
//    cocos2d::extension::EditBox*  pEditBox= getEditBoxImplIOS()->getEditBox();
//    if (NULL != pEditBox && 0 != pEditBox->getScriptEditBoxHandler())
//    {
//        cocos2d::CommonScriptData data(pEditBox->getScriptEditBoxHandler(), "ended",pEditBox);
//        cocos2d::ScriptEvent event(cocos2d::kCommonEvent,(void*)&data);
//        cocos2d::ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(&event);
//        memset(data.eventName, 0, sizeof(data.eventName));
//        strncpy(data.eventName, "return", sizeof(data.eventName));
//        event.data = (void*)&data;
//        cocos2d::ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(&event);
//    }
//#endif
    
	if(editBox_ != nil)
	{
		getEditBoxImplIOS()->onEndEditing();
	}
    return YES;
}

/**
 * Delegate method called before the text has been changed.
 * @param textField The text field containing the text.
 * @param range The range of characters to be replaced.
 * @param string The replacement string.
 * @return YES if the specified text range should be replaced; otherwise, NO to keep the old text.
 */
- (BOOL)textField:(UITextField *) textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string
{
    if (getEditBoxImplIOS()->getMaxLength() < 0)
    {
        return YES;
    }
    
    NSUInteger oldLength = [textField.text length];
    NSUInteger replacementLength = [string length];
    NSUInteger rangeLength = range.length;
    
    NSUInteger newLength = oldLength - rangeLength + replacementLength;
    
    return newLength <= getEditBoxImplIOS()->getMaxLength();
}

/**
 * Called each time when the text field's text has changed.
 */
- (void) textChanged
{
    // NSLog(@"text is %@", self.textField.text);
//    cocos2d::extension::EditBoxDelegate* pDelegate = getEditBoxImplIOS()->getDelegate();
//    if (pDelegate != NULL)
//    {
//        pDelegate->editBoxTextChanged(getEditBoxImplIOS()->getEditBox(), getEditBoxImplIOS()->getText());
//    }
//    
//#if CC_ENABLE_SCRIPT_BINDING
//    cocos2d::extension::EditBox*  pEditBox= getEditBoxImplIOS()->getEditBox();
//    if (NULL != pEditBox && 0 != pEditBox->getScriptEditBoxHandler())
//    {
//        cocos2d::CommonScriptData data(pEditBox->getScriptEditBoxHandler(), "changed",pEditBox);
//        cocos2d::ScriptEvent event(cocos2d::kCommonEvent,(void*)&data);
//        cocos2d::ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(&event);
//    }
//#endif
}

@end


namespace LORD
{

GUIEditBoxImpl* __createSystemEditBox(GUIEditBox* pEditBox)
{
    return new EditBoxImplIOS(pEditBox);
}
void __destroySystemEditBox(GUIEditBoxImpl* pEditBoxImpl)
{
    if (pEditBoxImpl) {
        delete pEditBoxImpl;
    }
}
    
EditBoxImplIOS::EditBoxImplIOS(GUIEditBox* pEditText)
: GUIEditBoxImpl(pEditText)
//, _label(nullptr)
//, _labelPlaceHolder(nullptr)
, _anchorPoint(Vector2(0.5f, 0.5f))
, _systemControl(NULL)
, _maxTextLength(-1)
{
    _systemControl = [[EditBoxImplIOS_objc alloc] initWithEditBox:this];
}

EditBoxImplIOS::~EditBoxImplIOS()
{
//    [_systemControl release];
}

void EditBoxImplIOS::doAnimationWhenKeyboardMove(float duration, float distance)
{

}

bool EditBoxImplIOS::initWithSize(const Vector2& size)
{
    do 
    {
//        auto glview = cocos2d::Director::getInstance()->getOpenGLView();
//
//        CGRect rect = CGRectMake(0, 0, size.width * glview->getScaleX(),size.height * glview->getScaleY());
//
//        if (_inRetinaMode)
//        {
//            rect.size.width /= 2.0f;
//            rect.size.height /= 2.0f;
//        }
        
        //_systemControl = [[EditBoxImplIOS_objc alloc] initWithEditBox:this];
        //if (!_systemControl) break;
        
		//initInactiveLabels(size);
        //setContentSize(size);
		
        return true;
    }while (0);
    
    return false;
}

//void EditBoxImplIOS::initInactiveLabels(const Size& size)
//{
//	const char* pDefaultFontName = [[_systemControl.textField.font fontName] UTF8String];
//
//	_label = Label::create();
//    _label->setAnchorPoint(Point(0, 0.5f));
//    _label->setColor(Color3B::WHITE);
//    _label->setVisible(false);
//    _editBox->addChild(_label, kLabelZOrder);
//	
//    _labelPlaceHolder = Label::create();
//	// align the text vertically center
//    _labelPlaceHolder->setAnchorPoint(Point(0, 0.5f));
//    _labelPlaceHolder->setColor(Color3B::GRAY);
//    _editBox->addChild(_labelPlaceHolder, kLabelZOrder);
//    
//    setFont(pDefaultFontName, size.height*2/3);
//    setPlaceholderFont(pDefaultFontName, size.height*2/3);
//}

//void EditBoxImplIOS::placeInactiveLabels()
//{
//    _label->setPosition(Point(CC_EDIT_BOX_PADDING, _contentSize.height / 2.0f));
//    _labelPlaceHolder->setPosition(Point(CC_EDIT_BOX_PADDING, _contentSize.height / 2.0f));
//}

//void EditBoxImplIOS::setInactiveText(const char* pText)
//{
//    if(_systemControl.textField.secureTextEntry == YES)
//    {
//        std::string passwordString;
//        for(int i = 0; i < strlen(pText); ++i)
//            passwordString.append("\u25CF");
//        _label->setString(passwordString.c_str());
//    }
//    else
//        _label->setString(getText());
//
//    // Clip the text width to fit to the text box
//    float fMaxWidth = _editBox->getContentSize().width - CC_EDIT_BOX_PADDING * 2;
//    Size labelSize = _label->getContentSize();
//    if(labelSize.width > fMaxWidth) {
//        _label->setDimensions(fMaxWidth,labelSize.height);
//    }
//}

void EditBoxImplIOS::setFont(const char* pFontName, int fontSize)
{
//    bool isValidFontName = true;
//	if(pFontName == NULL || strlen(pFontName) == 0) {
//        isValidFontName = false;
//    }
//
//    float retinaFactor = _inRetinaMode ? 2.0f : 1.0f;
//	NSString * fntName = [NSString stringWithUTF8String:pFontName];
//
//    auto glview = cocos2d::Director::getInstance()->getOpenGLView();
//
//    float scaleFactor = glview->getScaleX();
//    UIFont *textFont = nil;
//    if (isValidFontName) {
//        textFont = [UIFont fontWithName:fntName size:fontSize * scaleFactor / retinaFactor];
//    }
//    
//    if (!isValidFontName || textFont == nil){
//        textFont = [UIFont systemFontOfSize:fontSize * scaleFactor / retinaFactor];
//    }
//
//	if(textFont != nil) {
//		[_systemControl.textField setFont:textFont];
//    }
//
//	_label->setSystemFontName(pFontName);
//	_label->setSystemFontSize(fontSize);
//	_labelPlaceHolder->setSystemFontName(pFontName);
//	_labelPlaceHolder->setSystemFontSize(fontSize);
}

void EditBoxImplIOS::setFontColor(const Color& color)
{
//    _systemControl.textField.textColor = [UIColor colorWithRed:color.r / 255.0f green:color.g / 255.0f blue:color.b / 255.0f alpha:1.0f];
//	_label->setColor(color);
}

void EditBoxImplIOS::setPlaceholderFont(const char* pFontName, int fontSize)
{
	// TODO need to be implemented.
}

void EditBoxImplIOS::setPlaceholderFontColor(const Color& color)
{
	//_labelPlaceHolder->setColor(color);
}

void EditBoxImplIOS::setInputMode(InputMode inputMode)
{
    switch (inputMode)
    {
        case IM_EMAIL_ADDRESS:
            _systemControl.textField.keyboardType = UIKeyboardTypeEmailAddress;
            break;
        case IM_NUMERIC:
            _systemControl.textField.keyboardType = UIKeyboardTypeDecimalPad;
            break;
        case IM_PHONE_NUMBER:
            _systemControl.textField.keyboardType = UIKeyboardTypePhonePad;
            break;
        case IM_URL:
            _systemControl.textField.keyboardType = UIKeyboardTypeURL;
            break;
        case IM_DECIMAL:
            _systemControl.textField.keyboardType = UIKeyboardTypeDecimalPad;
            break;
        case IM_SINGLE_LINE:
            _systemControl.textField.keyboardType = UIKeyboardTypeDefault;
            break;
        default:
            _systemControl.textField.keyboardType = UIKeyboardTypeDefault;
            break;
    }
}

void EditBoxImplIOS::setMaxLength(int maxLength)
{
    _maxTextLength = maxLength;
}

int EditBoxImplIOS::getMaxLength()
{
    return _maxTextLength;
}

void EditBoxImplIOS::setInputFlag(InputFlag inputFlag)
{
    switch (inputFlag)
    {
        case IF_PASSWORD:
            _systemControl.textField.secureTextEntry = YES;
            break;
        case IF_INITIAL_CAPS_WORD:
            _systemControl.textField.autocapitalizationType = UITextAutocapitalizationTypeWords;
            break;
        case IF_INITIAL_CAPS_SENTENCE:
            _systemControl.textField.autocapitalizationType = UITextAutocapitalizationTypeSentences;
            break;
        case IF_INTIAL_CAPS_ALL_CHARACTERS:
            _systemControl.textField.autocapitalizationType = UITextAutocapitalizationTypeAllCharacters;
            break;
        case IF_SENSITIVE:
            _systemControl.textField.autocorrectionType = UITextAutocorrectionTypeNo;
            break;
        default:
            break;
    }
}

void EditBoxImplIOS::setReturnType(KeyboardReturnType returnType)
{
    switch (returnType) {
        case KRT_DEFAULT:
            _systemControl.textField.returnKeyType = UIReturnKeyDefault;
            break;
        case KRT_DONE:
            _systemControl.textField.returnKeyType = UIReturnKeyDone;
            break;
        case KRT_SEND:
            _systemControl.textField.returnKeyType = UIReturnKeySend;
            break;
        case KRT_SEARCH:
            _systemControl.textField.returnKeyType = UIReturnKeySearch;
            break;
        case KRT_GO:
            _systemControl.textField.returnKeyType = UIReturnKeyGo;
            break;
        default:
            _systemControl.textField.returnKeyType = UIReturnKeyDefault;
            break;
    }
}

bool EditBoxImplIOS::isEditing()
{
    return [_systemControl isEditState] ? true : false;
}

void EditBoxImplIOS::refreshInactiveText()
{
//    const char* text = getText();
//    if(_systemControl.textField.hidden == YES)
//    {
//		setInactiveText(text);
//		if(strlen(text) == 0)
//		{
//			_label->setVisible(false);
//			_labelPlaceHolder->setVisible(true);
//		}
//		else
//		{
//			_label->setVisible(true);
//			_labelPlaceHolder->setVisible(false);
//		}
//	}
}

void EditBoxImplIOS::setText(const char* text)
{
    NSString* nsText =[NSString stringWithUTF8String:text];
    if ([nsText compare:_systemControl.textField.text] != NSOrderedSame)
    {
        _systemControl.textField.text = nsText;
    }
    
    refreshInactiveText();
}

NSString* removeSiriString(NSString* str)
{
    NSString* siriString = @"\xef\xbf\xbc";
    return [str stringByReplacingOccurrencesOfString:siriString withString:@""];
}

const char*  EditBoxImplIOS::getText(void)
{
    return [removeSiriString(_systemControl.textField.text) UTF8String];
}

void EditBoxImplIOS::setPlaceHolder(const char* pText)
{
    _systemControl.textField.placeholder = [NSString stringWithUTF8String:pText];
	//_labelPlaceHolder->setString(pText);
}

//static CGPoint convertDesignCoordToScreenCoord(const Point& designCoord, bool bInRetinaMode)
//{
////    auto glview = cocos2d::Director::getInstance()->getOpenGLView();
////    CCEAGLView *eaglview = (CCEAGLView *) glview->getEAGLView();
////
////    float viewH = (float)[eaglview getHeight];
////    
////    Point visiblePos = Point(designCoord.x * glview->getScaleX(), designCoord.y * glview->getScaleY());
////    Point screenGLPos = visiblePos + glview->getViewPortRect().origin;
////    
////    CGPoint screenPos = CGPointMake(screenGLPos.x, viewH - screenGLPos.y);
//    
//    if (bInRetinaMode)
//    {
//        screenPos.x = screenPos.x / 2.0f;
//        screenPos.y = screenPos.y / 2.0f;
//    }
////    CCLOGINFO("[EditBox] pos x = %f, y = %f", screenGLPos.x, screenGLPos.y);
//    return screenPos;
//}

void EditBoxImplIOS::setPosition(const Vector2& pos)
{
	_position = pos;
	//adjustTextFieldPosition();
}

void EditBoxImplIOS::setVisible(bool visible)
{
//    _systemControl.textField.hidden = !visible;
}

void EditBoxImplIOS::setContentSize(const Vector2& size)
{

}

void EditBoxImplIOS::setAnchorPoint(const Vector2& anchorPoint)
{

}

void EditBoxImplIOS::visit(void)
{
}

void EditBoxImplIOS::onEnter(void)
{
//    adjustTextFieldPosition();
//    const char* pText = getText();
//    if (pText) {
//        setInactiveText(pText);
//    }
}

void EditBoxImplIOS::updatePosition(float dt)
{
//    if (nullptr != _systemControl) {
//        this->adjustTextFieldPosition();
//    }
}



//void EditBoxImplIOS::adjustTextFieldPosition()
//{
//	Size contentSize = _editBox->getContentSize();
//	Rectangle rect = Rectangle(0, 0, contentSize.width, contentSize.height);
//    rect = RectApplyAffineTransform(rect, _editBox->nodeToWorldTransform());
//	
//	Point designCoord = Point(rect.origin.x, rect.origin.y + rect.size.height);
//    [_systemControl setPosition:convertDesignCoordToScreenCoord(designCoord, _inRetinaMode)];
//}

void EditBoxImplIOS::openKeyboard()
{
//	_label->setVisible(false);
//	_labelPlaceHolder->setVisible(false);

	_systemControl.textField.hidden = NO;
    [_systemControl openKeyboard];
}

void EditBoxImplIOS::closeKeyboard()
{
    [_systemControl closeKeyboard];
}

void EditBoxImplIOS::onEndEditing()
{
	_systemControl.textField.hidden = YES;
    
    _editBox->OnInputText(getText());
    
//	if(strlen(getText()) == 0)
//	{
//		_label->setVisible(false);
//		_labelPlaceHolder->setVisible(true);
//	}
//	else
//	{
//		_label->setVisible(true);
//		_labelPlaceHolder->setVisible(false);
//		setInactiveText(getText());
//	}
}

}


