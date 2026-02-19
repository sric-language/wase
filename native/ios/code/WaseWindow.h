//
//  VaseWindow.h
//  vaseIOS
//
//  Created by yangjiandong on 2021/9/20.
//

#ifndef WaseWindow_h
#define WaseWindow_h
#import <UIKit/UIKit.h>


@interface WaseWindow : UIView {
    void* windowObj;
    void* _graphics;
}

- (instancetype)initWithObj: (void*)winObj;

- (void)onBack;

@end

#endif /* WaseWindow_h */
