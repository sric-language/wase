
#include "IosGraphics.h"


#import <CoreGraphics/CoreGraphics.h>
#import <UIKit/UIKit.h>

#include <math.h>
#include <string.h>

#define MIN(a, b)  ((a) < (b) ? (a) : (b))
#define PI 3.14159265358979323846

extern float g_screenScale;

using namespace waseGraphics;


//plutovg_color_t toNVColor(Color color) {
//    plutovg_color_t c;
//    plutovg_color_init_rgba(&c, color.r(), color.g(), color.b(), color.a());
//    return c;
//}

class IosImage : public Image {
    int w = 0;
    int h = 0;
    int imageFlags = 0;
public:
    CGImageRef image = NULL;

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
        NSString *resPath = [NSBundle.mainBundle resourcePath];
        NSString* path = [NSString stringWithFormat:@"%@/%s", resPath, filename];
        
        UIImage * uiImage = [UIImage imageNamed: path];
        image = uiImage.CGImage;
        CGImageRetain(image);
        w = uiImage.size.width;
        h = uiImage.size.height;
        return image != NULL;
    }
    bool loadMem(unsigned char* buffer, int len) SC_NOTHROW {
        NSData *nsdata = [NSData dataWithBytes:buffer length:len];
        UIImage *uiImage = [UIImage imageWithData:nsdata];
        image = uiImage.CGImage;
        CGImageRetain(image);
        w = uiImage.size.width;
        h = uiImage.size.height;
        return image != NULL;
    }
    void initFromRgba(int w, int h, const unsigned char* data) SC_NOTHROW {
        int componentsPerPixel = 4;
        int bitsPerComponent = 8;
        int bitsPerPixel = 32;
        
        size_t bufferLength = w * h * componentsPerPixel;
        CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, data, bufferLength, NULL);
        size_t bytesPerRow = componentsPerPixel * w;
        CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;

        CGImageRef iref = CGImageCreate(w,
                                      h,
                                      bitsPerComponent,
                                      bitsPerPixel,
                                      bytesPerRow,
                                      CGColorSpaceCreateDeviceRGB(),
                                      kCGBitmapByteOrder32Little | kCGImageAlphaFirst,
                                      provider,
                                      NULL,
                                      true,
                                      renderingIntent);
        CGDataProviderRelease(provider);
    //    UIImage * img = [UIImage imageNamed:@"image.png"];
    //    CGImageRef temImg = img.CGImage;
        
        image = iref;
        if (image) {
            this->w = w;
            this->h = h;
        }
    }
    void updateRgba(const unsigned char* data) SC_NOTHROW {
        //nvgUpdateImage(vg, image, data);
    }
    virtual ~IosImage() SC_NOTHROW {
        if (image != NULL) {
            CGImageRelease(image);
            image = NULL;
        }
    }
    bool isReady() SC_NOTHROW {
        return image != NULL;
    }
};

class IosGraphicsPath : public GraphicsPath {
public:
    CGContextRef vg = NULL;

    void clear() SC_NOTHROW {
        CGContextBeginPath(vg);
    }
    void close() SC_NOTHROW {
        CGContextClosePath(vg);
    }
    void moveTo(float x, float y) SC_NOTHROW {
        CGContextMoveToPoint(vg, x, y);
    }
    void lineTo(float x, float y) SC_NOTHROW {
        CGContextAddLineToPoint(vg, x, y);
    }
    void quadTo(float cx, float cy, float x, float y) SC_NOTHROW {
        CGContextAddQuadCurveToPoint(vg, cx, cy, x, y);
    }
    void cubicTo(float cx1, float cy1, float cx2, float cy2, float x, float y) SC_NOTHROW {
        CGContextAddCurveToPoint(vg, cx1, cy1, cx2, cy2, x, y);
    }
    void arcTo(float x1, float y1, float x2, float y2, float radius) SC_NOTHROW {
        //TODO radius to angle
        //plutovg_canvas_arc_to(vg, x1, y1, radius, true, true, x2, y2);
    }
    void arc(float cx, float cy, float radius, float startAngle, float arcAngle) SC_NOTHROW {
        CGContextAddArc(vg, cx, cy, radius, startAngle, arcAngle, 0);
    }
};

