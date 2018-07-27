#include "badaudiostripext.h"

#include <eventsequencer.pb.h>

namespace stripext {

BadAudioStripExt::BadAudioStripExt(QObject *parent) : QObject(parent)
{

}

void BadAudioStripExt::toPb(pb::Strip_BadAudio &pb) const
{
}

void BadAudioStripExt::fromPb(const pb::Strip_BadAudio &pb)
{
}

} // namespace stripext
