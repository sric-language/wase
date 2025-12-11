#include "NanovgGraphics.h"
#include "nanovg.h"

using namespace waseGraphics;

NVGcolor toNVColor(Color color) {
    return nvgRGBA(color.ir(), color.ig(), color.ib(), color.ia());
}

NVGcontext* getCurrentNanoVgContext();

class NanovgImage : public Image {
    int w = 0;
    int h = 0;
    int imageFlags = 0;
public:
    int image = 0;
    NVGcontext* vg;

    int width() SC_NOTHROW {
        return w;
    }
    int height() SC_NOTHROW {
        return h;
    }
    void setFlags(int flags) SC_NOTHROW {
        imageFlags = flags;
    }
    bool load(const char* filename) SC_NOTHROW {
        image = nvgCreateImage(vg, filename, imageFlags);
        if (image) {
            nvgImageSize(vg, image, &w, &h);
        }
        return image != 0;
    }
    bool loadMem(unsigned char* data, int ndata) SC_NOTHROW {
        image = nvgCreateImageMem(vg, imageFlags, data, ndata);
        if (image) {
            nvgImageSize(vg, image, &w, &h);
        }
        return image != 0;
    }
    void initFromRgba(int w, int h, const unsigned char* data) SC_NOTHROW {
        image = nvgCreateImageRGBA(vg, w, h, imageFlags, data);
        if (image) {
            nvgImageSize(vg, image, &w, &h);
        }
    }
    void updateRgba(const unsigned char* data) SC_NOTHROW {
        nvgUpdateImage(vg, image, data);
    }
    virtual ~NanovgImage() SC_NOTHROW {
        if (vg == getCurrentNanoVgContext()) {
            nvgDeleteImage(vg, image);
        }
        image = 0;
    }
    bool isReady() SC_NOTHROW {
        return image != 0;
    }
};

class NanovgGraphicsPath : public GraphicsPath {
public:
    NVGcontext* vg;

    void clear() SC_NOTHROW {
        nvgBeginPath(vg);
    }
    void close() SC_NOTHROW {
        nvgClosePath(vg);
    }
    void moveTo(float x, float y) SC_NOTHROW {
        nvgMoveTo(vg, x, y);
    }
    void lineTo(float x, float y) SC_NOTHROW {
        nvgLineTo(vg, x, y);
    }
    void quadTo(float cx, float cy, float x, float y) SC_NOTHROW {
        nvgQuadTo(vg, cx, cy, x, y);
    }
    void cubicTo(float cx1, float cy1, float cx2, float cy2, float x, float y) SC_NOTHROW {
        nvgBezierTo(vg, cx1, cy1, cx2, cy2, x, y);
    }
    void arcTo(float x1, float y1, float x2, float y2, float radius) SC_NOTHROW {
        nvgArcTo(vg, x1, y1, x2, y2, radius);
    }
    void arc(float cx, float cy, float radius, float startAngle, float arcAngle) SC_NOTHROW {
        nvgArc(vg, cx, cy, radius, (startAngle), (startAngle+arcAngle), NVG_CCW);
    }
};

NVGpaint toNVPaint(NVGcontext* vg, Paint* brush) SC_NOTHROW {
    if (LinearGradient* p = dynamic_cast<LinearGradient*>(brush)) {
        return nvgLinearGradient(vg, p->sx, p->sy, p->ex, p->ey, toNVColor(p->icol), toNVColor(p->ocol));
    }
    else if (BoxGradient* p = dynamic_cast<BoxGradient*>(brush)) {
        return nvgBoxGradient(vg, p->x, p->y, p->w, p->h, p->r, p->f, toNVColor(p->icol), toNVColor(p->ocol));
    }
    else if (RadialGradient* p = dynamic_cast<RadialGradient*>(brush)) {
        return nvgRadialGradient(vg, p->cx, p->cy, p->inr, p->outr, toNVColor(p->icol), toNVColor(p->ocol));
    }
    else if (ImagePattern* p = dynamic_cast<ImagePattern*>(brush)) {
        int handle = dynamic_cast<NanovgImage*>(p->image.get())->image;
        return nvgImagePattern(vg, p->ox, p->oy, p->ex, p->ey, p->angle, handle, p->alpha);
    }
    else {
        abort();
    }
}

class NanovgGraphics : public Graphics {
    NVGcontext* vg;
    sric::OwnPtr<NanovgGraphicsPath> curPath;
    int defaultFont = 0;
public:
    NanovgGraphics(NVGcontext* vg) SC_NOTHROW : vg(vg) {
#ifdef __EMSCRIPTEN__
        defaultFont = nvgCreateFont(vg, "fallback", "res/Roboto-Regular.ttf");
#else
        defaultFont = nvgCreateFont(vg, "fallback", "C:/Windows/Fonts/msyh.ttc");
        if (defaultFont == -1) {
            defaultFont = nvgCreateFont(vg, "fallback", "res/Roboto-Regular.ttf");
        }
#endif
        if (defaultFont != -1) {
            nvgAddFallbackFontId(vg, defaultFont, defaultFont);
        }
    }

