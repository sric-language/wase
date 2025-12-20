#include "QtWindow.h"
#include <QGuiApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>
#include <QApplication>

#include "QtTextInput.h"

extern float g_screenScle;
extern bool g_autoScale;

QtWindow::~QtWindow() {
    delete _peer;
    delete _graphics;
}

void QtWindow::init(QtWindowPeer* window, sric::OwnPtr<waseGraphics::View> view) SC_NOTHROW {
    this->_peer = window;
    _view = std::move(view);
    sric::RefPtr<Window> self = sric::rawToRef((Window*)this);
    _view->setHost(self);
}

sric::RefPtr<View> QtWindow::view() SC_NOTHROW {
    return _view;
}

void QtWindow::repaint(Rect& dirty) SC_NOTHROW {
    _peer->update();
}

Size QtWindow::size() SC_NOTHROW {
    Size size;
    size.w = _peer->width();
    size.h = _peer->height();
    return size;
}

bool QtWindow::hasFocus() SC_NOTHROW {
    return _peer->hasFocus();
}

void QtWindow::focus() SC_NOTHROW {
    _peer->setFocus();
}

sric::OwnPtr<TextInput> QtWindow::textInput(int inputType) SC_NOTHROW {
    auto t = sric::new_<QtTextInput>();
    t->init(this->_peer, inputType);
    _textInput = std::move(t);
    return _textInput.share();
}

void QtWindow::fileDialog(bool isOpen, const char* accept) SC_NOTHROW {

}

void QtWindow::displayKeyboard(bool display) SC_NOTHROW {

}

void QtWindow::onResize(int w, int h) SC_NOTHROW {
    _peer->update();
}

//////////////////////////////////////////////////////////////////////////////////////////

static void mouseEventConvert(QMouseEvent* evt, MotionEvent& mouse) {
    float d = 1;//QGuiApplication::primaryScreen()->devicePixelRatio();
    mouse.x = evt->x() * d;
    mouse.y = evt->y() * d;

    if (evt->buttons() & Qt::LeftButton) {
        mouse.button = ButtonType::Left;
    }
    else if (evt->buttons() & Qt::RightButton) {
        mouse.button = ButtonType::Right;
    }
    else if (evt->buttons() & Qt::MiddleButton) {
        mouse.button = ButtonType::Middle;
    }
    else {
        if (evt->button() == Qt::LeftButton) {
            mouse.button = ButtonType::Left;
        }
        else if (evt->button() == Qt::RightButton) {
            mouse.button = ButtonType::Right;
        }
        else if (evt->button() == Qt::MiddleButton) {
            mouse.button = ButtonType::Middle;
        }
    }
}

QtWindowPeer::QtWindowPeer(QWidget *parent) : QWidget(parent) {
    this->setMouseTracking(true);
}

void QtWindowPeer::mousePressEvent(QMouseEvent* evt)
{
    QWidget::mousePressEvent(evt);
    MotionEvent mouse;
    mouse.type = MotionEventType::Press;
    mouseEventConvert(evt, mouse);
    isMouseDown = true;
    _waseWindow->view()->onMotionEvent(mouse);
}

void QtWindowPeer::mouseReleaseEvent(QMouseEvent* evt)
{
    QWidget::mouseReleaseEvent(evt);
    MotionEvent mouse;
    mouse.type = MotionEventType::Release;
    mouseEventConvert(evt, mouse);
    isMouseDown = false;
    _waseWindow->view()->onMotionEvent(mouse);
}

void QtWindowPeer::mouseMoveEvent(QMouseEvent* evt)
{
    QWidget::mouseMoveEvent(evt);
    MotionEvent mouse;
    mouse.type = MotionEventType::MouseMove;
    if (isMouseDown)
    {
        mouse.type = MotionEventType::TouchMove;
    }
    mouseEventConvert(evt, mouse);

    _waseWindow->view()->onMotionEvent(mouse);
}

