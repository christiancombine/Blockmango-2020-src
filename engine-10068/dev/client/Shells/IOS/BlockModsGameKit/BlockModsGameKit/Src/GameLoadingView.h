//
//  GameLoadingView.h
//  KingBeatKnight
//
//  Created by KiBen on 2017/11/30.
//

#import <UIKit/UIKit.h>

@interface GameLoadingView : UIView
- (void)startLoading;

- (void)updateProgressText:(NSString *)progress;

- (void)setVersionText:(NSString*) text;
@end
