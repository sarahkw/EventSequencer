#ifndef NODATAPB_H
#define NODATAPB_H

#include <functional>
#include <eventsequencer.pb.h>
#include <QDebug>
#include "ipb.h"

namespace channel {

template <typename T, T* (pb::ChannelData::*First)(), bool (pb::ChannelData::*Second)() const>
class NoDataPb : public virtual IPb {
public:

    virtual void toPb(pb::ChannelData& pb) const override
    {
        (pb.*First)();
    }

    virtual void fromPb(const pb::ChannelData &pb)
    {
        Q_ASSERT((pb.*Second)());
    }

};

} // namespace channel

#endif // NODATAPB_H
