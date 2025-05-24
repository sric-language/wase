

#include "PlutovgGraphics.h"
#include "Window.h"
#include "Win32TextInput.h"
#include "plutovg.h"
#include <windows.h>
#include <windowsx.h>
using namespace waseGraphics;

std::string LocalToUTF8(const std::string& localStr);

extern float g_screenScle;
extern bool g_autoScale;

class Win32Window : public Window
{
public:
    HWND hWnd = NULL;
    plutovg_surface_t* surface = NULL;
    plutovg_canvas_t* vg = NULL;
    Graphics* graphics = NULL;
    sric::OwnPtr<waseGraphics::View> _view;
    Size _size;
    sric::OwnPtr<Win32TextInput> _textInput;
    bool _dirty = true;

    void init(sric::OwnPtr<waseGraphics::View> view, Size size) SC_NOTHROW {
        _size = size;
        surface = plutovg_surface_create(size.w, size.h);
        vg = plutovg_canvas_create(surface);
        graphics = waseGraphics::createPlutovgGraphics(vg);

        _view = std::move(view);
        sric::RefPtr<Window> self = sric::rawToRef(this);
        _view->setHost(self);
    }

    ~Win32Window() SC_NOTHROW {
        plutovg_canvas_destroy(vg);
        plutovg_surface_destroy(surface);
    }

    sric::RefPtr<View> view() SC_NOTHROW {
        return _view;
    }

    void repaint(Rect& dirty) SC_NOTHROW {
        _dirty = true;
    }

    Size size() SC_NOTHROW {
        /*RECT rect;
        GetClientRect(hWnd, &rect);
        Size s;
        s.w = rect.right - rect.left;
        s.h = rect.bottom - rect.top;
        return s;*/
        return _size;
    }

    bool hasFocus() SC_NOTHROW {
        return true;
    }

    void focus() SC_NOTHROW {

    }

    sric::OwnPtr<TextInput> textInput(int inputType) SC_NOTHROW {
        _textInput = sric::new_<Win32TextInput>();
        _textInput->init(this->hWnd, inputType);
        return _textInput.share();
    }

    void fileDialog(bool isOpen, const char* accept) SC_NOTHROW {

    }

    void displayKeyboard(bool display) SC_NOTHROW {

    }

    void onResize(int w, int h) SC_NOTHROW {
        plutovg_canvas_destroy(vg);
        plutovg_surface_destroy(surface);

        surface = plutovg_surface_create(w, h);
        vg = plutovg_canvas_create(surface);
        graphics = waseGraphics::createPlutovgGraphics(vg);

        _size.w = w;
        _size.h = h;
        InvalidateRect(hWnd, NULL, false);
    }
};

sric::OwnPtr<Win32Window> g_window;

int openWindow(const char* title, int w, int h);

int Window::open(sric::OwnPtr<waseGraphics::View> view, const char* name) SC_NOTHROW {
    if (!g_window.isNull()) {
        return -1;
    }
    auto size = view->getPrefSize(1024, 768);
    g_window = sric::new_<Win32Window>();
    g_window->init(std::move(view), size);
    return openWindow(name, size.w, size.h);
}

sric::RefPtr<Window> Window::getCur() SC_NOTHROW {
    return g_window;
}

int testPaint(plutovg_canvas_t* canvas)
{
    static int i = 0;
    ++i;
    if (i == 1000) {
        i = 0;
    }

    const int width = 150;
    const int height = 150;

    float center_x = width / 2.f + i;
    float center_y = height / 2.f + i;
    float face_radius = 70;
    float eye_radius = 10;
    float mouth_radius = 50;
    float eye_offset_x = 25;
    float eye_offset_y = 20;
    float eye_x = center_x - eye_offset_x;
    float eye_y = center_y - eye_offset_y;

    plutovg_canvas_save(canvas);
    plutovg_canvas_arc(canvas, center_x, center_y, face_radius, 0, PLUTOVG_TWO_PI, 0);
    plutovg_canvas_set_rgb(canvas, 1, 1, 0);
    plutovg_canvas_fill_preserve(canvas);
    plutovg_canvas_set_rgb(canvas, 0, 0, 0);
    plutovg_canvas_set_line_width(canvas, 5);
    plutovg_canvas_stroke(canvas);
    plutovg_canvas_restore(canvas);

    plutovg_canvas_save(canvas);
    plutovg_canvas_arc(canvas, eye_x, eye_y, eye_radius, 0, PLUTOVG_TWO_PI, 0);
    plutovg_canvas_arc(canvas, center_x + eye_offset_x, eye_y, eye_radius, 0, PLUTOVG_TWO_PI, 0);
    plutovg_canvas_set_rgb(canvas, 0, 0, 0);
    plutovg_canvas_fill(canvas);
    plutovg_canvas_restore(canvas);

    plutovg_canvas_save(canvas);
    plutovg_canvas_arc(canvas, center_x, center_y, mouth_radius, 0, PLUTOVG_PI, 0);
    plutovg_canvas_set_rgb(canvas, 0, 0, 0);
    plutovg_canvas_set_line_width(canvas, 5);
    plutovg_canvas_stroke(canvas);
    plutovg_canvas_restore(canvas);


    return 0;
}

