#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QCommandLineParser>

#include "registerqmltypes.h"

int main(int argc, char *argv[])
{
    RegisterQmlTypes::registerQmlTypes();

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    bool launcherMode = false;
    {
        QCommandLineParser parser;
        parser.addHelpOption();
        parser.addOptions({{{"l", "launcher"}, "Start program launcher"}});
        parser.process(app);
        if (parser.isSet("launcher")) {
            launcherMode = true;
        }
    }

    // For now, launcherMode is used to prototype Android app.
    if (!launcherMode) {
        QQuickStyle::setStyle("Fusion");
    }

    QQmlApplicationEngine engine;
    if (launcherMode) {
        engine.load(QUrl(QStringLiteral("qrc:/LauncherMain.qml")));
    } else {
        engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    }
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
