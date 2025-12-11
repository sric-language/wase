#include "PlutovgGraphics.h"
#include "plutovg.h"
using namespace waseGraphics;


plutovg_color_t toNVColor(Color color) {
    plutovg_color_t c;
    plutovg_color_init_rgba(&c, color.r(), color.g(), color.b(), color.a());
    return c;
}

class PlutovgImage : public Image {
    int w = 0;
    int h = 0;
    int imageFlags = 0;
public:
    plutovg_surface_t* image = NULL;

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
        image = plutovg_surface_load_from_image_file(filename);
        if (image) {
            w = plutovg_surface_get_width(image);
            h = plutovg_surface_get_height(image);
        }
        return image != NULL;
    }
    bool loadMem(unsigned char* data, int ndata) SC_NOTHROW {
        image = plutovg_surface_load_from_image_data(data, ndata);
        if (image) {
            w = plutovg_surface_get_width(image);
            h = plutovg_surface_get_height(image);
        }
        return image != NULL;
    }
    void initData(int w, int h, const unsigned char* data) SC_NOTHROW {
        int stride = w * 4;
        image = plutovg_surface_create_for_data((unsigned char*)data, w, h, stride);
        if (image) {
            w = plutovg_surface_get_width(image);
            h = plutovg_surface_get_height(image);
        }
    }
    void updateData(const unsigned char* data) SC_NOTHROW {
        //nvgUpdateImage(vg, image, data);
    }
    virtual ~PlutovgImage() SC_NOTHROW {
        plutovg_surface_destroy(image);
        image = NULL;
    }
    bool isReady() SC_NOTHROW {
        return image != NULL;
    }
};

class PlutovgGraphicsPath : public GraphicsPath {
public:
    plutovg_canvas_t* vg = NULL;

    void clear() SC_NOTHROW {
        plutovg_canvas_new_path(vg);
    }
    void close() SC_NOTHROW {
        plutovg_canvas_close_path(vg);
    }
    void moveTo(float x, float y) SC_NOTHROW {
        plutovg_canvas_move_to(vg, x, y);
    }
    void lineTo(float x, float y) SC_NOTHROW {
        plutovg_canvas_line_to(vg, x, y);
    }
    void quadTo(float cx, float cy, float x, float y) SC_NOTHROW {
        plutovg_canvas_quad_to(vg, cx, cy, x, y);
    }
    void cubicTo(float cx1, float cy1, float cx2, float cy2, float x, float y) SC_NOTHROW {
        plutovg_canvas_cubic_to(vg, cx1, cy1, cx2, cy2, x, y);
    }
    void arcTo(float x1, float y1, float x2, float y2, float radius) SC_NOTHROW {
        //TODO radius to angle
        //plutovg_canvas_arc_to(vg, x1, y1, radius, true, true, x2, y2);
    }
    void arc(float cx, float cy, float radius, float startAngle, float arcAngle) SC_NOTHROW {
        plutovg_canvas_arc(vg, cx, cy, radius, (startAngle), (startAngle+arcAngle), true);
    }
};

void toNVPaint(plutovg_canvas_t* vg, Paint* brush) SC_NOTHROW {
    if (LinearGradient* p = dynamic_cast<LinearGradient*>(brush)) {
        plutovg_gradient_stop_t stops[2] = {
            {0, toNVColor(p->icol)},{1, toNVColor(p->ocol)}
        };
        plutovg_canvas_set_linear_gradient(vg, p->sx, p->sy, p->ex, p->ey, PLUTOVG_SPREAD_METHOD_PAD, stops, 2, NULL);
    }
    else if (BoxGradient* p = dynamic_cast<BoxGradient*>(brush)) {
        return;
    }
    else if (RadialGradient* p = dynamic_cast<RadialGradient*>(brush)) {
        plutovg_gradient_stop_t stops[2] = {
            {0, toNVColor(p->icol)},{1, toNVColor(p->ocol)}
        };
        plutovg_canvas_set_radial_gradient(vg, p->cx, p->cy, p->inr, p->cx, p->cy, p->outr, PLUTOVG_SPREAD_METHOD_PAD, stops, 2, NULL);
    }
    else if (ImagePattern* p = dynamic_cast<ImagePattern*>(brush)) {
        plutovg_surface_t* handle = dynamic_cast<PlutovgImage*>(p->image.get())->image;
        plutovg_canvas_set_texture(vg, handle, PLUTOVG_TEXTURE_TYPE_PLAIN, p->alpha, NULL);
    }
    else {
        abort();
    }
}

