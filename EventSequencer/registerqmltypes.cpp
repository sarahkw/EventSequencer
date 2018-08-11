#include "registerqmltypes.h"

#include "document.h"
#include "strip.h"
#include "framesandseconds.h"
#include "channel/channeltype.h"
#include "waitfor.h"
#include "stripext/audiostripext.h"
#include "stripext/badjsstripext.h"
#include "stripext/textstripext.h"
#include "stripext/labelstripext.h"
#include "stripext/playliststripext.h"
#include "constrainedmetricsfontutil.h"
#include "wordwrappedtexttrack.h"
#include "showmultiplelinesononeline.h"
#include "recordercontrol.h"
#include "session.h"
#include "managedresources.h"
#include "playercontrol.h"
#include "channel/collatechannel.h"
#include "channelindexfactory.h"
#include "countchannelindex.h"

#include <QQmlEngine>

void RegisterQmlTypes::registerQmlTypes()
{
    qmlRegisterType<Document>("eventsequencer", 1, 0, "Document");
    qmlRegisterUncreatableType<Strip>("eventsequencer", 1, 0, "Strip", "Created on C++ side");
    qmlRegisterUncreatableType<channel::ChannelType>("eventsequencer", 1, 0, "ChannelType", "Only used for enumeration");
    qmlRegisterUncreatableType<WaitFor>("eventsequencer", 1, 0, "WaitFor", "Created on C++ side");
    qmlRegisterType<stripext::AudioStripExt>();
    qmlRegisterType<stripext::BadJsStripExt>();
    qmlRegisterType<stripext::TextStripExt>();
    qmlRegisterType<stripext::LabelStripExt>();
    qmlRegisterType<stripext::PlaylistStripExt>();
    qmlRegisterSingletonType<FramesAndSeconds>(
        "eventsequencer", 1, 0, "FramesAndSeconds",
        [](QQmlEngine*, QJSEngine*) -> QObject* {
            return new FramesAndSeconds;
        });
    qmlRegisterSingletonType<ChannelIndexFactory>(
        "eventsequencer", 1, 0, "ChannelIndexFactory",
        [](QQmlEngine*, QJSEngine*) -> QObject* {
            return new ChannelIndexFactory;
        });
    qmlRegisterType<ConstrainedMetricsFontUtil>("eventsequencer", 1, 0, "ConstrainedMetricsFontUtil");
    qmlRegisterType<WordWrappedTextTrack>("eventsequencer", 1, 0, "WordWrappedTextTrack");
    qmlRegisterType<ShowMultipleLinesOnOneLine>("eventsequencer", 1, 0, "ShowMultipleLinesOnOneLine");
    qmlRegisterType<RecorderControl>("eventsequencer", 1, 0, "RecorderControl");
    qmlRegisterType<Session>("eventsequencer", 1, 0, "Session");
    qmlRegisterType<ManagedResources>("eventsequencer", 1, 0, "ManagedResources");
    qmlRegisterType<PlayerControl>("eventsequencer", 1, 0, "PlayerControl");
    qmlRegisterUncreatableType<channel::CollateChannel>("eventsequencer", 1, 0, "CollateChannel", "Only used for enumeration");
    qmlRegisterType<CountChannelIndex>("eventsequencer", 1, 0, "CountChannelIndex");
}
