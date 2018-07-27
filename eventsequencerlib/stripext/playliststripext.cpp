#include "playliststripext.h"

#include <eventsequencer.pb.h>

namespace stripext {

PlaylistStripExt::PlaylistStripExt(QObject *parent) : QObject(parent)
{

}

void PlaylistStripExt::toPb(pb::Strip_Playlist &pb) const
{
}

void PlaylistStripExt::fromPb(const pb::Strip_Playlist &pb)
{
}

} // namespace stripext