void toNVPaint(CGContextRef vg, Paint* brush) SC_NOTHROW {
    if (LinearGradient* p = dynamic_cast<LinearGradient*>(brush)) {
        CGGradientRef myGradient;
        CGColorSpaceRef myColorspace;
        size_t num_locations = 2;
        CGFloat locations[2] = { 0.0, 1.0 };
        CGFloat components[8] = { p->icol.r(), p->icol.g(), p->icol.b(), p->icol.a(),  // Start color
                                  p->ocol.r(), p->ocol.g(), p->ocol.b(), p->ocol.a()}; // End color
        
        myColorspace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
        myGradient = CGGradientCreateWithColorComponents (myColorspace, components,
                                  locations, num_locations);
        
        
        CGPoint myStartPoint, myEndPoint;
        myStartPoint.x = p->sx;
        myStartPoint.y = p->sy;
        myEndPoint.x = p->ex;
        myEndPoint.y = p->ey;
        CGContextDrawLinearGradient (vg, myGradient, myStartPoint, myEndPoint, 0);
        
        CGColorSpaceRelease(myColorspace);
        CGGradientRelease(myGradient);
    }
    else if (BoxGradient* p = dynamic_cast<BoxGradient*>(brush)) {
        return;
    }
    else if (RadialGradient* p = dynamic_cast<RadialGradient*>(brush)) {
        return;
    }
    else if (ImagePattern* p = dynamic_cast<ImagePattern*>(brush)) {
        return;
    }
    else {
        abort();
    }
}

class IosGraphics : public Graphics {
    CGContextRef vg;
    sric::OwnPtr<IosGraphicsPath> curPath;
    UIFont *uifont = NULL;
    float fontSize = 20;
    bool fontBold = false;
    bool fontItalic = false;
    Color color;
public:
    IosGraphics(CGContextRef vg) SC_NOTHROW : vg(vg) {
        uifont = [UIFont systemFontOfSize:fontSize];
    }
    
    void setHandle(CGContextRef vg) {
        this->vg = vg;
        curPath.clear();
    }

    void compositeOperation(CompositeOperation op) SC_NOTHROW {
        CGBlendMode p = kCGBlendModeNormal;
        switch (op)
        {
        case waseGraphics::CompositeOperation::SourceOver:
            p = kCGBlendModeNormal;
            break;
        case waseGraphics::CompositeOperation::SourceIn:
            p = kCGBlendModeSourceIn;
            break;
        case waseGraphics::CompositeOperation::SourceOut:
            p = kCGBlendModeSourceOut;
            break;
        case waseGraphics::CompositeOperation::Atop:
            p = kCGBlendModeSourceAtop;
            break;
        case waseGraphics::CompositeOperation::DestinationOver:
            p = kCGBlendModeDestinationOver;
            break;
        case waseGraphics::CompositeOperation::DestinationIn:
            p = kCGBlendModeDestinationIn;
            break;
        case waseGraphics::CompositeOperation::DestinationOut:
            p = kCGBlendModeDestinationOut;
            break;
        case waseGraphics::CompositeOperation::DesitnationAtop:
            p = kCGBlendModeDestinationAtop;
            break;
        case waseGraphics::CompositeOperation::Lighter:
            p = kCGBlendModeLighten;
            break;
        case waseGraphics::CompositeOperation::Copy:
            p = kCGBlendModeCopy;
            break;
        case waseGraphics::CompositeOperation::Xor:
            p = kCGBlendModeXOR;
            break;
        default:
            break;
        }
        CGContextSetBlendMode(vg, p);
    }
    void globalAlpha(float alpha) SC_NOTHROW {
        CGContextSetAlpha(vg, alpha);
    }
    void antiAlias(bool enabled) SC_NOTHROW {
        CGContextSetShouldAntialias(vg, enabled);
    }

