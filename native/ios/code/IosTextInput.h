#ifndef IOSTEXTINPUT_H_
#define IOSTEXTINPUT_H_

#include "Window.h"
#import "WaseWindow.h"

@interface EditTextListener : UIResponder <UITextViewDelegate, UITextFieldDelegate>

//@property(atomic,assign) fr_Obj neditText;

@end

struct IosTextInput : waseGraphics::TextInput
{
    UIView *textView;
    bool isTextField;
    EditTextListener *delegate;

    void init(WaseWindow* window, int type) SC_NOTHROW;
    void close() SC_NOTHROW;

    void setPos(int x, int y, int w, int h) SC_NOTHROW;
    void setStyle(waseGraphics::Font& font, float fontSize, waseGraphics::Color textColor, waseGraphics::Color backgroundColor) SC_NOTHROW;
    void setText(const char* text) SC_NOTHROW;
    void setType(int lineNum, bool editable) SC_NOTHROW;
    void focus() SC_NOTHROW;

    void select(int start, int end) SC_NOTHROW;
    int caretPos() SC_NOTHROW;
};

#endif
