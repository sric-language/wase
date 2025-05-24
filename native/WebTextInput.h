
#ifndef WEBTEXTINPUT_H_
#define WEBTEXTINPUT_H_

#include "Window.h"

struct WebTextInput : waseGraphics::TextInput
{
    int type;
    float fontSize;
    int x;
    int y;
    int w;
    int h;
    std::string text;

    void init(int type) SC_NOTHROW;
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