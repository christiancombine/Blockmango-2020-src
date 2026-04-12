//
//  BMEditBox.m
//  BlockWorld
//
//  Created by KiBen on 2017/12/4.
//

#import "BMEditBox.h"
#import "EditBoxImplIOS.h"

@interface BMEditBox () <UITextFieldDelegate>
@property (nonatomic, weak) UITextField *inputTextField;
@end

@implementation BMEditBox

- (instancetype)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        self.backgroundColor = [UIColor clearColor];
        
        UITextField *inputTextField = [[UITextField alloc] initWithFrame:self.bounds];
        inputTextField.delegate = self;
        inputTextField.borderStyle = UITextBorderStyleRoundedRect;
        inputTextField.clearButtonMode = UITextFieldViewModeWhileEditing;
        inputTextField.returnKeyType = UIReturnKeyDone;
        [self addSubview:inputTextField];
        _inputTextField = inputTextField;
    }
    return self;
}

- (NSString *)editText {
    return _inputTextField.text ? : @"";
}

- (BOOL)isEditing {
    return _inputTextField.isEditing;
}

- (void)becameResponder {
    [_inputTextField becomeFirstResponder];
}

- (void)resignResponder {
    [_inputTextField resignFirstResponder];
}

- (BOOL)textFieldShouldEndEditing:(UITextField *)textField {
    NSLog(@"%s", __PRETTY_FUNCTION__);
    if (_editBoxImpl) {
        ((LORD::EditBoxImplIOS *)_editBoxImpl)->onEndEditing();
    }
    return YES;
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    NSLog(@"%s", __PRETTY_FUNCTION__);
    self.hidden = YES;
    [textField resignFirstResponder];
    return YES;
}
@end
