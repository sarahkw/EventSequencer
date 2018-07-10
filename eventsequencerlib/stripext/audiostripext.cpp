#include "audiostripext.h"

#include <eventsequencer.pb.h>

#include <QDebug>

namespace stripext {

AudioStripExt::AudioStripExt(QObject *parent) : QObject(parent)
{

}

QString AudioStripExt::fileName() const
{
    return fileName_;
}

void AudioStripExt::setFileName(const QString &fileName)
{
    if (fileName_ != fileName) {
        fileName_ = fileName;
        emit fileNameChanged();
    }
}

void AudioStripExt::toPb(pb::Strip_Audio &pb) const
{
    pb.set_filename(fileName().toStdString());
}

void AudioStripExt::fromPb(const pb::Strip_Audio &pb)
{
    setFileName(QString::fromStdString(pb.filename()));
}

} // namespace stripext
