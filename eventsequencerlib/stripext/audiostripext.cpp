#include "audiostripext.h"

#include <eventsequencer.pb.h>

namespace stripext {

AudioStripExt::AudioStripExt(QObject *parent) : QObject(parent)
{

}

void AudioStripExt::toPb(pb::Strip_Audio &pb) const
{
}

void AudioStripExt::fromPb(const pb::Strip_Audio &pb)
{
}

} // namespace stripext
