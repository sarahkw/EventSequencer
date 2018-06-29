#include "registerqmltypes.h"

#include "document.h"
#include "strip.h"
#include "framesandseconds.h"

#include <QQmlEngine>

void RegisterQmlTypes::registerQmlTypes()
{
    qmlRegisterType<Document>("eventsequencer", 1, 0, "Document");
    qmlRegisterType<Strip>();
    qmlRegisterSingletonType<FramesAndSeconds>(
        "eventsequencer", 1, 0, "FramesAndSeconds",
        [](QQmlEngine*, QJSEngine*) -> QObject* {
            return new FramesAndSeconds;
        });
}
