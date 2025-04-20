
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

void WebTextInput::init(int type) {
    this->type = type;
}

void WebTextInput::close() {
    editTextRemove();
}
void WebTextInput::setPos(int x, int y, int w, int h) {
    float scale = 1.0 / lastXScale;
    this->x = x * scale;
    this->y = y * scale;
    this->w = w * scale;
    this->h = h * scale;
}
void WebTextInput::setStyle(waseGraphics::Font& font, float fontSize, waseGraphics::Color textColor, waseGraphics::Color backgroundColor) {
    float scale = 1.0 / lastXScale;
    this->fontSize = fontSize * scale;
}
void WebTextInput::setText(const char* text) {
    this->text = text;
}
void WebTextInput::setType(int lineNum, bool editable) {
}
void WebTextInput::focus() {
    editTextCreate(type, text.c_str(), this);
    editTextUpdate(x, y, w, h, fontSize);
}

void WebTextInput::select(int start, int end) {
}
int WebTextInput::caretPos() {
    return 0;
}

#endif