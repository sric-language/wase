#ifndef QTTEXTINPUT_H
#define QTTEXTINPUT_H

#include "Window.h"
#include <QTextEdit>
#include <QLineEdit>

class QtTextInput : public waseGraphics::TextInput
{
    QTextEdit* _textEdit = nullptr;
    QLineEdit* _lineEdit = nullptr;
    QWidget* _widget = nullptr;
    bool _valid = false;
public:
    ~QtTextInput();

    void init(QWidget* parent, int type) SC_NOTHROW;
    void close() SC_NOTHROW;

    void setPos(int x, int y, int w, int h) SC_NOTHROW;
    void setStyle(waseGraphics::Font& font, float fontSize, waseGraphics::Color textColor, waseGraphics::Color backgroundColor) SC_NOTHROW;
    void setText(const char* text) SC_NOTHROW;
    void setType(int lineNum, bool editable) SC_NOTHROW;
    void focus() SC_NOTHROW;

    void select(int start, int end) SC_NOTHROW;
    int caretPos() SC_NOTHROW;
};

#endif // QTTEXTINPUT_H