    void save() SC_NOTHROW {
        CGContextSaveGState(vg);
    }
    void restore() SC_NOTHROW {
        CGContextRestoreGState(vg);
    }

    void setColor(Color color) SC_NOTHROW {
        CGContextSetRGBFillColor(vg, color.r(), color.g(), color.b(), color.a());
        CGContextSetRGBStrokeColor(vg, color.r(), color.g(), color.b(), color.a());
        this->color = color;
    }

    void lineWidth(float size) SC_NOTHROW {
        CGContextSetLineWidth(vg, size);
    }
    void setPen(Pen& pen) SC_NOTHROW {
        CGLineCap cap = kCGLineCapRound;
        switch (pen.cap)
        {
        case LineCap::Butt:
            cap = kCGLineCapButt;
            break;
        case LineCap::Round:
            cap = kCGLineCapRound;
            break;
        case LineCap::Square:
            cap = kCGLineCapSquare;
            break;
        default:
            break;
        }
        CGContextSetLineCap(vg, cap);

        CGLineJoin join = kCGLineJoinRound;
        switch (pen.join)
        {
        case LineJoin::Bevel:
            join = kCGLineJoinBevel;
            break;
        case LineJoin::Round:
            join = kCGLineJoinRound;
            break;
        case LineJoin::Miter:
            join = kCGLineJoinMiter;
            break;
        default:
            break;
        }
        CGContextSetLineJoin(vg, join);
        //TODO
        //plutovg_canvas_set_miter_limit(vg, pen.miterLimit);
    }

    void setPaint(Paint& brush) SC_NOTHROW {
        toNVPaint(vg, &brush);
    }

    //Transform2D getTransform();
    void transform(const Transform2D& trans) SC_NOTHROW {
        CGContextConcatCTM(vg, CGAffineTransformMake(trans.a, trans.b, trans.c, trans.d, trans.e, trans.f));
    }

    void clip(float x, float y, float w, float h) SC_NOTHROW {
        CGRect rect = CGRectMake(x, y, w, h);
        CGContextClipToRect(vg, rect);
    }

    sric::RefPtr<GraphicsPath> beginPath() SC_NOTHROW {
        CGContextBeginPath(vg);
        if (curPath.isNull()) {
            curPath = sric::new_<IosGraphicsPath>();
            curPath->vg = vg;
        }
        return curPath;
    }
    void fillPath(sric::RefPtr<GraphicsPath> path) SC_NOTHROW {
        sc_assert(path.get() == curPath.get(), "Must Current Path");
        CGContextFillPath(vg);
    }
    void drawPath(sric::RefPtr<GraphicsPath> path) SC_NOTHROW {
        sc_assert(path.get() == curPath.get(), "Must Current Path");
        CGContextStrokePath(vg);
    }

    void setFont(Font& font) SC_NOTHROW {
        CFStringRef fontName = CFStringCreateWithCString(NULL, font.name.c_str(), CFStringGetSystemEncoding());
        CGFontRef cgfont = CGFontCreateWithFontName(fontName);
        CFRelease(fontName);
        
        CGContextSetFont(vg, cgfont);
        CGFontRelease(cgfont);
        fontBold = font.bold;
        fontItalic = font.italic;
        
        updateUiFont();
    }
    
    void updateUiFont() {
        //if (this->uifont) return;
        UIFont *uifont;
        if (fontBold) {
            uifont = [UIFont boldSystemFontOfSize:fontSize];
        }
        else if (fontItalic) {
            uifont = [UIFont italicSystemFontOfSize:fontSize];
        }
        else {
            uifont = [UIFont systemFontOfSize:fontSize];
        }
        this->uifont = uifont;
    }
    
    void setFontSize(float size) SC_NOTHROW {
        //fontSize = size / g_screenScale;
        fontSize = size;
        CGContextSetFontSize(vg, fontSize);
        
        updateUiFont();
    }
    
