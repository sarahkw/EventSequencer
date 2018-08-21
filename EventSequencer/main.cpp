#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QCommandLineParser>
#include <QQmlPropertyMap>
#include <QQmlContext>
#include <QUrl>

#include "registerqmltypes.h"

int main(int argc, char *argv[])
{
    RegisterQmlTypes::registerQmlTypes();

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    bool scoutMode = false;
    QQmlPropertyMap commandLine;
#ifdef Q_OS_ANDROID
    scoutMode = true;
#else
    {
        QCommandLineParser parser;
        parser.addHelpOption();
        parser.addOptions({{{"s", "scout"}, "Start EvSeq Scout"}});
        parser.addPositionalArgument("file", "The file to open");
        parser.process(app);
        if (parser.isSet("scout")) {
            scoutMode = true;
        }
        auto positionalArgs = parser.positionalArguments();
        commandLine["file"] = positionalArgs.size() > 0 ? QUrl::fromLocalFile(positionalArgs[0]) : QVariant();
    }
#endif

    // Fusion is a desktop-looking theme. Scout mode by nature shouldn't look
    // like a desktop application, because if you're scouting, you should be
    // mobile.
    if (!scoutMode) {
        QQuickStyle::setStyle("Fusion");
    }

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("commandLine", &commandLine);
    if (scoutMode) {
        engine.load(QUrl(QStringLiteral("qrc:/ScoutMain.qml")));
    } else {
        engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    }
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
