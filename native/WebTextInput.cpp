
#include "WebTextInput.h"

using namespace waseGraphics;

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

extern "C" {
  extern void editTextCreate(int type, const char* text, void* textBox);
  extern void editTextRemove();
  extern void editTextUpdate(int x, int y, int w, int h, int fontSize);

  void EMSCRIPTEN_KEEPALIVE editTextOnTextChange(void* textBox, const char* text) {
      ((WebTextInput*)textBox)->onTextChange(text);
  }
}

extern float lastXScale;

void WebTextInput::init(int type) SC_NOTHROW {
    this->type = type;
}

void WebTextInput::close() SC_NOTHROW {
    editTextRemove();
}
void WebTextInput::setPos(int x, int y, int w, int h) SC_NOTHROW {
    float scale = 1.0 / lastXScale;
    this->x = x * scale;
    this->y = y * scale;
    this->w = w * scale;
    this->h = h * scale;
}
void WebTextInput::setStyle(waseGraphics::Font& font, float fontSize, waseGraphics::Color textColor, waseGraphics::Color backgroundColor) SC_NOTHROW {
    float scale = 1.0 / lastXScale;
    this->fontSize = fontSize * scale;
}
void WebTextInput::setText(const char* text) SC_NOTHROW {
    this->text = text;
}
void WebTextInput::setType(int lineNum, bool editable) SC_NOTHROW {
}
void WebTextInput::focus() SC_NOTHROW {
    editTextCreate(type, text.c_str(), this);
    editTextUpdate(x, y, w, h, fontSize);
}

void WebTextInput::select(int start, int end) SC_NOTHROW {
}
int WebTextInput::caretPos() SC_NOTHROW {
    return 0;
}

#endif