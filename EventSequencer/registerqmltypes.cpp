#include "registerqmltypes.h"

#include "document.h"
#include "strip.h"
#include "framesandseconds.h"
#include "stripext/badjs.h"
#include "channel/channeltype.h"
#include "waitfor.h"

#include <QQmlEngine>

void RegisterQmlTypes::registerQmlTypes()
{
    qmlRegisterType<Document>("eventsequencer", 1, 0, "Document");
    qmlRegisterUncreatableType<Strip>("eventsequencer", 1, 0, "Strip", "Created on C++ side");
    qmlRegisterUncreatableType<stripext::BadJs>("eventsequencer.stripext", 1, 0, "BadJs", "Created on C++ side");
    qmlRegisterUncreatableType<channel::ChannelType>("eventsequencer", 1, 0, "ChannelType", "Only used for enumeration");
    qmlRegisterUncreatableType<WaitFor>("eventsequencer", 1, 0, "WaitFor", "Created on C++ side");
    qmlRegisterSingletonType<FramesAndSeconds>(
        "eventsequencer", 1, 0, "FramesAndSeconds",
        [](QQmlEngine*, QJSEngine*) -> QObject* {
            return new FramesAndSeconds;
        });
}