class PlutovgGraphics : public Graphics {
    plutovg_canvas_t* vg;
    sric::OwnPtr<PlutovgGraphicsPath> curPath;
    plutovg_font_face_t* defaultFont = 0;
public:
    PlutovgGraphics(plutovg_canvas_t* vg) SC_NOTHROW : vg(vg) {
        defaultFont = plutovg_font_face_load_from_file("C:/Windows/Fonts/msyh.ttc", 0);
        if (defaultFont == NULL) {
            defaultFont = plutovg_font_face_load_from_file("res/Roboto-Regular.ttf", 0);
        }
        if (defaultFont != NULL) {
            plutovg_canvas_set_font_face(vg, defaultFont);
        }
    }

    void compositeOperation(CompositeOperation op) SC_NOTHROW {
        plutovg_operator_t p = PLUTOVG_OPERATOR_CLEAR;
        switch (op)
        {
        case waseGraphics::CompositeOperation::SourceOver:
            p = PLUTOVG_OPERATOR_SRC_OVER;
            break;
        case waseGraphics::CompositeOperation::SourceIn:
            p = PLUTOVG_OPERATOR_SRC_IN;
            break;
        case waseGraphics::CompositeOperation::SourceOut:
            p = PLUTOVG_OPERATOR_SRC_OUT;
            break;
        case waseGraphics::CompositeOperation::Atop:
            p = PLUTOVG_OPERATOR_SRC_ATOP;
            break;
        case waseGraphics::CompositeOperation::DestinationOver:
            p = PLUTOVG_OPERATOR_DST_OVER;
            break;
        case waseGraphics::CompositeOperation::DestinationIn:
            p = PLUTOVG_OPERATOR_DST_IN;
            break;
        case waseGraphics::CompositeOperation::DestinationOut:
            p = PLUTOVG_OPERATOR_DST_OUT;
            break;
        case waseGraphics::CompositeOperation::DesitnationAtop:
            p = PLUTOVG_OPERATOR_DST_ATOP;
            break;
        case waseGraphics::CompositeOperation::Lighter:
            p = PLUTOVG_OPERATOR_CLEAR;
            break;
        case waseGraphics::CompositeOperation::Copy:
            p = PLUTOVG_OPERATOR_SRC;
            break;
        case waseGraphics::CompositeOperation::Xor:
            p = PLUTOVG_OPERATOR_XOR;
            break;
        default:
            break;
        }
        plutovg_canvas_set_operator(vg, p);
    }
    void globalAlpha(float alpha) SC_NOTHROW {
        plutovg_canvas_set_opacity(vg, alpha);
    }
    void antiAlias(bool enabled) SC_NOTHROW {
        //nvgShapeAntiAlias(vg, enabled);
    }

    void save() SC_NOTHROW {
        plutovg_canvas_save(vg);
    }
    void restore() SC_NOTHROW {
        plutovg_canvas_restore(vg);
    }

    void setColor(Color color) SC_NOTHROW {
        plutovg_canvas_set_rgba(vg, color.r(), color.g(), color.b(), color.a());
    }

    void lineWidth(float size) SC_NOTHROW {
        plutovg_canvas_set_line_width(vg, size);
    }
    void setPen(Pen& pen) SC_NOTHROW {
        plutovg_line_cap_t cap = PLUTOVG_LINE_CAP_ROUND;
        switch (pen.cap)
        {
        case LineCap::Butt:
            cap = PLUTOVG_LINE_CAP_BUTT;
            break;
        case LineCap::Round:
            cap = PLUTOVG_LINE_CAP_ROUND;
            break;
        case LineCap::Square:
            cap = PLUTOVG_LINE_CAP_SQUARE;
            break;
        default:
            break;
        }
        plutovg_canvas_set_line_cap(vg, cap);

        plutovg_line_join_t join = PLUTOVG_LINE_JOIN_ROUND;
        switch (pen.join)
        {
        case LineJoin::Bevel:
            join = PLUTOVG_LINE_JOIN_BEVEL;
            break;
        case LineJoin::Round:
            join = PLUTOVG_LINE_JOIN_ROUND;
            break;
        case LineJoin::Miter:
            join = PLUTOVG_LINE_JOIN_MITER;
            break;
        default:
            break;
        }
        plutovg_canvas_set_line_join(vg, join);
        plutovg_canvas_set_miter_limit(vg, pen.miterLimit);
    }

