
#ifndef WIN32TEXTINPUT_H_
#define WIN32TEXTINPUT_H_

#include "Window.h"

#include <windows.h>
#include <windowsx.h>


#define ID_INPUTEDIT 1001

struct Win32TextInput : waseGraphics::TextInput
{
    HWND textInputHandle = 0;
    HWND hWnd = 0;
    LONG_PTR custemProc = 0;

    void init(HWND hWnd, int type);
    void close();

    void setPos(int x, int y, int w, int h);
    void setStyle(waseGraphics::Font& font, float fontSize, waseGraphics::Color textColor, waseGraphics::Color backgroundColor);
    void setText(const char* text);
    void setType(int lineNum, bool editable);
    void focus();

    void select(int start, int end);
    int caretPos();
};

#endif