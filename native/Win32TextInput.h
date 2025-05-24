
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

    void init(HWND hWnd, int type) SC_NOTHROW;
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