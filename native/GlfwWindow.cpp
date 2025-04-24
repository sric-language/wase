
#include <stdio.h>

#if __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#include "WebTextInput.h"
#endif

//#ifdef NANOVG_GLEW
#define GLEW_STATIC
#include <GL/glew.h>
//#endif

#ifdef _WIN32
	#include "Win32TextInput.h"
#endif

#ifdef __APPLE__
	#define GLFW_INCLUDE_GLCOREARB
#endif
#define GLFW_INCLUDE_GLEXT

#include <GLFW/glfw3.h>

#ifdef _WIN32
	#define GLFW_EXPOSE_NATIVE_WIN32
	#include <GLFW/glfw3native.h>
#endif

#include "nanovg.h"
#define NANOVG_GLES3_IMPLEMENTATION
#include "nanovg_gl.h"

#include "NanovgGraphics.h"
#include "Window.h"

using namespace waseGraphics;

extern float g_screenScle;
extern bool g_autoScale;

#ifdef _WIN32
	std::string LocalToUTF8(const std::string& localStr);
	LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

class GlfwWindow : public Window
{
public:
	GLFWwindow* window = NULL;
	NVGcontext* vg = NULL;
	Graphics* graphics = NULL;
	sric::OwnPtr<waseGraphics::View> _view;
	Size _size;
	sric::OwnPtr<TextInput> _textInput;

	void init(GLFWwindow* window, NVGcontext* vg, sric::OwnPtr<waseGraphics::View> view) {
		this->window = window;
		this->vg = vg;
		graphics = waseGraphics::createNanovgGraphics(vg);
		_view = std::move(view);
		sric::RefPtr<Window> self = sric::rawToRef((Window*)this);
		_view->setHost(self);
	}

	sric::RefPtr<View> view() {
		return _view;
	}

	void repaint(Rect& dirty) {

	}

	Size size() {
		return _size;
	}

	bool hasFocus() {
		return true;
	}

	void focus() {

	}

	sric::OwnPtr<TextInput> textInput(int inputType) {
#ifdef _WIN32
		sric::OwnPtr<Win32TextInput> textInput = sric::new_<Win32TextInput>();
		textInput->custemProc = (LONG_PTR)EditSubclassProc;
		HWND hwnd = glfwGetWin32Window(this->window);
		textInput->init(hwnd, inputType);
		_textInput = std::move(textInput);
		return _textInput.share();
#elif defined(__EMSCRIPTEN__)
		sric::OwnPtr<WebTextInput> textInput = sric::new_<WebTextInput>();
		textInput->init(inputType);
		_textInput = std::move(textInput);
		return _textInput.share();
#else
		return sric::OwnPtr<TextInput>();
#endif
	}

	void fileDialog(bool isOpen, const char* accept) {

	}

	void displayKeyboard(bool display) {

	}

	void onResize(int w, int h) {
		_size.w = w;
		_size.h = h;
	}
};

sric::OwnPtr<GlfwWindow> g_window;

int openWindow(const char* title, int w, int h, sric::OwnPtr<waseGraphics::View> view);


int Window::open(sric::OwnPtr<waseGraphics::View> view, const char* name) {
	if (!g_window.isNull()) {
		return -1;
	}
	auto size = view->getPrefSize(1024, 768);
	g_window = sric::new_<GlfwWindow>();
	return openWindow(name, size.w, size.h, std::move(view));
}

sric::RefPtr<Window> Window::getCur() {
	return g_window;
}

void renderGraph(NVGcontext* vg)
{
	float w, h;
	float x = 0;
	float y = 0;
	w = 200;
	h = 35;


	int font = nvgCreateFont(vg, "fallback", "C:/Windows/Fonts/msyh.ttc");
	nvgAddFallbackFontId(vg, font, font);

	//nvgBeginPath(vg);
	//nvgRect(vg, x, y, w, h);
	//nvgFillColor(vg, nvgRGBA(255, 0, 0, 192));
	//nvgFill(vg);

	nvgFontFace(vg, "fallback");
	nvgFontSize(vg, 20.0f);
	nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	nvgFillColor(vg, nvgRGBA(240, 240, 240, 192));
	nvgText(vg, x + 100, y + 100, "Hello", NULL);
}


void errorcb(int error, const char* desc)
{
	printf("GLFW error %d: %s\n", error, desc);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	NVG_NOTUSED(scancode);
	NVG_NOTUSED(mods);
}

void character_callback(GLFWwindow* window, unsigned int codepoint)
{
}

float lastXScale = 0;
float lastYScale = 0;

static void getContentScale(GLFWwindow* window) {
	if (lastXScale == 0) {
		float xscale, yscale;
		glfwGetWindowContentScale(window, &xscale, &yscale);
		lastXScale = xscale;
		lastYScale = yscale;
	}
}

static void getCursorPosPixel(GLFWwindow* window, double* x, double* y) {
	glfwGetCursorPos(window, x, y);
#if defined(_WIN32) || defined(__EMSCRIPTEN__)
#else
	getContentScale(window);
	*x *= lastXScale;
	*y *= lastYScale;
#endif
}

