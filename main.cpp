#include "mainwindow.h"
#include "filter.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    Filter filter;
//    a.installEventFilter(&filter);

    OverlayWindow w;
    w.show();
    return a.exec();
}
