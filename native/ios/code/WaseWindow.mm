//
//  VaseWindow.m
//  vaseIOS
//
//  Created by yangjiandong on 2021/9/20.
//

#import "WaseWindow.h"
#include "Window.h"
#include "IosGraphics.h"

using namespace waseGraphics;

@interface WaseWindow () {
    
}
@end

@implementation WaseWindow

- (instancetype)initWithObj: (void*)winObj {
    self = [super init];
    if (self != nil) {
        windowObj = winObj;
    }
    return self;
}

- (void)dealloc {
    
}

- (id)initWithFrame:(CGRect)frame{
    self = [super initWithFrame:frame];
    if (self) {
        self.backgroundColor = [UIColor clearColor];
    }
    return self;
}

- (void)drawRect:(CGRect)rect {
    //printf("drawRect\n");
    Window* win = (Window*)windowObj;
    
    CGContextRef ctx = UIGraphicsGetCurrentContext();
    CGContextSaveGState(ctx);
    float desityScale = [[UIScreen mainScreen] scale];
    CGContextScaleCTM(ctx, 1/desityScale, 1/desityScale);
    
    waseGraphics::Graphics* graphics = waseGraphics::createIOSGraphics((waseGraphics::Graphics*)_graphics, ctx);
    _graphics = graphics;
    win->paint(*graphics);
    
    CGContextRestoreGState(ctx);
}

bool toFanTouch(UIView *view, UITouch * touch, waseGraphics::MotionEventType type, MotionEvent& env) {

    if (type == waseGraphics::MotionEventType::Other) {
        UITouchPhase phase = touch.phase;
        switch (phase) {
            case UITouchPhaseBegan:
                type = waseGraphics::MotionEventType::Press;
                break;
            case UITouchPhaseMoved:
                type = waseGraphics::MotionEventType::TouchMove;
                break;
            case UITouchPhaseEnded:
                type = waseGraphics::MotionEventType::Release;
                break;
            case UITouchPhaseCancelled:
                type = waseGraphics::MotionEventType::Cancel;
                break;
            default:
        }
    }
    
    float desityScale = [[UIScreen mainScreen] scale];
    
    env.type = type;
    env.pressure = touch.force/touch.maximumPossibleForce;
    env.size = touch.majorRadius * desityScale;
    
    CGPoint pos = [touch locationInView:view];
    env.x = pos.x * desityScale;
    env.y = pos.y * desityScale;
    
    //printf("touch:%f, %f, %f\n", touch.force, touch.majorRadius, touch.maximumPossibleForce);
    
    uint64_t pointerId = (uint64_t)touch;
    env.pointerId = pointerId;
    return true;
}

bool fireTouchAll(UIView *view, NSSet<UITouch *> *touches, UIEvent* event, waseGraphics::MotionEventType type, Window* winObj) {
    UITouch *touch = touches.anyObject;
    NSSet *all = event.allTouches;
    
    MotionEvent evt;
    toFanTouch(view, touch, type, evt);
    
    for (UITouch *t in all) {
        MotionEvent evt1;
        if (toFanTouch(view, t, waseGraphics::MotionEventType::Other, evt1)) {
            if (evt.pointers.getPtr() == NULL) {
                evt.pointers = sric::toShared(sric::new_<sric::DArray<waseGraphics::MotionEvent> >());
            }
            evt.pointers->add(std::move(evt1));
        }
    }
    
    winObj->view()->onMotionEvent(evt);
    
    return evt.consumed;
}

-(void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    fireTouchAll(self, touches, event, waseGraphics::MotionEventType::Press, (Window*)windowObj);
    [self endEditing:YES];
}

-(void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    fireTouchAll(self, touches, event, waseGraphics::MotionEventType::TouchMove, (Window*)windowObj);
}

-(void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    fireTouchAll(self, touches, event, waseGraphics::MotionEventType::Release, (Window*)windowObj);
}

-(void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    fireTouchAll(self, touches, event, waseGraphics::MotionEventType::Cancel, (Window*)windowObj);
}
- (void)onBack {
    
}

@end
