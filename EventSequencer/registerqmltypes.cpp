#include "registerqmltypes.h"

#include "document.h"
#include "strip.h"
#include "framesandseconds.h"
#include "channel/channeltype.h"
#include "waitfor.h"
#include "stripext/badaudiostripext.h"
#include "stripext/badjsstripext.h"
#include "stripext/textstripext.h"
#include "stripext/labelstripext.h"
#include "constrainedmetricsfontutil.h"
#include "wordwrappedtexttrack.h"
#include "showmultiplelinesononeline.h"
#include "resourcecreatorcontrol.h"

#include <QQmlEngine>

void RegisterQmlTypes::registerQmlTypes()
{
    qmlRegisterType<Document>("eventsequencer", 1, 0, "Document");
    qmlRegisterUncreatableType<Strip>("eventsequencer", 1, 0, "Strip", "Created on C++ side");
    qmlRegisterUncreatableType<channel::ChannelType>("eventsequencer", 1, 0, "ChannelType", "Only used for enumeration");
    qmlRegisterUncreatableType<WaitFor>("eventsequencer", 1, 0, "WaitFor", "Created on C++ side");
    qmlRegisterType<stripext::BadAudioStripExt>();
    qmlRegisterType<stripext::BadJsStripExt>();
    qmlRegisterType<stripext::TextStripExt>();
    qmlRegisterType<stripext::LabelStripExt>();
    qmlRegisterSingletonType<FramesAndSeconds>(
        "eventsequencer", 1, 0, "FramesAndSeconds",
        [](QQmlEngine*, QJSEngine*) -> QObject* {
            return new FramesAndSeconds;
        });
    qmlRegisterType<ConstrainedMetricsFontUtil>("eventsequencer", 1, 0, "ConstrainedMetricsFontUtil");
    qmlRegisterType<WordWrappedTextTrack>("eventsequencer", 1, 0, "WordWrappedTextTrack");
    qmlRegisterType<ShowMultipleLinesOnOneLine>("eventsequencer", 1, 0, "ShowMultipleLinesOnOneLine");
    qmlRegisterType<ResourceCreatorControl>("eventsequencer", 1, 0, "ResourceCreatorControl");
}
