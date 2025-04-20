#ifndef NANOVG_GRAPHICS_H_
#define NANOVG_GRAPHICS_H_

#include "waseGraphics.h"

struct NVGcontext;

namespace waseGraphics {
	Graphics* createNanovgGraphics(NVGcontext* vg);
}
#endif