void QtWindowPeer::wheelEvent(QWheelEvent* evt)
{
    QWidget::wheelEvent(evt);
    MotionEvent mouse;
    mouse.type = MotionEventType::Wheel;
    float d = 1;//QGuiApplication::primaryScreen()->devicePixelRatio();
    mouse.x = evt->position().x() * d;
    mouse.y = evt->position().y() * d;
    mouse.delta = -evt->angleDelta().y() / 120;

    _waseWindow->view()->onMotionEvent(mouse);
}

static void keyEventConvert(QKeyEvent* evt, KeyEvent& key) {
    switch (evt->key()) {
    case Qt::Key_Backspace:
        key.key = Key::Backspace;
        break;
    case Qt::Key_Enter:
        key.key = Key::Enter;
        break;
    case Qt::Key_Left:
        key.key = Key::Left;
        break;
    case Qt::Key_Right:
        key.key = Key::Right;
        break;
    case Qt::Key_Up:
        key.key = Key::Up;
        break;
    case Qt::Key_Down:
        key.key = Key::Down;
        break;
    case Qt::Key_Delete:
        key.key = Key::Delete;
        break;
    default:
        key.key = (Key)evt->key();
        break;
    }
}

void QtWindowPeer::keyPressEvent(QKeyEvent* evt)
{
    QWidget::keyPressEvent(evt);
    KeyEvent key;
    key.keyChar = 0;
    key.type = KeyEventType::Press;
    keyEventConvert(evt, key);
    _waseWindow->view()->onKeyEvent(key);
}

void QtWindowPeer::keyReleaseEvent(QKeyEvent* evt)
{
    QWidget::keyReleaseEvent(evt);
    KeyEvent key;
    key.keyChar = 0;
    key.type = KeyEventType::Release;
    keyEventConvert(evt, key);
    _waseWindow->view()->onKeyEvent(key);
}

void QtWindowPeer::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    _waseWindow->onResize(event->size().width(), event->size().height());
}

void QtWindowPeer::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter painter(this);
    _waseWindow->_graphics = createQtGraphics(_waseWindow->_graphics, &painter);
    _waseWindow->paint(*_waseWindow->_graphics);
    _waseWindow->_graphics = createQtGraphics(_waseWindow->_graphics, nullptr);
}

//////////////////////////////////////////////////////////////////////////////////////////

QWidget* g_curWindowParent = nullptr;
sric::OwnPtr<QtWindow> g_window;
QTimer* g_timer;
void startMainLoopTimer() {
    if (!g_timer) {
        g_timer = new QTimer();
        QObject::connect(g_timer, &QTimer::timeout, [=]() {
            fireTimeEvents();
        });
        g_timer->start(16);
    }
}

void setCurWindowParent(QWidget* parent) {
    g_curWindowParent = parent;
}

int Window::open(sric::OwnPtr<waseGraphics::View> view, const char* name) SC_NOTHROW {
    if (!g_window.isNull()) {
        return -1;
    }

    std::unique_ptr<QCoreApplication> localApp;
    if (!QCoreApplication::instance()) {
        int argc = 0;
        localApp = std::unique_ptr<QCoreApplication>(new QApplication (argc, NULL));
    }

    g_screenScle = 1;//QGuiApplication::primaryScreen()->devicePixelRatio();
    g_autoScale = false;

    auto size = view->getPrefSize(1024, 768);
    g_window = sric::new_<QtWindow>();

    QtWindowPeer* widget = new QtWindowPeer(g_curWindowParent);
    widget->setWindowTitle(QString::fromUtf8(name));
    widget->resize(size.w, size.h);
    widget->_waseWindow = g_window.get();
    g_window->init(widget, std::move(view));

    widget->show();

    startMainLoopTimer();

    if (localApp.get()) {
        return localApp->exec();
    }
    return 0;
}

sric::RefPtr<Window> Window::getCur() SC_NOTHROW {
    return g_window;
}
