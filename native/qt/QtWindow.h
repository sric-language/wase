#ifndef QTWINDOW_H
#define QTWINDOW_H

#include "QtGraphics.h"
#include "Window.h"
#include <QWidget>

using namespace waseGraphics;

class QtWindow;

class QtWindowPeer : public QWidget {
    //Q_OBJECT
    bool isMouseDown = false;
public:
    QtWindow *_waseWindow = nullptr;
    QtWindowPeer(QWidget *parent = nullptr);
protected:
    /**
     * @see QWidget::mousePressEvent
     */
    void mousePressEvent(QMouseEvent* evt) override;

    /**
     * @see QWidget::mouseReleaseEvent
     */
    void mouseReleaseEvent(QMouseEvent* evt) override;

    /**
     * @see QWidget::mouseMoveEvent
     */
    void mouseMoveEvent(QMouseEvent* evt) override;

    /**
     * @see QWidget::mouseWheelEvent
     */
    void wheelEvent(QWheelEvent* evt) override;

    /**
     * @see QWidget::keyPressEvent
     */
    void keyPressEvent(QKeyEvent* evt) override;

    /**
     * @see QWidget::keyReleaseEvent
     */
    void keyReleaseEvent(QKeyEvent* evt) override;

    void resizeEvent(QResizeEvent *event) override;

    void paintEvent(QPaintEvent *event) override;
};

class QtWindow : public Window
{
public:
    QtWindowPeer* _peer = nullptr;
    Graphics* _graphics = nullptr;
private:
    sric::OwnPtr<waseGraphics::View> _view;
    sric::OwnPtr<TextInput> _textInput;
public:
    ~QtWindow();

    void init(QtWindowPeer* widget, sric::OwnPtr<waseGraphics::View> view) SC_NOTHROW;

    sric::RefPtr<View> view() SC_NOTHROW;

    void repaint(Rect& dirty) SC_NOTHROW;

    Size size() SC_NOTHROW;

    bool hasFocus() SC_NOTHROW;

    void focus() SC_NOTHROW;

    sric::OwnPtr<TextInput> textInput(int inputType) SC_NOTHROW;

    void fileDialog(bool isOpen, const char* accept) SC_NOTHROW;

    void displayKeyboard(bool display) SC_NOTHROW;

    void onResize(int w, int h) SC_NOTHROW;
};

void startMainLoopTimer();

#endif // QTWINDOW_H
