#include <QApplication>
#include <QQmlApplicationEngine>

#include "registerqmltypes.h"

int main(int argc, char *argv[])
{
    RegisterQmlTypes::registerQmlTypes();

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
