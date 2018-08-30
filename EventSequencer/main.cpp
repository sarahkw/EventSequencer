#include <QtGlobal> // This is what would provide Q_OS_ANDROID!
#ifdef Q_OS_ANDROID
#include <QGuiApplication>
#else
// TODO When porting to other stuff like Windows I guess we should avoid widgets
//      on Windows too.
#include <QApplication>
#endif

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

//    // Trying to hunt down a bug that happens intermittently. At this
//    // point I don't have a custom AndroidManifest.xml in order to add
//    // this option there.
//#ifdef Q_OS_ANDROID
//#ifndef QT_DEBUG
//#error "Debugging hack should only be in debug builds"
//#endif
//    argc = 2;
//    static const char* hacked_argv[] = {
//        "org.qtproject.example.EventSequencer",
//        "-qmljsdebugger=port:9991,host:127.0.0.1"
//    };
//    argv = const_cast<char**>(&hacked_argv[0]);
//#endif

#ifdef Q_OS_ANDROID
    QGuiApplication app(argc, argv);
#else
    // I think(!) this was for QWidgets to draw the file browser dialogs on
    // Linux.
    QApplication app(argc, argv);
#endif

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
