#include "collatechannelrefreshevent.h"

namespace channel {

const QEvent::Type CollateChannelRefreshEvent::s_CustomType =
        static_cast<QEvent::Type>(QEvent::registerEventType());

CollateChannelRefreshEvent::CollateChannelRefreshEvent()
    : QEvent(s_CustomType)
{

}

} // namespace channel
