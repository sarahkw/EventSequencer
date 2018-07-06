#include "channeltypeconversions.h"

#include <QtGlobal>

namespace channel {

pb::ChannelData::ChannelCase ChannelTypeConversions::ToPb(ChannelType::Enum ct)
{
    switch (ct) {
    case ChannelType::BadClock: return pb::ChannelData::kBadClock;
    case ChannelType::BadJs: return pb::ChannelData::kBadJs;
    case ChannelType::Text: return pb::ChannelData::kText;
    }
    Q_ASSERT(false);
}

ChannelType::Enum ChannelTypeConversions::ToCc(pb::ChannelData::ChannelCase cc)
{
    switch (cc) {
    case pb::ChannelData::ChannelCase::kBadClock: return ChannelType::BadClock;
    case pb::ChannelData::ChannelCase::kBadJs: return ChannelType::BadJs;
    case pb::ChannelData::ChannelCase::kText: return ChannelType::Text;
    case pb::ChannelData::ChannelCase::CHANNEL_NOT_SET: Q_ASSERT(false);
    }
    Q_ASSERT(false);
}

} // namespace channel
