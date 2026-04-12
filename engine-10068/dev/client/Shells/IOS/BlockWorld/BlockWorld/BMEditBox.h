//
//  BMEditBox.h
//  BlockWorld
//
//  Created by KiBen on 2017/12/4.
//

#import <UIKit/UIKit.h>

@interface BMEditBox : UIView

@property (nonatomic, readonly) NSString *editText;

@property (nonatomic, assign) NSUInteger editTextMaxLength;

@property (nonatomic, assign) void *editBoxImpl;

- (BOOL)isEditing;

- (void)becameResponder;

- (void)resignResponder;
@end