    void setPaint(Paint& brush) SC_NOTHROW {
        toNVPaint(vg, &brush);
    }

    //Transform2D getTransform();
    void transform(const Transform2D& trans) SC_NOTHROW {
        plutovg_matrix_t matrix;
        plutovg_matrix_init(&matrix, trans.a, trans.b, trans.c, trans.d, trans.e, trans.f);
        plutovg_canvas_transform(vg, &matrix);
    }

    void clip(float x, float y, float w, float h) SC_NOTHROW {
        plutovg_canvas_clip_rect(vg, x, y, w, h);
    }

    sric::RefPtr<GraphicsPath> beginPath() SC_NOTHROW {
        plutovg_canvas_new_path(vg);
        if (curPath.isNull()) {
            curPath = sric::new_<PlutovgGraphicsPath>();
            curPath->vg = vg;
        }
        return curPath;
    }
    void fillPath(sric::RefPtr<GraphicsPath> path) SC_NOTHROW {
        sc_assert(path.get() == curPath.get(), "Must Current Path");
        plutovg_canvas_fill(vg);
    }
    void drawPath(sric::RefPtr<GraphicsPath> path) SC_NOTHROW {
        sc_assert(path.get() == curPath.get(), "Must Current Path");
        plutovg_canvas_stroke(vg);
    }

    void setFont(Font& font) SC_NOTHROW {
        //nvgFontFace(vg, font->name);
        plutovg_canvas_set_font_face(vg, defaultFont);
    }
    void setFontSize(float size) SC_NOTHROW {
        plutovg_canvas_set_font_size(vg, size);
    }
    void drawText(float x, float y, const char* str, int size = -1) SC_NOTHROW {
        plutovg_canvas_fill_text(vg, str, size, PLUTOVG_TEXT_ENCODING_UTF8, x, y);
    }
    void fontMetrics(FontMetrics& metric) SC_NOTHROW {
        float ascender; float descender; float lineGap;
        plutovg_rect_t rect;
        plutovg_canvas_font_metrics(vg, &ascender, &descender, &lineGap, &rect);
        metric.ascent = ascender;
        metric.descent = descender;
        metric.height = std::max(rect.h, (ascender - descender + lineGap));
        metric.leading = lineGap;
    }
    float textWidth(const char* str, int size = -1) SC_NOTHROW {
        plutovg_rect_t bounds;
        float w = plutovg_canvas_text_extents(vg, str, size, PLUTOVG_TEXT_ENCODING_UTF8, &bounds);
        return ceill(w);
    }

    sric::OwnPtr<Image> createImage() SC_NOTHROW {
        sric::OwnPtr<PlutovgImage> image = sric::new_<PlutovgImage>();
        //image->vg = vg;
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
        plutovg_canvas_save(vg);

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
        plutovg_surface_t* handle = dynamic_cast<PlutovgImage*>(image.get())->image;
        plutovg_canvas_set_texture(vg, handle, PLUTOVG_TEXTURE_TYPE_PLAIN, 1, NULL);

        plutovg_canvas_new_path(vg);
        //nvgRect(vg, dstX, dstY, dstW, dstH);
        //nvgRect(vg, 50, 50, dstW, dstH);

        float scaleX = (float)dstW / srcW;
        float scaleY = (float)dstH / srcH;
        plutovg_canvas_translate(vg, dstX - (srcX * scaleX), dstY - (srcY * scaleY));
        //nvgTranslate(vg, 50, 50);
        plutovg_canvas_scale(vg, scaleX, scaleY);
        plutovg_canvas_rect(vg, srcX, srcY, srcW, srcH);

        //nvgFillPaint(vg, paint);
        plutovg_canvas_fill(vg);
        plutovg_canvas_restore(vg);
    }
};

Graphics* waseGraphics::createPlutovgGraphics(void* vg) SC_NOTHROW
{
    return new PlutovgGraphics((plutovg_canvas_t*)vg);
}