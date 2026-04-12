//
//  GameLoadingView.h
//  BlockWorld
//
//  Created by KiBen on 2013/11/30.
//

#import <UIKit/UIKit.h>

@interface GameLoadingView : UIView
- (void)startLoading;

- (void)updateProgressText:(NSString *)progress;

- (void)setVersionText:(NSString*) text;
@end
