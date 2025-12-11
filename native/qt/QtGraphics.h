#ifndef QTGRAPHICS_H
#define QTGRAPHICS_H

#include "waseGraphics.h"

namespace waseGraphics {
    Graphics* createQtGraphics(Graphics* graphics, void* vg) SC_NOTHROW;
}

#endif // QTGRAPHICS_H
