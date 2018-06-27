#include "registerqmltypes.h"

#include "document.h"
#include "strip.h"

#include <QQmlEngine>

void RegisterQmlTypes::registerQmlTypes()
{
    qmlRegisterType<Document>("eventsequencer", 1, 0, "Document");
    qmlRegisterType<Strip>();
}