    void drawText(float x, float y, const char* str, int size = -1) SC_NOTHROW {
        //TODO size
        NSString *nsstr = [NSString stringWithUTF8String: str];
        CGContextSetTextDrawingMode(vg, kCGTextFill);

        UIGraphicsPushContext(vg);
        
        NSDictionary *attrs = [NSDictionary dictionaryWithObjectsAndKeys:uifont, NSFontAttributeName,
                               [UIColor colorWithRed:color.r() green:color.g() blue:color.b() alpha:color.a()], NSForegroundColorAttributeName, nil, nil];
        //NSDictionary *attrs = [[NSDictionary alloc] init];
        int offset = uifont.ascender + uifont.leading;
        [nsstr drawAtPoint:CGPointMake(x,y-offset) withAttributes:attrs];
        //CGContextShowTextAtPoint(vg, x, y, str, strlen(str));

        UIGraphicsPopContext();
    }
    void fontMetrics(FontMetrics& metric) SC_NOTHROW {
        float ascender = uifont.ascender;
        float descender = uifont.descender;
        float lineGap = uifont.leading;
        
        metric.ascent = ascender;
        metric.descent = descender;
        metric.lineHeight = uifont.lineHeight;
        metric.leading = lineGap;
    }
    float textWidth(const char* str, int size = -1) SC_NOTHROW {
        NSString *nsstr = [NSString stringWithUTF8String: str];
        NSDictionary *attrs = [NSDictionary dictionaryWithObjectsAndKeys:uifont, NSFontAttributeName, nil, nil];
        CGSize tsize = [nsstr sizeWithAttributes:attrs];
        return tsize.width;
        //float w = tsize.width;
        //return ceill(w);
    }

    sric::OwnPtr<Image> createImage() SC_NOTHROW {
        sric::OwnPtr<IosImage> image = sric::new_<IosImage>();
        //image->vg = vg;
        return image;
    }

    void drawImageEx(sric::RefPtr<Image> image, waseGraphics::Rect& src, waseGraphics::Rect& dest) SC_NOTHROW {
        if (!image->isReady()) {
            return;
        }
        
        float srcX = src.x;
        float srcY = src.y;
        float srcW = src.w;
        float srcH = src.h;
        float dstX = dest.x;
        float dstY = dest.y;
        float dstW = dest.w;
        float dstH = dest.h;
        
        CGImageRef img = dynamic_cast<IosImage*>(image.get())->image;
            
        CGContextSaveGState(vg);
        CGRect iRect = CGRectMake(0, 0, CGImageGetWidth(img), CGImageGetHeight(img));
        //CGRect sRect = CGRectMake(srcX, srcY, srcW, srcH);
        CGRect dRect = CGRectMake(dstX, dstY, dstW, dstH);
        
        CGContextClipToRect(vg, dRect);
        
        CGRect nRect;
        double scaleX = (double)dstW / srcW;
        double scaleY = (double)dstH / srcH;
        //nRect.origin.x = (iRect.origin.x - (srcX+srcW/2.0)) * scaleX + (dstX + dstW/2.0);
        //nRect.origin.y = (iRect.origin.y - (srcY+srcH/2.0)) * scaleY + (dstY + dstH/2.0);
        nRect.origin.x = dstX - (srcX * scaleX);
        nRect.origin.y = dstY - (srcY * scaleY);
        nRect.size.width = iRect.size.width * scaleX;
        nRect.size.height = iRect.size.height * scaleY;
        
    //    fr_Obj surface = vaseWindow_NGraphics_getBitmap(env, self);
    //    if (surface == NULL)
        {
            double y = nRect.origin.y+nRect.size.height/2.0;
            CGContextTranslateCTM(vg, 0, y);
            CGContextScaleCTM(vg, 1.0, -1.0);
            CGContextTranslateCTM(vg, 0, -y);
        }
        
        CGContextDrawImage(vg, nRect, img);

        CGContextRestoreGState(vg);
    }
};

Graphics* waseGraphics::createIOSGraphics(Graphics* graphics, void* vg) SC_NOTHROW
{
    if (graphics) {
        dynamic_cast<IosGraphics*>(graphics)->setHandle((CGContextRef)vg);
        return graphics;
    }
    return new IosGraphics((CGContextRef)vg);
}
