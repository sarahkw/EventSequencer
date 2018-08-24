#include "singleurl.h"

#include <concatiodevice.h>

#include <memory>

namespace playable {

SingleUrl::SingleUrl(QObject *parent) : PlayableBase(parent)
{
}

QIODevice *SingleUrl::createPlayableDevice(const QAudioFormat& outputFormat)
{
    std::unique_ptr<ConcatIODevice> playingDevice(new ConcatIODevice);
    if (!appendUrlToConcatIODevice(*playingDevice, singleUrl_, outputFormat)) {
        return nullptr;
    }
    return playingDevice.release();
}

QString SingleUrl::singleUrl() const
{
    return singleUrl_;
}

void SingleUrl::setSingleUrl(const QString &singleUrl)
{
    if (singleUrl_ != singleUrl) {
        singleUrl_ = singleUrl;
        emit singleUrlChanged();
    }
}

} // namespace playable
