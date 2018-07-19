#ifndef COLLATECHANNELREFRESHEVENT_H
#define COLLATECHANNELREFRESHEVENT_H

#include <QEvent>

namespace channel {

class CollateChannelRefreshEvent : public QEvent
{
public:
    static const QEvent::Type s_CustomType;
    CollateChannelRefreshEvent();
};

} // namespace channel

#endif // COLLATECHANNELREFRESHEVENT_H