    void compositeOperation(CompositeOperation op) SC_NOTHROW {
        nvgGlobalCompositeOperation(vg, (int)op);
    }
    void globalAlpha(float alpha) SC_NOTHROW {
        nvgGlobalAlpha(vg, alpha);
    }
    void antiAlias(bool enabled) SC_NOTHROW {
        nvgShapeAntiAlias(vg, enabled);
    }

    void save() SC_NOTHROW {
        nvgSave(vg);
    }
    void restore() SC_NOTHROW {
        nvgRestore(vg);
    }

    void setColor(Color color) SC_NOTHROW {
        nvgStrokeColor(vg, toNVColor(color));
        nvgFillColor(vg, toNVColor(color));
    }

    void lineWidth(float size) SC_NOTHROW {
        nvgStrokeWidth(vg, size);
    }
    void setPen(Pen& pen) SC_NOTHROW {
        nvgLineCap(vg, (int)pen.cap);
        nvgLineJoin(vg, (int)pen.join);
        nvgMiterLimit(vg, pen.miterLimit);
    }

    void setPaint(Paint& brush) SC_NOTHROW {
        NVGpaint nvgp = toNVPaint(vg, &brush);
        nvgStrokePaint(vg, nvgp);
        nvgFillPaint(vg, nvgp);
    }

    //Transform2D getTransform();
    void transform(const Transform2D& trans) SC_NOTHROW {
        nvgTransform(vg, trans.a, trans.b, trans.c, trans.d, trans.e, trans.f);
    }

    void clip(float x, float y, float w, float h) SC_NOTHROW {
        nvgIntersectScissor(vg, x, y, w, h);
    }

    sric::RefPtr<GraphicsPath> beginPath() SC_NOTHROW {
        nvgBeginPath(vg);
        if (curPath.isNull()) {
            curPath = sric::new_<NanovgGraphicsPath>();
            curPath->vg = vg;
        }
        return curPath;
    }
    void fillPath(sric::RefPtr<GraphicsPath> path) SC_NOTHROW {
        sc_assert(path.get() == curPath.get(), "Must Current Path");
        nvgFill(vg);
    }
    void drawPath(sric::RefPtr<GraphicsPath> path) SC_NOTHROW {
        sc_assert(path.get() == curPath.get(), "Must Current Path");
        nvgStroke(vg);
    }

    void setFont(Font& font) SC_NOTHROW {
        //nvgFontFace(vg, font->name);
        nvgFontFaceId(vg, defaultFont);
        nvgFontBlur(vg, font.bold ? 1 : 0);
    }
    void setFontSize(float size) SC_NOTHROW {
        nvgFontSize(vg, size);
    }
    void drawText(float x, float y, const char* str, int size = -1) SC_NOTHROW {
        const char* end = NULL;
        if (size != -1) {
            end = str + size;
        }
        nvgText(vg, x, y, str, end);
    }
    void fontMetrics(FontMetrics& metric) SC_NOTHROW {
        float ascender; float descender; float lineh;
        nvgTextMetrics(vg, &ascender, &descender, &lineh);
        metric.ascent = ascender;
        metric.descent = descender;
        metric.lineHeight = lineh;
        metric.leading = lineh - ascender - descender;
    }
    float textWidth(const char* str, int size = -1) SC_NOTHROW {
        const char* end = NULL;
        if (size != -1) {
            end = str + size;
        }
        float bounds[4];
        nvgTextBounds(vg, 0, 0, str, end, bounds);
        return bounds[2];
    }

    sric::OwnPtr<Image> createImage() SC_NOTHROW {
        sric::OwnPtr<NanovgImage> image = sric::new_<NanovgImage>();
        image->vg = vg;
        return image;
    }

    /**
    ** Copy a rectangular region of the image to the graphics
    ** device.  If the source and destination don't have the
    ** same size, then the copy is resized.
    **/
    void drawImageEx(sric::RefPtr<Image> image, Rect& src, Rect& dest) SC_NOTHROW {
        if (!image->isReady()) {
            return;
        }
        nvgSave(vg);

        float srcX = src.x;
        float srcY = src.y;
        float srcW = src.w;
        float srcH = src.h;
        float dstX = dest.x;
        float dstY = dest.y;
        float dstW = dest.w;
        float dstH = dest.h;
        
        int w = image->width();
        int h = image->height();
        int handle = dynamic_cast<NanovgImage*>(image.get())->image;
        NVGpaint paint = nvgImagePattern(vg, 0, 0, w, h,
            0, handle, 1);

        nvgBeginPath(vg);
        //nvgRect(vg, dstX, dstY, dstW, dstH);
        //nvgRect(vg, 50, 50, dstW, dstH);

        float scaleX = (float)dstW / srcW;
        float scaleY = (float)dstH / srcH;
        nvgTranslate(vg, dstX - (srcX * scaleX), dstY - (srcY * scaleY));
        //nvgTranslate(vg, 50, 50);
        nvgScale(vg, scaleX, scaleY);
        nvgRect(vg, srcX, srcY, srcW, srcH);

        nvgFillPaint(vg, paint);
        nvgFill(vg);
        nvgRestore(vg);
    }
};

Graphics* waseGraphics::createNanovgGraphics(NVGcontext* vg) SC_NOTHROW
{
    return new NanovgGraphics(vg);
}
