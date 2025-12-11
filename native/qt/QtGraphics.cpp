#include "QtGraphics.h"

#include <QPainter>
#include <QPainterPath>
#include <QFontDatabase>

using namespace waseGraphics;


QColor toQtColor(Color color) {
    QColor c(color.ir(), color.ig(), color.ib(), color.ia());
    return c;
}

class QtImage : public Image {

public:
    QPixmap image;

    int width() SC_NOTHROW {
        return image.width();
    }
    int height() SC_NOTHROW {
        return image.height();
    }
    void setFlags(int flags) SC_NOTHROW {
    }
    bool load(const char* filename) SC_NOTHROW {
        return image.load(filename);
    }
    bool loadMem(unsigned char* data, int ndata) SC_NOTHROW {
        return image.loadFromData(data, ndata);
    }
    void initData(int w, int h, const unsigned char* data) SC_NOTHROW {
        QImage timage(data, w, h, QImage::Format_RGBA8888);
        image = QPixmap::fromImage(timage);
    }
    void updateData(const unsigned char* data) SC_NOTHROW {
        QImage timage(data, width(), height(), QImage::Format_RGBA8888);
        image = QPixmap::fromImage(timage);
    }
    virtual ~QtImage() SC_NOTHROW {
    }
    bool isReady() SC_NOTHROW {
        return !image.isNull();
    }
};

double rad2deg(double rad) {
    return rad * (180.0 / M_PI);
}

class QtGraphicsPath : public GraphicsPath {
public:
    QPainterPath vg;

    void clear() SC_NOTHROW {
        vg.clear();
    }
    void close() SC_NOTHROW {
        vg.closeSubpath();
    }
    void moveTo(float x, float y) SC_NOTHROW {
        vg.moveTo(x, y);
    }
    void lineTo(float x, float y) SC_NOTHROW {
        vg.lineTo(x, y);
    }
    void quadTo(float cx, float cy, float x, float y) SC_NOTHROW {
        vg.quadTo(cx, cy, x, y);
    }
    void cubicTo(float cx1, float cy1, float cx2, float cy2, float x, float y) SC_NOTHROW {
        vg.cubicTo(cx1, cy1, cx2, cy2, x, y);
    }
    void arcTo(float x1, float y1, float x2, float y2, float radius) SC_NOTHROW {
        //TODO unsupport by Qt
    }
    void arc(float cx, float cy, float radius, float startAngle, float arcAngle) SC_NOTHROW {
        vg.arcMoveTo(cx-radius, cy-radius, radius+radius, radius+radius, rad2deg(startAngle));
        vg.arcTo(cx-radius, cy-radius, radius+radius, radius+radius, rad2deg(startAngle), rad2deg(arcAngle));
    }
};

void toQtPaint(QBrush& qbrush, Paint* brush) SC_NOTHROW {
    if (LinearGradient* p = dynamic_cast<LinearGradient*>(brush)) {
        QLinearGradient stops(p->sx, p->sy, p->ex, p->ey);
        stops.setColorAt(0.0, toQtColor(p->icol));
        stops.setColorAt(1.0, toQtColor(p->ocol));
        qbrush = (QBrush(stops));
    }
    else if (BoxGradient* p = dynamic_cast<BoxGradient*>(brush)) {
        return;
    }
    else if (RadialGradient* p = dynamic_cast<RadialGradient*>(brush)) {
        QRadialGradient stops(p->cx, p->cy, p->inr, p->cx, p->cy, p->outr);
        stops.setColorAt(0.0, toQtColor(p->icol));
        stops.setColorAt(1.0, toQtColor(p->ocol));
        qbrush = (QBrush(stops));
    }
    else if (ImagePattern* p = dynamic_cast<ImagePattern*>(brush)) {
        auto handle = dynamic_cast<QtImage*>(p->image.get())->image;
        qbrush.setTexture(handle);
    }
    else {
        abort();
    }
}

