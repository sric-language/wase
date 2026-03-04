

#include "IosGraphics.h"
#include "IosWindow.h"
#import <UIKit/UIKit.h>
#include "WaseWindow.h"
#include "IosTextInput.h"

using namespace waseGraphics;

extern float g_screenScale;
extern bool g_autoScale;

UIViewController *g_controller;

void waseSetViewController(UIViewController *controller) {
    g_controller = controller;
}

void IosWindow::init(WaseWindow* window, sric::OwnPtr<waseGraphics::View> view, waseGraphics::Size size) SC_NOTHROW {
    this->window = window;
    _size = size;
    _view = std::move(view);
    sric::RefPtr<Window> self = sric::rawToRef(this);
    _view->setHost(self);
}

IosWindow::~IosWindow() SC_NOTHROW {
    this->window = NULL;
}

sric::RefPtr<View> IosWindow::view() SC_NOTHROW {
    return _view;
}

void IosWindow::repaint(waseGraphics::Rect& dirty) SC_NOTHROW {
    _dirty = true;
    [window performSelectorOnMainThread:@selector(setNeedsDisplay) withObject:nil waitUntilDone:NO];
}

waseGraphics::Size IosWindow::size() SC_NOTHROW {
    return _size;
}

bool IosWindow::hasFocus() SC_NOTHROW {
    return [window isFocused];
}

void IosWindow::focus() SC_NOTHROW {
    [this->window setNeedsFocusUpdate];
    [this->window updateFocusIfNeeded];
}

sric::OwnPtr<TextInput> IosWindow::textInput(int inputType) SC_NOTHROW {
#if 1
    _textInput = sric::new_<IosTextInput>();
    _textInput->init(this->window, inputType);
    return _textInput.share();
#else
    return sric::OwnPtr<TextInput>();
#endif
}

void IosWindow::fileDialog(bool isOpen, const char* accept) SC_NOTHROW {

}

void IosWindow::displayKeyboard(bool display) SC_NOTHROW {

}

void IosWindow::onResize(int w, int h) SC_NOTHROW {
    _size.w = w;
    _size.h = h;
}


sric::OwnPtr<IosWindow> g_window;

bool fireTimeEventsScheduled = false;
void fireTimeEventsLatter() {
    int delay= 16;
    dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delay * NSEC_PER_MSEC));
    dispatch_after(popTime, dispatch_get_main_queue(), ^(void){
        fireTimeEvents();
        fireTimeEventsLatter();
    });
}

int Window::open(sric::OwnPtr<waseGraphics::View> view, const char* name) SC_NOTHROW {
    if (!g_window.isNull()) {
        return -1;
    }
    
    float desityScale = [[UIScreen mainScreen] scale];
    g_screenScale = desityScale;
    g_autoScale = true;
    
    g_window = sric::new_<IosWindow>();
    WaseWindow* window = [[WaseWindow alloc] initWithObj: g_window.get()];
    
    CGRect frame = [UIScreen mainScreen].bounds;//g_controller.view.bounds;
        
    #ifdef SAFE_AREA
        UIEdgeInsets insets = [UIApplication sharedApplication].windows.firstObject.safeAreaInsets;
        frame.origin.y += insets.top;
        frame.size.height -= insets.top;
    #elif defined(SAFE_AREA)
        UIEdgeInsets insets = [UIApplication sharedApplication].windows.firstObject.safeAreaInsets;
        frame.origin.x += insets.left;
        frame.origin.y += insets.top;
        frame.size.width -= insets.left + insets.right;
        frame.size.height -= insets.top + insets.bottom;
    #endif
    
    window.frame = frame;
    [g_controller.view addSubview:window];
    
    waseGraphics::Size size;
    size.w = frame.size.width * desityScale;
    size.h = frame.size.height * desityScale;
    g_window->init(window, std::move(view), size);
    
    
    if (!fireTimeEventsScheduled) {
        fireTimeEventsScheduled = true;
        fireTimeEventsLatter();
    }
    
    return 0;
}

sric::RefPtr<Window> Window::getCur() SC_NOTHROW {
    return g_window;
}
