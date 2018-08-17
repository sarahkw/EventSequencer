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
#ifdef Q_OS_ANDROID
    scoutMode = true;
#else
    {
        QCommandLineParser parser;
        parser.addHelpOption();
        parser.addOptions({{{"s", "scout"}, "Start EvSeq Scout"}});
        parser.process(app);
        if (parser.isSet("scout")) {
            scoutMode = true;
        }
    }
#endif

    // Fusion is a desktop-looking theme. Scout mode by nature shouldn't look
    // like a desktop application, because if you're scouting, you should be
    // mobile.
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