class QtGraphics : public Graphics {
    QPainter* vg = nullptr;
    sric::OwnPtr<QtGraphicsPath> curPath;
    QPen pen;
    QBrush brush;
    QFont font;
public:

    QtGraphics(QPainter* vg) SC_NOTHROW {
        setHandle(vg);
        bool fontOk = loadFont("C:/Windows/Fonts/msyh.ttc");
        if (!fontOk) {
            fontOk = loadFont("res/Roboto-Regular.ttf");
        }
        font.setStyleStrategy(QFont::PreferAntialias);
        font.setHintingPreference(QFont::PreferFullHinting);
    }

    bool loadFont(const char* file) {
        int fontId = QFontDatabase::addApplicationFont(file);
        if (fontId != -1) {
            QStringList families = QFontDatabase::applicationFontFamilies(fontId);
            if (!families.isEmpty()) {
                font.setFamily(families.at(0));
                return true;
            }
        }
        return false;
    }

    void setHandle(QPainter* vg) {
        this->vg = vg;
        if (vg) {
            vg->setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform, true);
        }
    }

    void compositeOperation(CompositeOperation op) SC_NOTHROW {
        QPainter::CompositionMode p = QPainter::CompositionMode_Clear;
        switch (op)
        {
        case waseGraphics::CompositeOperation::SourceOver:
            p = QPainter::CompositionMode_SourceOver;
            break;
        case waseGraphics::CompositeOperation::SourceIn:
            p = QPainter::CompositionMode_SourceIn;
            break;
        case waseGraphics::CompositeOperation::SourceOut:
            p = QPainter::CompositionMode_SourceOut;
            break;
        case waseGraphics::CompositeOperation::Atop:
            p = QPainter::CompositionMode_SourceAtop;
            break;
        case waseGraphics::CompositeOperation::DestinationOver:
            p = QPainter::CompositionMode_DestinationOver;
            break;
        case waseGraphics::CompositeOperation::DestinationIn:
            p = QPainter::CompositionMode_DestinationIn;
            break;
        case waseGraphics::CompositeOperation::DestinationOut:
            p = QPainter::CompositionMode_DestinationOut;
            break;
        case waseGraphics::CompositeOperation::DesitnationAtop:
            p = QPainter::CompositionMode_DestinationAtop;
            break;
        case waseGraphics::CompositeOperation::Lighter:
            p = QPainter::CompositionMode_Lighten;
            break;
        case waseGraphics::CompositeOperation::Copy:
            p = QPainter::CompositionMode_Overlay;
            break;
        case waseGraphics::CompositeOperation::Xor:
            p = QPainter::QPainter::CompositionMode_Xor;
            break;
        default:
            break;
        }
        vg->setCompositionMode(p);
    }
    void globalAlpha(float alpha) SC_NOTHROW {
        vg->setOpacity(alpha);
    }
    void antiAlias(bool enabled) SC_NOTHROW {
        vg->setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform, enabled);
    }

    void save() SC_NOTHROW {
        vg->save();
    }
    void restore() SC_NOTHROW {
        vg->restore();
    }

    void setColor(Color color) SC_NOTHROW {
        pen.setColor(toQtColor(color));
        this->vg->setPen(this->pen);

        brush.setStyle(Qt::SolidPattern);
        brush.setColor(toQtColor(color));
        //this->vg->setBrush(this->brush);
    }

    void lineWidth(float size) SC_NOTHROW {
        pen.setWidth(size);
        this->vg->setPen(this->pen);
    }
    void setPen(Pen& pen) SC_NOTHROW {
        Qt::PenCapStyle cap = Qt::RoundCap;
        switch (pen.cap)
        {
        case LineCap::Butt:
            cap = Qt::FlatCap;
            break;
        case LineCap::Round:
            cap = Qt::RoundCap;
            break;
        case LineCap::Square:
            cap = Qt::SquareCap;
            break;
        default:
            break;
        }
        this->pen.setCapStyle(cap);

        Qt::PenJoinStyle join = Qt::RoundJoin;
        switch (pen.join)
        {
        case LineJoin::Bevel:
            join = Qt::BevelJoin;
            break;
        case LineJoin::Round:
            join = Qt::RoundJoin;
            break;
        case LineJoin::Miter:
            join = Qt::MiterJoin;
            break;
        default:
            break;
        }
        this->pen.setJoinStyle(join);
        this->pen.setMiterLimit(pen.miterLimit);
        this->vg->setPen(this->pen);
    }

    void setPaint(Paint& brush) SC_NOTHROW {
        toQtPaint(this->brush, &brush);
    }

    //Transform2D getTransform();
    void transform(const Transform2D& trans) SC_NOTHROW {
        //QTransform matrix(trans.a, trans.c, trans.e, trans.b, trans.d, trans.f, 0, 0, 1);
        QTransform matrix(trans.a, trans.b, trans.c, trans.d, trans.e, trans.f);
        vg->setTransform(matrix, true);
    }

    void clip(float x, float y, float w, float h) SC_NOTHROW {
        vg->setClipRect(QRectF(x, y, w, h), Qt::IntersectClip);
    }

    sric::RefPtr<GraphicsPath> beginPath() SC_NOTHROW {
        if (curPath.isNull()) {
            curPath = sric::new_<QtGraphicsPath>();
        }
        else {
            curPath->clear();
        }
        return curPath;
    }
    void fillPath(sric::RefPtr<GraphicsPath> path) SC_NOTHROW {
        sc_assert(path.get() == curPath.get(), "Must Current Path");
        vg->fillPath(curPath->vg, brush);
    }
    void drawPath(sric::RefPtr<GraphicsPath> path) SC_NOTHROW {
        sc_assert(path.get() == curPath.get(), "Must Current Path");
        //vg->setBrush(Qt::NoBrush);
        vg->drawPath(curPath->vg);
    }

    void setFont(Font& font) SC_NOTHROW {
        if (font.name.size()) {
            this->font.setFamily(font.name.c_str());
        }
        //this->font.setBold(font.bold);
        this->font.setWeight(font.bold ? QFont::DemiBold : QFont::Normal);
        this->font.setItalic(font.italic);
        vg->setFont(this->font);
    }
    void setFontSize(float size) SC_NOTHROW {
        this->font.setPixelSize(size);
        vg->setFont(this->font);
    }
    void drawText(float x, float y, const char* str, int size = -1) SC_NOTHROW {
        vg->drawText(QPointF(x, y), QString::fromUtf8(str, size));
    }
    void fontMetrics(FontMetrics& metric) SC_NOTHROW {
        QFontMetrics qmetrics(font);
        metric.ascent = qmetrics.ascent();
        metric.descent = -qmetrics.descent();
        metric.height = qmetrics.lineSpacing();
        metric.leading = qmetrics.leading();
    }
    float textWidth(const char* str, int size = -1) SC_NOTHROW {
        QFontMetrics qmetrics(font);
        float w = qmetrics.size(Qt::TextExpandTabs, QString::fromUtf8(str, size)).width();
        return w;
    }

    sric::OwnPtr<Image> createImage() SC_NOTHROW {
        sric::OwnPtr<QtImage> image = sric::new_<QtImage>();
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
        vg->drawPixmap(dest.x, dest.y, dest.w, dest.h, dynamic_cast<QtImage*>(image.get())->image, src.x, src.y, src.w, src.h);
    }
};

Graphics* waseGraphics::createQtGraphics(Graphics* graphics, void* vg) SC_NOTHROW
{
    if (graphics) {
        dynamic_cast<QtGraphics*>(graphics)->setHandle((QPainter*)vg);
        return graphics;
    }
    return new QtGraphics((QPainter*)vg);
}
