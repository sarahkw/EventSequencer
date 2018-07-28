#include "audiostripext.h"

#include <eventsequencer.pb.h>

namespace stripext {

AudioStripExt::AudioStripExt(QObject *parent) : QObject(parent)
{

}

void AudioStripExt::toPb(pb::Strip_Audio &pb) const
{
    pb.set_mediaframeskipbegin(mediaFrameSkipBegin_);
}

void AudioStripExt::fromPb(const pb::Strip_Audio &pb)
{
    setMediaFrameSkipBegin(pb.mediaframeskipbegin());
}

unsigned int AudioStripExt::mediaFrameSkipBegin() const
{
    return mediaFrameSkipBegin_;
}

void AudioStripExt::setMediaFrameSkipBegin(unsigned int mediaFrameSkipBegin)
{
    if (mediaFrameSkipBegin_ != mediaFrameSkipBegin) {
        mediaFrameSkipBegin_ = mediaFrameSkipBegin;
        emit mediaFrameSkipBeginChanged();
    }
}

} // namespace stripext
