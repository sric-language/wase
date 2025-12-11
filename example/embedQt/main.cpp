#include <QApplication>
#include <QWidget>

#include "waseGui.h"
using namespace sric;
using namespace waseGui;

void setCurWindowParent(QWidget* parent);

int32_t main(int argc, char *argv[]) {
    QApplication app(argc, argv);

#if 1
    QWidget* mainWindow = new QWidget();
    mainWindow->showMaximized();
    mainWindow->show();
    setCurWindowParent(mainWindow);
#endif

    auto frame = new_<Frame>();
    {
        auto it = new_<Button>();
        it->setText("Button");
        it->onClick = ([=](RefPtr<Widget> w) {
            Toast::showText("hello world");
        });
        frame->add(std::move(it));
    }

    frame->show();

    return app.exec();
}