void drawBitmap(HWND hwnd, HDC hdc, char* data, int w, int h) {
    int x = 0;
    int y = 0;
    int width = w;
    int height = h;

    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    SetDIBitsToDevice(
        hdc,
        x, y,
        width, height,
        0, 0,
        0, height,
        data,
        &bmi,
        DIB_RGB_COLORS
    );
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static bool isMouseDown = false;
    static int mouseX = 0;
    static int mouseY = 0;

    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        //testPaint(g_window->vg);
        g_window->paint(*g_window->graphics);

        char* data = (char*)plutovg_surface_get_data(g_window->surface);
        int w = plutovg_surface_get_width(g_window->surface);
        int h = plutovg_surface_get_height(g_window->surface);
        drawBitmap(hwnd, hdc, data, w, h);

        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_ERASEBKGND:
        return TRUE;
    case WM_SIZE:
        {
            UINT width = LOWORD(lParam);
            UINT height = HIWORD(lParam);
            g_window->onResize(width, height);
        }
        break;
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP: {
        MotionEventType type = MotionEventType::Press;
        if (msg == WM_LBUTTONUP || msg == WM_RBUTTONUP || msg == WM_MBUTTONUP) {
            type = MotionEventType::Release;
            isMouseDown = false;
        }
        else {
            isMouseDown = true;
        }
        ButtonType button = ButtonType::Left;
        if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP) {
            button = ButtonType::Right;
        }
        else if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP) {
            button = ButtonType::Middle;
        }
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        MotionEvent env;
        env.button = button;
        env.type = type;
        env.x = xPos;
        env.y = yPos;
        g_window->view()->onMotionEvent(env);
        return 0;
    }
    case WM_MOUSEWHEEL:
    {
        MotionEvent env;
        env.type = MotionEventType::Wheel;
        env.delta = -(SHORT)HIWORD(wParam) / (double)WHEEL_DELTA;
        env.x = mouseX;
        env.y = mouseY;
        g_window->view()->onMotionEvent(env);
        return 0;
    }
    case WM_MOUSEMOVE:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        MotionEvent env;
        env.type = isMouseDown ? MotionEventType::TouchMove : MotionEventType::MouseMove;
        env.x = xPos;
        env.y = yPos;
        g_window->view()->onMotionEvent(env);
        mouseX = xPos;
        mouseY = yPos;
        return 0;
    }
    case WM_COMMAND:
    {
        int notifyCode = HIWORD(wParam);
        int controlId = LOWORD(wParam);
        HWND controlHandle = (HWND)lParam;

        switch (controlId)
        {
        case ID_INPUTEDIT:
            if (notifyCode == EN_UPDATE)
            {
                if (g_window->_textInput != nullptr) {
                    char result[256];
                    GetWindowText(g_window->_textInput->textInputHandle, result, 256);
                    std::string utf8 = LocalToUTF8(result);
                    g_window->_textInput->onTextChange(utf8.c_str());
                }
            }
            break;
        }
        break;
    }
    case WM_TIMER:
        if (wParam == 1)
        {
            //KillTimer(hwnd, 1);
            //fireTimeEvents();
        }
        break;

    }//switch
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

WNDCLASS wndClass;


int openWindow(const char* title, int w, int h) {
    g_screenScle = 1;
    g_autoScale = false;

    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "waseWindowClass";
    //wc.style = CS_HREDRAW | CS_VREDRAW;
    //wc.hbrBackground = NULL;

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "Register Window Fail", "Error", MB_ICONERROR);
        return -1;
    }

    HWND hwnd = CreateWindow(
        "waseWindowClass",
        title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        w,
        h,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hwnd) {
        MessageBox(NULL, "Create Window Fail", "Error", MB_ICONERROR);
        return -1;
    }

    g_window->hWnd = hwnd;

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    SetTimer(hwnd, 1, 100, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        fireTimeEvents();
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (g_window->_dirty) {
            g_window->_dirty = false;
            InvalidateRect(hwnd, NULL, false);
        }
    }

    return (int)msg.wParam;
}

