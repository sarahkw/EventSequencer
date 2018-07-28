#include "playliststripext.h"

#include <eventsequencer.pb.h>

namespace stripext {

PlaylistStripExt::PlaylistStripExt(QObject *parent) : QObject(parent)
{

}

void PlaylistStripExt::toPb(pb::Strip_Playlist &pb) const
{
    pb.set_mediaframeskipbegin(mediaFrameSkipBegin_);
    pb.set_mediaframeskipend(mediaFrameSkipEnd_);
}

void PlaylistStripExt::fromPb(const pb::Strip_Playlist &pb)
{
    setMediaFrameSkipBegin(pb.mediaframeskipbegin());
    setMediaFrameSkipEnd(pb.mediaframeskipend());
}

unsigned int PlaylistStripExt::mediaFrameSkipBegin() const
{
    return mediaFrameSkipBegin_;
}

void PlaylistStripExt::setMediaFrameSkipBegin(unsigned int mediaFrameSkipBegin)
{
    if (mediaFrameSkipBegin_ != mediaFrameSkipBegin) {
        mediaFrameSkipBegin_ = mediaFrameSkipBegin;
        emit mediaFrameSkipBeginChanged();
    }
}

unsigned int PlaylistStripExt::mediaFrameSkipEnd() const
{
    return mediaFrameSkipEnd_;
}

void PlaylistStripExt::setMediaFrameSkipEnd(unsigned int mediaFrameSkipEnd)
{
    if (mediaFrameSkipEnd_ != mediaFrameSkipEnd) {
        mediaFrameSkipEnd_ = mediaFrameSkipEnd;
        emit mediaFrameSkipEndChanged();
    }
}


} // namespace stripext
