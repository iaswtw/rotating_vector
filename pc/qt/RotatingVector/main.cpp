#include "mainwindow.h"
#include "controlwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication a(argc, argv);
    MainWindow w;
    ControlWindow cw(&w, &w);

    w.setControlWindow(&cw);        // give control window handle to main window

    w.show();
    cw.show();

    return a.exec();
}