static void cursor_position_callback(GLFWwindow* window, double x, double y)
{
#if defined(_WIN32) || defined(__EMSCRIPTEN__)
#else
	getContentScale(window);
	x *= lastXScale;
	y *= lastYScale;
#endif

	MotionEvent env;
	env.type = MotionEventType::MouseMove;
	env.x = x;
	env.y = y;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		env.type = MotionEventType::TouchMove;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		env.type = MotionEventType::TouchMove;
		env.button = ButtonType::Right;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
		env.type = MotionEventType::TouchMove;
		env.button = ButtonType::Middle;
	}

	g_window->view()->onMotionEvent(env);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	double x, y;
	getCursorPosPixel(window, &x, &y);

	MotionEvent env;
	//env.button = button;
	env.type = (action == GLFW_PRESS) ? MotionEventType::Press : MotionEventType::Release;
	env.x = x;
	env.y = y;

	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		env.button = ButtonType::Left;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		env.button = ButtonType::Right;
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
		env.button = ButtonType::Middle;
	}

	g_window->view()->onMotionEvent(env);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	double x, y;
	getCursorPosPixel(window, &x, &y);

	int wheelDelta;
#if __APPLE__
	int delta = yoffset * 10;
	wheelDelta = delta;
#else
	wheelDelta = yoffset;
#endif

	MotionEvent env;
	env.type = MotionEventType::Wheel;
	env.delta = -wheelDelta;
	env.x = x;
	env.y = y;
	g_window->view()->onMotionEvent(env);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	g_window->onResize(width, height);
}

#ifdef _WIN32
extern WNDPROC oldEditProc;

LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT res = CallWindowProc(oldEditProc, hWnd, uMsg, wParam, lParam);
	static char lastText[1024] = { 0 };
	if (uMsg == WM_CHAR || uMsg == WM_KEYDOWN ||
		uMsg == WM_PASTE || uMsg == WM_CUT ||
		uMsg == WM_CLEAR || uMsg == WM_SETTEXT)
	{
		if (g_window->_textInput != nullptr) {
			Win32TextInput* textInput = dynamic_cast<Win32TextInput*>(g_window->_textInput.get());
			if (textInput && textInput->textInputHandle) {
				char result[256];
				if (GetWindowText(textInput->textInputHandle, result, 256)) {
					std::string utf8 = LocalToUTF8(result);
					g_window->_textInput->onTextChange(utf8.c_str());
				}
			}
		}
	}

	return res;
}
#endif

static bool doFrame(double time, void* userData) {
	GLFWwindow* window = g_window->window;
	NVGcontext* vg = g_window->vg;

	double mx, my;
	int winWidth, winHeight;
	int fbWidth, fbHeight;
	float pxRatio;
	int i, n;

	fireTimeEvents();

	glfwGetCursorPos(window, &mx, &my);
	glfwGetWindowSize(window, &winWidth, &winHeight);
	glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
	// Calculate pixel ration for hi-dpi devices.
	pxRatio = 1;//(float)fbWidth / (float)winWidth;

	// Update and render
	glViewport(0, 0, fbWidth, fbHeight);
	glClearColor(0,0,0,0);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	nvgBeginFrame(vg, winWidth, winHeight, pxRatio);

	
	//renderGraph(vg);
	g_window->_size.w = fbWidth;
	g_window->_size.h = fbHeight;
	g_window->paint(*g_window->graphics);

	nvgEndFrame(vg);

	glfwSwapBuffers(window);

	glfwPollEvents();

	return true;
}

#ifdef __EMSCRIPTEN__
EM_JS(bool, isMobile, (), {
	if (/Mobi|Android|iPhone/i.test(navigator.userAgent)) {
		return true;
	}
	return false;
  });
#else
  bool isMobile() {
	return false;
  }
#endif

int openWindow(const char* title, int w, int h, sric::OwnPtr<waseGraphics::View> view)
{
	GLFWwindow* window = NULL;
	NVGcontext* vg = NULL;

	if (!glfwInit()) {
		printf("Failed to init GLFW.");
		return -1;
	}

	glfwSetErrorCallback(errorcb);

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	glfwWindowHint(GLFW_SAMPLES, 4);

	window = glfwCreateWindow(w, h, title, NULL, NULL);
	if (!window) {
#ifdef __EMSCRIPTEN__
        EM_ASM(
            alert('Your browser does not support WebGL 2.0');
        );
#endif
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCharCallback(window, character_callback);

	glfwMakeContextCurrent(window);
//#ifdef NANOVG_GLEW
	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK) {
		printf("Could not init glew.\n");
		return -1;
	}
	// GLEW generates GL error because it calls glGetString(GL_EXTENSIONS), we'll consume it here.
	glGetError();
//#endif

	vg = nvgCreateGLES3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	if (vg == NULL) {
		printf("Could not init nanovg.\n");
		return -1;
	}
	g_window->init(window, vg, std::move(view));

	glfwSwapInterval(1);
	glfwSetTime(0);

	getContentScale(window);
	g_screenScle = lastXScale;
	g_autoScale = isMobile();


#if __EMSCRIPTEN__
    emscripten_request_animation_frame_loop(doFrame, NULL);
#else
    while (!glfwWindowShouldClose(window))
    {
        // If we are done, then exit.
        if (!doFrame(0, NULL))
            break;
    }

	nvgDeleteGLES3(vg);
	glfwTerminate();
#endif

	return 0;
}

