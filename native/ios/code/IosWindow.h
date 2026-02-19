#ifndef IOSWINDOW_H_
#define IOSWINDOW_H_

#include "Window.h"
#import "WaseWindow.h"
#include "IosTextInput.h"

class IosWindow : public waseGraphics::Window
{
public:
    WaseWindow* window = NULL;
    
    sric::OwnPtr<waseGraphics::View> _view;
    waseGraphics::Size _size;
    sric::OwnPtr<IosTextInput> _textInput;
    bool _dirty = true;

    void init(WaseWindow* window, sric::OwnPtr<waseGraphics::View> view, waseGraphics::Size size) SC_NOTHROW ;

    ~IosWindow() SC_NOTHROW;

    sric::RefPtr<waseGraphics::View> view() SC_NOTHROW;

    void repaint(waseGraphics::Rect& dirty) SC_NOTHROW;

    waseGraphics::Size size() SC_NOTHROW;

    bool hasFocus() SC_NOTHROW;

    void focus() SC_NOTHROW;

    sric::OwnPtr<waseGraphics::TextInput> textInput(int inputType) SC_NOTHROW;

    void fileDialog(bool isOpen, const char* accept) SC_NOTHROW;

    void displayKeyboard(bool display) SC_NOTHROW;

    void onResize(int w, int h) SC_NOTHROW;
};

#endif
