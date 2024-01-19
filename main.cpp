#include "mainwindow.h"
#include "cardstatistics.h"
#include "filter.h"
#include "logparser.h"
#include "mousetracker.h"

#include <QApplication>
#include <QQuickView>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQuickView view(QUrl("qrc:/ui/root.qml"));

    QSurfaceFormat surfaceFormat;
    surfaceFormat.setAlphaBufferSize(8);
    view.setFormat(surfaceFormat);
    view.setColor(QColor(Qt::transparent));
    view.setFlag(Qt::FramelessWindowHint);
    view.setFlag(Qt::WindowStaysOnTopHint);
    view.setFlag(Qt::WindowTransparentForInput);
    view.showFullScreen();

    MouseTracker tracker;
    LogParser logParser("C:\\Users\\hagen\\AppData\\LocalLow\\Wizards Of The Coast\\MTGA");

    CardStatisticsDatabase cardDB;
    cardDB.addSet("LCI");

    return app.exec();
}
