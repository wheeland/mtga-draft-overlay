#include "cardlayout.h"
#include "carddb.h"
#include "draftmodel.h"
#include "logparser.h"
#include "inputtracker.h"

#include <QApplication>
#include <QQmlContext>
#include <QQuickView>
#include <QScreen>

int main(int argc, char *argv[])
{
    qputenv("QT_ENABLE_HIGHDPI_SCALING", "0");

    QApplication app(argc, argv);
    app.setApplicationName("MTGA-DraftOverlay");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("de.wheeland");

    CardLayout layout;
    InputTracker tracker;
    LogParser logParser("C:\\Users\\hagen\\AppData\\LocalLow\\Wizards Of The Coast\\MTGA");

    CardDatabase cardDB;

    DraftModel model(&cardDB);
    QObject::connect(&logParser, &LogParser::draftPack, &model, &DraftModel::onDraftPack);
    logParser.startParsing();

    QSurfaceFormat surfaceFormat;
    surfaceFormat.setAlphaBufferSize(8);

    QQuickView view;
    view.rootContext()->setContextProperty("_app", &app);
    view.rootContext()->setContextProperty("_cardDB", &cardDB);
    view.rootContext()->setContextProperty("_draftModel", &model);
    view.rootContext()->setContextProperty("_inputTracker", &tracker);
    view.rootContext()->setContextProperty("_layout", &layout);
    view.setSource((QUrl("qrc:/ui/root.qml")));

    view.setFormat(surfaceFormat);
    view.setColor(QColor(Qt::transparent));
    view.setFlag(Qt::FramelessWindowHint);
    view.setFlag(Qt::WindowStaysOnTopHint);
    view.setFlag(Qt::WindowTransparentForInput);
    view.showFullScreen();

    layout.setSize(view.size());

    return app.exec();
}
