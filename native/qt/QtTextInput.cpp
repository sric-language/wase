#include "QtTextInput.h"
#include "QtWindow.h"

QColor toQtColor(waseGraphics::Color color);

QtTextInput::~QtTextInput() {
    if (!_valid && _widget) {
        _widget->deleteLater();
    }
    _valid = false;
    _widget = nullptr;
    _textEdit = nullptr;
    _lineEdit = nullptr;
}

void QtTextInput::init(QWidget* parent, int type) SC_NOTHROW {

    if (type == TextInput::inputTypeMultiLine) {
        _textEdit = new QTextEdit();
        _widget = _textEdit;

        QObject::connect(_textEdit, &QTextEdit::textChanged,
                         [=]() {
            QString text = _textEdit->toPlainText();
            this->onTextChange(text.toUtf8().data());
        });
    }
    else {
        _lineEdit = new QLineEdit();
        if (type == TextInput::inputTypePassword) {
            _lineEdit->setEchoMode(QLineEdit::Password);
        }
        _widget = _lineEdit;

        QObject::connect(_lineEdit, &QLineEdit::textChanged,
                         [=](const QString &text) {
            this->onTextChange(text.toUtf8().data());
        });
    }
    _widget->setParent(parent);
    _widget->show();
    _valid = true;
}

void QtTextInput::close() SC_NOTHROW {
    _widget->setParent(nullptr);
    _valid = false;
}

void QtTextInput::setPos(int x, int y, int w, int h) SC_NOTHROW {
    _widget->setGeometry(x, y, w, h);
}

void QtTextInput::setStyle(waseGraphics::Font& font, float fontSize, waseGraphics::Color textColor, waseGraphics::Color backgroundColor) SC_NOTHROW {
    QFont qfont;
    if (font.name.size()) {
        qfont.setFamily(font.name.c_str());
    }
    qfont.setWeight(font.bold ? QFont::DemiBold : QFont::Normal);
    qfont.setItalic(font.italic);
    qfont.setPixelSize(fontSize);
    _widget->setFont(qfont);

    QPalette palette = _widget->palette();
    palette.setColor(QPalette::Text, toQtColor(textColor));
    palette.setColor(QPalette::Base, toQtColor(backgroundColor));
    _widget->setPalette(palette);
}
void QtTextInput::setText(const char* text) SC_NOTHROW {
    if (_lineEdit) {
        _lineEdit->setText(QString::fromUtf8(text));
    }
    else if (_textEdit) {
        _textEdit->setText(QString::fromUtf8(text));
    }
}

void QtTextInput::setType(int lineNum, bool editable) SC_NOTHROW {
    if (_lineEdit) {
        _lineEdit->setReadOnly(!editable);
    }
    else if (_textEdit) {
        _textEdit->setReadOnly(!editable);
    }
}

void QtTextInput::focus() SC_NOTHROW {
    _widget->setFocus();
}

void QtTextInput::select(int start, int end) SC_NOTHROW {
    if (_lineEdit) {
        _lineEdit->setSelection(start, end-start);
    }
    else if (_textEdit) {
        QTextCursor cursor = _textEdit->textCursor();
        cursor.setPosition(start);
        cursor.setPosition(end, QTextCursor::KeepAnchor);
        _textEdit->setTextCursor(cursor);
    }
}

int QtTextInput::caretPos() SC_NOTHROW {
    if (_lineEdit) {
        return _lineEdit->cursorPosition();
    }
    else if (_textEdit) {
        return _textEdit->textCursor().position();
    }
    return 0;
}
