
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

    void init(int type);
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