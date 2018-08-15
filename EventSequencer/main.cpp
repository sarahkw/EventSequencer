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

    bool scoutMode = false;
    {
        QCommandLineParser parser;
        parser.addHelpOption();
        parser.addOptions({{{"s", "scout"}, "Start EvSeq Scout"}});
        parser.process(app);
        if (parser.isSet("scout")) {
            scoutMode = true;
        }
    }

    // For now, scoutMode is used to prototype Android app.
    if (!scoutMode) {
        QQuickStyle::setStyle("Fusion");
    }

    QQmlApplicationEngine engine;
    if (scoutMode) {
        engine.load(QUrl(QStringLiteral("qrc:/ScoutMain.qml")));
    } else {
        engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    }
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
